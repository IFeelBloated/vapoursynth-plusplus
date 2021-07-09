#pragma once
#include "Core.vxx"

struct GaussBlurFast {
	field(InputClip, VideoNode{});

public:
	static constexpr auto Signature = "clip: vnode";
	
public:
	GaussBlurFast(auto Arguments) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw std::runtime_error{ "only single precision floating point clips with constant format and dimensions supported." };
	}
	auto SpecifyMetadata() {
		return InputClip.ExtractMetadata();
	}
	template<auto ClampAbove = false, auto ClampBelow = false, auto ClampLeft = false, auto ClampRight = false>
	auto GaussKernel(auto& Channel, auto y, auto x) {
		auto Above = y - 1;
		auto Below = y + 1;
		auto Left = x - 1;
		auto Right = x + 1;
		if constexpr (ClampAbove)
			Above = 0;
		if constexpr (ClampBelow)
			Below = Channel.Height - 1;
		if constexpr (ClampLeft)
			Left = 0;
		if constexpr (ClampRight)
			Right = Channel.Width - 1;
		auto WeightedSum = Channel[Above][Left] + Channel[Above][x] * 2 + Channel[Above][Right] +
			Channel[y][Left] * 2 + Channel[y][x] * 4 + Channel[y][Right] * 2 +
			Channel[Below][Left] + Channel[Below][x] * 2 + Channel[Below][Right];
		return WeightedSum / 16;
	}
	auto GenerateFrame(auto Index, auto GeneratorContext, auto Core) {
		auto InputFrame = InputClip.AcquireFrame<const float>(Index, GeneratorContext);
		auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrame);
		for (auto c : Range{ InputFrame.PlaneCount }) {
			auto& Canvas = InputFrame[c].DirectAccess();
			for (auto y : Range{ 1, Canvas.Height - 1 }) {
				ProcessedFrame[c][y][0] = GaussKernel<false, false, true>(Canvas, y, 0);
				for (auto x : Range{ 1, Canvas.Width - 1 })
					ProcessedFrame[c][y][x] = GaussKernel(Canvas, y, x);
				ProcessedFrame[c][y][Canvas.Width - 1] = GaussKernel<false, false, false, true>(Canvas, y, Canvas.Width - 1);
			}
			for (auto x : Range{ 1, Canvas.Width - 1 }) {
				ProcessedFrame[c][0][x] = GaussKernel<true>(Canvas, 0, x);
				ProcessedFrame[c][Canvas.Height - 1][x] = GaussKernel<false, true>(Canvas, Canvas.Height - 1, x);
			}
			ProcessedFrame[c][0][0] = GaussKernel<true, false, true>(Canvas, 0, 0);
			ProcessedFrame[c][0][Canvas.Width - 1] = GaussKernel<true, false, false, true>(Canvas, 0, Canvas.Width - 1);
			ProcessedFrame[c][Canvas.Height - 1][0] = GaussKernel<false, true, true>(Canvas, Canvas.Height - 1, 0);
			ProcessedFrame[c][Canvas.Height - 1][Canvas.Width - 1] = GaussKernel<false, true, false, true>(Canvas, Canvas.Height - 1, Canvas.Width - 1);
		}
		return ProcessedFrame;
	}
};