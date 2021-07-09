#pragma once
#include "Core.vxx"

struct TemporalMedian {
	field(InputClip, VideoNode{});
	field(Radius, 1);

public:
	static constexpr auto Signature = "clip: vnode, radius: int?";

public:
	TemporalMedian(auto Arguments) {
		InputClip = Arguments["clip"];
		if (Arguments["radius"].Exists())
			Radius = Arguments["radius"];
		InputClip.FrameRequestor = [this](auto Index) { return Range{ Index - Radius, Index + Radius + 1 }; };
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw std::runtime_error{ "only single precision floating point clips with constant format and dimensions supported." };
		if (Radius < 0)
			throw std::runtime_error{ "radius cannot be negative!" };
	}
	auto SpecifyMetadata() {
		return InputClip.ExtractMetadata();
	}
	auto GenerateFrame(auto Index, auto GeneratorContext, auto Core) {
		auto InputFrames = InputClip.AcquireFrames<const float>(Index, GeneratorContext);
		auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrames[0]);
		auto Samples = std::vector<float>{};
		Samples.resize(2 * Radius + 1);
		for (auto c : Range{ ProcessedFrame.PlaneCount })
			for (auto y : Range{ ProcessedFrame[c].Height })
				for (auto x : Range{ ProcessedFrame[c].Width }) {
					for (auto t : Range{ -Radius, Radius + 1 })
						Samples[t + Radius] = InputFrames[t][c][y][x];
					std::nth_element(Samples.begin(), Samples.begin() + Radius, Samples.end());
					ProcessedFrame[c][y][x] = Samples[Radius];
				}
		return ProcessedFrame;
	}
};