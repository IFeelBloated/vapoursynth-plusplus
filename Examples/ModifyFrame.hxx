#pragma once
#include "Interface.vxx"

struct ModifyFrame {
	field(InputClip, VideoNode{});
	field(Evaluator, Function{});

public:
	static constexpr auto Name = "ModifyFrame";
	static constexpr auto Signature = "clip:clip;evaluator:func;";
	static constexpr auto MultithreadingMode = VSFilterMode::fmParallelRequests;

public:
	ModifyFrame(auto Arguments) {
		InputClip = Arguments["clip"];
		Evaluator = Arguments["evaluator"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions())
			throw RuntimeError{ "only clips with constant format and dimensions supported." };
	}
	auto RegisterMetadata(auto Core) {
		return InputClip.ExtractMetadata();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.PeekFrame(Index, FrameContext);
		auto EvaluatedFrame = static_cast<Frame>(Evaluator("src", InputFrame));
		return EvaluatedFrame.Leak();
	}
};