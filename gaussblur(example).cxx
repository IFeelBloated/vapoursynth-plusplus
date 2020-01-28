#include "Frame.hxx"
#include "VapourSynth.h"
#include "VSHelper.h"

struct FilterData final {
	static constexpr auto filterName = "test";
	self(node, static_cast<VSNodeRef*>(nullptr));
	self(vi, static_cast<const VSVideoInfo*>(nullptr));
	~FilterData() {
		if (node != nullptr)
			VaporGlobals::API->freeNode(node);
	}
	auto CheckFormat() {
		auto errmsg = filterName + ": only single precision floating point clips with constant format and dimensions supported."s;
		if (vi->format == nullptr || vi->width == 0 || vi->height == 0 || vi->format->sampleType != stFloat || vi->format->bitsPerSample != 32) {
			VaporGlobals::API->setError(VaporGlobals::Outputs, errmsg.data());
			return false;
		}
		return true;
	}
	auto Initialize() {
		node = VaporGlobals::API->propGetNode(VaporGlobals::Arguments, "clip", 0, nullptr);
		vi = VaporGlobals::API->getVideoInfo(node);
		if (auto format_status = CheckFormat(); format_status == false)
			return false;
		return true;
	}
};

auto FilterInit = [](auto in, auto out, auto instanceData, auto node, auto core, auto vsapi) {
	auto d = reinterpret_cast<FilterData*>(*instanceData);
	vsapi->setVideoInfo(d->vi, 1, node);
};

auto FilterGetFrame = [](auto n, auto activationReason, auto instanceData, auto frameData, auto frameCtx, auto core, auto vsapi) {
	auto d = reinterpret_cast<const FilterData*>(*instanceData);
	auto nullframe = static_cast<const VSFrameRef*>(nullptr);
	if (activationReason == arInitial)
		vsapi->requestFrameFilter(n, d->node, frameCtx);
	else if (activationReason == arAllFramesReady) {
		auto srcRaw = vsapi->getFrameFilter(n, d->node, frameCtx);
		auto src = Frame<const float>{ const_cast<VSFrameRef*>(srcRaw) };
		auto frames = std::array{ srcRaw,srcRaw,srcRaw };
		auto planes = std::array{ 0, 1, 2 };
		auto dst = Frame<float>{ vsapi->newVideoFrame2(src.Format, src.Width[0], src.Height[0], frames.data(), planes.data(), srcRaw, core) };

		for (auto plane : Range{ src.Format->numPlanes }) {
			auto GaussBlur = [](auto Center) {
				auto sum = Center[-1][-1] + Center[-1][0] * 2 + Center[-1][1] +
					Center[0][-1] * 2 + Center[0][0] * 4 + Center[0][1] * 2 +
					Center[1][-1] + Center[1][0] * 2 + Center[1][1];
				return sum / (1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1);
			};

			for (auto y : Range{ src.Height[plane] })
				for (auto x : Range{ src.Width[plane] })
					dst[plane][y][x] = GaussBlur(src[plane].View(y, x));
		}
		return const_cast<decltype(nullframe)>(dst.RawFrame);
	}
	return nullframe;
};

auto FilterFree = [](auto instanceData, auto core, auto vsapi) {
	auto d = reinterpret_cast<FilterData*>(instanceData);
	delete d;
};

auto FilterCreate = [](auto in, auto out, auto userData, auto core, auto vsapi) {
	RefreshGlobals(vsapi, in, out);
	auto d = new FilterData{};
	if (auto init_status = d->Initialize(); init_status == false) {
		delete d;
		return;
	}
	vsapi->createFilter(in, out, "Test", FilterInit, FilterGetFrame, FilterFree, fmParallel, 0, d, core);
};

VS_EXTERNAL_API(auto) VapourSynthPluginInit(VSConfigPlugin configFunc, VSRegisterFunction registerFunc, VSPlugin* plugin) {
	configFunc("com.zonked.test", "test", "Test Filter", VAPOURSYNTH_API_VERSION, 1, plugin);
	registerFunc("Test",
		"clip:clip;"
		, FilterCreate, 0, plugin);
}
