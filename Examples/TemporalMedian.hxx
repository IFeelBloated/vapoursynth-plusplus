#pragma once
#include "Interface.vxx"

struct TemporalMedian {
	field(InputClip, VideoNode{});
	field(Radius, 1);

public:
	static constexpr auto Name = "TemporalMedian";
	static constexpr auto Signature = "clip:clip;radius:int:opt;";

public:
	TemporalMedian(auto Arguments) {
		InputClip = Arguments["clip"];
		if (Arguments["radius"].Exists())
			Radius = Arguments["radius"];
		InputClip.RequestFunction = [Radius = Radius](auto Index) { return Range{ Index - Radius, Index + Radius + 1 }; };
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
			throw RuntimeError{ "only single precision floating point clips with constant format and dimensions supported." };
		if (Radius < 0)
			throw RuntimeError{ "radius cannot be negative!" };
	}
	auto RegisterMetadata(auto Core) {
		return InputClip.ExtractMetadata();
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrames(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrames = InputClip.FetchFrames<const float>(Index, FrameContext);
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
		return ProcessedFrame.Leak();
	}
};