#pragma once
#include "Interface.vxx"

struct ModifyFrame final {
	static constexpr auto Name = "ModifyFrame";
	static constexpr auto Parameters = "clip:clip;evaluator:func;";
	static constexpr auto MultithreadingMode = VSFilterMode::fmParallelRequests;
	self(InputClip, Clip{});
	self(Evaluator, Function{});
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			return Console.RaiseError("only single precision floating point clips with constant format and dimensions supported.");
		Evaluator = Arguments["evaluator"];
		return true;
	}
	auto RegisterVideoInfo(auto Core) {
		return InputClip.ExposeVideoInfo();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.GetFrame<const float>(Index, FrameContext);
		if (auto EvaluatedFrame = Evaluator("src", InputFrame); Evaluator.EvaluationFailed())
			return FrameContext.RaiseError(Evaluator.ErrorMessage);
		else
			return static_cast<Frame<const float>>(EvaluatedFrame).Leak();
	}
};