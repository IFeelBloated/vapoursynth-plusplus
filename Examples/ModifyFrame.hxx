#pragma once
#include "Core.vxx"

struct ModifyFrame {
	field(InputClip, VideoNode{});
	field(Evaluator, Function{});

public:
	static constexpr auto Signature = "clip: vnode, evaluator: func";
	static constexpr auto ExecutionPolicyForFrameGenerator = ExecutionSchemes::ParallelResourceAcquisition;

public:
	ModifyFrame(auto Arguments) {
		InputClip = Arguments["clip"];
		Evaluator = Arguments["evaluator"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions())
			throw RuntimeError{ "only clips with constant format and dimensions supported." };
	}
	auto SpecifyMetadata() {
		return InputClip.ExtractMetadata();
	}
	auto AcquireResourcesForFrameGenerator(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
		auto InputFrame = InputClip.PeekFrame(Index, FrameContext);
		return static_cast<FrameReference>(Evaluator("src", InputFrame));
	}
};