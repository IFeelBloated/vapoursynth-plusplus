#pragma once
#include "Core.vxx"

struct GaussBlur {
	field(InputClip, VideoNode{});

public:
	static constexpr auto Signature = "clip: vnode";
	
public:
	GaussBlur(auto Arguments) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw std::runtime_error{ "only single precision floating point clips with constant format and dimensions supported." };
	}
	auto SpecifyMetadata() {
		return InputClip.ExtractMetadata();
	}
	auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
		auto InputFrame = InputClip.AcquireFrame<const float>(Index, FrameContext);
		auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrame);
		auto GaussKernel = [](auto Center) {
			auto WeightedSum = Center[-1][-1] + Center[-1][0] * 2 + Center[-1][1] +
				Center[0][-1] * 2 + Center[0][0] * 4 + Center[0][1] * 2 +
				Center[1][-1] + Center[1][0] * 2 + Center[1][1];
			return WeightedSum / 16;
		};
		for (auto c : Range{ InputFrame.PlaneCount })
			for (auto y : Range{ InputFrame[c].Height })
				for (auto x : Range{ InputFrame[c].Width })
					ProcessedFrame[c][y][x] = GaussKernel(InputFrame[c].View(y, x));
		return ProcessedFrame;
	}
};