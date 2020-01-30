#include "Clip.hxx"
#include "VapourSynth.h"
#include "VSHelper.h"

struct FilterData final {
	self(vid, Clip<FilterData>{});
	auto CheckFormat() {
		auto errmsg = "test: only single precision floating point clips with constant format and dimensions supported."s;
		if (!And(vid.WithConstantFormat(), vid.WithConstantDimensions(), vid.IsSinglePrecision())) {
			VaporGlobals::API<FilterData>->setError(VaporGlobals::Outputs<FilterData>, errmsg.data());
			return false;
		}
		return true;
	}
	auto Initialize() {
		auto node = VaporGlobals::API<FilterData>->propGetNode(VaporGlobals::Arguments<FilterData>, "clip", 0, nullptr);
		new(&vid) Clip<FilterData>{ node };
		if (auto format_status = CheckFormat(); format_status == false)
			return false;
		return true;
	}
};

auto FilterInit = [](auto in, auto out, auto instanceData, auto node, auto core, auto vsapi) {
	auto d = reinterpret_cast<FilterData*>(*instanceData);
	vsapi->setVideoInfo(d->vid.Info, 1, node);
};

auto FilterGetFrame = [](auto n, auto activationReason, auto instanceData, auto frameData, auto frameCtx, auto core, auto vsapi) {
	auto d = reinterpret_cast<FilterData*>(*instanceData);
	auto nullframe = static_cast<const VSFrameRef*>(nullptr);
	if (activationReason == arInitial)
		d->vid.RequestFrame(n, frameCtx);
	else if (activationReason == arAllFramesReady) {
		auto src = d->vid.GetFrame<const float>(n, frameCtx);
		auto srcRaw = PointerAddConstant(src.RawFrame);
		auto frames = std::array{ srcRaw, srcRaw, srcRaw };
		auto planes = std::array{ 0, 1, 2 };
		auto dst = Frame<float, FilterData>{ vsapi->newVideoFrame2(src.Format, src.Width[0], src.Height[0], frames.data(), planes.data(), src, core) };

		auto GaussBlur = [](auto Center) {
			auto sum = Center[-1][-1] + Center[-1][0] * 2 + Center[-1][1] +
				Center[0][-1] * 2 + Center[0][0] * 4 + Center[0][1] * 2 +
				Center[1][-1] + Center[1][0] * 2 + Center[1][1];
			return sum / (1 + 2 + 1 + 2 + 4 + 2 + 1 + 2 + 1);
		};

		for (auto plane : Range{ src.Format->numPlanes })
			for (auto y : Range{ src.Height[plane] })
				for (auto x : Range{ src.Width[plane] })
					dst[plane][y][x] = GaussBlur(src[plane].View(y, x));

		return dst.Leak();
	}
	return nullframe;
};

auto FilterFree = [](auto instanceData, auto core, auto vsapi) {
	auto d = reinterpret_cast<FilterData*>(instanceData);
	delete d;
};

auto FilterCreate = [](auto in, auto out, auto userData, auto core, auto vsapi) {
	RefreshGlobals<FilterData>(vsapi, in, out, core);
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
