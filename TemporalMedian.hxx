#pragma once
#include "Interface.hxx"

struct TemporalMedian final {
	static constexpr auto Name = "TemporalMedian";
	static constexpr auto PythonInterface = "clip:clip;radius:int:opt;";
	static constexpr auto Mode = VSFilterMode::fmParallel;
	self(InputClip, Clip{});
	self(Radius, 1);
	auto Initialize(auto Arguments, auto Outputs) {
		Arguments.Fetch(InputClip, "clip");
		Arguments.Fetch(Radius, "radius");
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision()) {
			Outputs.RaiseError(Name + ": only single precision floating point clips with constant format and dimensions supported."s);
			return false;
		}
		if (Radius < 0) {
			Outputs.RaiseError(Name + ": radius cannot be negative!"s);
			return false;
		}
		return true;
	}
	auto ProvideMetadataForOutputs() {
		return std::tuple{ InputClip.GetMetadata(), 1 };
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrames(Index, Radius, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrames = InputClip.GetFrames<const float>(Index, Radius, PaddingPolicies::Temporal::Reflect, FrameContext);
		auto ProcessedFrame = Core.CreateNewFrameFrom(InputFrames[0]);
		auto Samples = std::vector<float>{};
		Samples.resize(2 * Radius + 1);
		for (auto c : Range{ ProcessedFrame.Format->numPlanes })
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