#pragma once
#include "Interface.hxx"

struct GaussBlur final {
	static constexpr auto Name = "GaussBlur";
	static constexpr auto PythonInterface = "clip:clip;";
	static constexpr auto Mode = VSFilterMode::fmParallel;
	self(InputClip, Clip{});
	auto Initialize(auto Arguments, auto Outputs) {
		Arguments.Fetch(InputClip, "clip");
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision()) {
			Outputs.RaiseError(Name + ": only single precision floating point clips with constant format and dimensions supported."s);
			return false;
		}
		return true;
	}
	auto ProvideMetadataForOutputs() {
		return std::tuple{ InputClip.GetMetadata(), 1 };
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.GetFrame<const float>(Index, FrameContext);
		auto ProcessedFrame = Core.CreateNewFrameFrom(InputFrame);
		auto GaussBlur = [](auto Center) {
			auto Conv = Center[-1][-1] + Center[-1][0] * 2 + Center[-1][1] +
				Center[0][-1] * 2 + Center[0][0] * 4 + Center[0][1] * 2 +
				Center[1][-1] + Center[1][0] * 2 + Center[1][1];
			return Conv / 16;
		};
		for (auto c : Range{ InputFrame.Format->numPlanes })
			for (auto y : Range{ InputFrame[c].Height })
				for (auto x : Range{ InputFrame[c].Width })
					ProcessedFrame[c][y][x] = GaussBlur(InputFrame[c].View(y, x));
		return ProcessedFrame.Leak();
	}
};