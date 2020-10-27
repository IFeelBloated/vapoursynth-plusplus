#pragma once
#include "Interface.vxx"

struct ModifyFrame final {
	static constexpr auto Name = "ModifyFrame";
	static constexpr auto Parameters = "clip:clip;evaluator:func;";
	static constexpr auto MultithreadingMode = VSFilterMode::fmParallelRequests;
	self(InputClip, VideoNode{});
	self(Evaluator, Function{});
	ModifyFrame(auto Arguments) {
		InputClip = Arguments["clip"];
		Evaluator = Arguments["evaluator"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw RuntimeError{ "only single precision floating point clips with constant format and dimensions supported." };
	}
	auto RegisterMetadata(auto Core) {
		return InputClip.ExtractMetadata();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		using FrameType = VideoFrame<const float>;
		auto InputFrame = InputClip.FetchFrame<const float>(Index, FrameContext);
		auto EvaluatedFrame = static_cast<FrameType>(Evaluator("src", InputFrame));
		return EvaluatedFrame.Leak();
	}
};