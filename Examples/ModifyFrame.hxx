#pragma once
#include "Interface.vxx"

struct ModifyFrame final {
	static constexpr auto Name = "ModifyFrame";
	static constexpr auto Parameters = "clip:clip;evaluator:func;";
	static constexpr auto MultithreadingMode = VSFilterMode::fmParallelRequests;
	self(InputClip, Clip{});
	self(Evaluator, Function{});
	ModifyFrame(auto Arguments) {
		InputClip = Arguments["clip"];
		Evaluator = Arguments["evaluator"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw RuntimeError{ "only single precision floating point clips with constant format and dimensions supported." };
	}
	auto RegisterVideoInfo(auto Core) {
		return InputClip.ExposeVideoInfo();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		using FrameType = Frame<const float>;
		auto InputFrame = InputClip.GetFrame<const float>(Index, FrameContext);
		auto EvaluatedFrame = static_cast<FrameType>(Evaluator("src", InputFrame));
		return EvaluatedFrame.Leak();
	}
};