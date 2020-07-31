#pragma once
#include "Interface.vxx"

struct GaussBlurFast final {
	static constexpr auto Name = "GaussBlurFast";
	static constexpr auto Parameters = "clip:clip;";
	self(InputClip, Clip{});
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			return Console.RaiseError("only single precision floating point clips with constant format and dimensions supported.");
		return true;
	}
	auto RegisterVideoInfo(auto Core) {
		return InputClip.ExposeVideoInfo();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
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
		auto Conv = Channel[Above][Left] + Channel[Above][x] * 2 + Channel[Above][Right] +
			Channel[y][Left] * 2 + Channel[y][x] * 4 + Channel[y][Right] * 2 +
			Channel[Below][Left] + Channel[Below][x] * 2 + Channel[Below][Right];
		return Conv / 16;
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.GetFrame<const float, true>(Index, FrameContext);
		auto ProcessedFrame = Core.CreateNewFrameFrom(InputFrame);
		for (auto c : Range{ InputFrame.PlaneCount }) {
			for (auto y : Range{ 1, InputFrame[c].Height - 1 }) {
				ProcessedFrame[c][y][0] = GaussKernel<false, false, true>(InputFrame[c], y, 0);
				for (auto x : Range{ 1, InputFrame[c].Width - 1 })
					ProcessedFrame[c][y][x] = GaussKernel(InputFrame[c], y, x);
				ProcessedFrame[c][y][InputFrame[c].Width - 1] = GaussKernel<false, false, false, true>(InputFrame[c], y, InputFrame[c].Width - 1);
			}
			for (auto x : Range{ 1, InputFrame[c].Width - 1 }) {
				ProcessedFrame[c][0][x] = GaussKernel<true>(InputFrame[c], 0, x);
				ProcessedFrame[c][InputFrame[c].Height - 1][x] = GaussKernel<false, true>(InputFrame[c], InputFrame[c].Height - 1, x);
			}
			ProcessedFrame[c][0][0] = GaussKernel<true, false, true>(InputFrame[c], 0, 0);
			ProcessedFrame[c][0][InputFrame[c].Width - 1] = GaussKernel<true, false, false, true>(InputFrame[c], 0, InputFrame[c].Width - 1);
			ProcessedFrame[c][InputFrame[c].Height - 1][0] = GaussKernel<false, true, true>(InputFrame[c], InputFrame[c].Height - 1, 0);
			ProcessedFrame[c][InputFrame[c].Height - 1][InputFrame[c].Width - 1] = GaussKernel<false, true, false, true>(InputFrame[c], InputFrame[c].Height - 1, InputFrame[c].Width - 1);
		}
		return ProcessedFrame.Leak();
	}
};