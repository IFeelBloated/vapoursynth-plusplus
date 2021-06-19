#pragma once
#include "Core.vxx"

struct Rec601ToRGB {
	field(InputClip, VideoNode{});

public:
	static constexpr auto Signature = "clip: vnode";
	
public:
	Rec601ToRGB(auto Arguments) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision() || !InputClip.IsYUV() || !InputClip.Is444())
			throw std::runtime_error{ "only YUV444PS clips supported." };
	}
	auto SpecifyMetadata(auto Core) {
		auto Metadata = InputClip.ExtractMetadata();
		Metadata.Format = Core.Query(VideoFormats::RGBS);
		return Metadata;
	}
	auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
		auto InputFrame = InputClip.AcquireFrame<const float>(Index, FrameContext);
		auto ProcessedFrame = VideoFrame<float>{ Core.AllocateVideoFrame(VideoFormats::RGBS, InputClip.Width, InputClip.Height) };
		if (InputFrame["_Matrix"].Exists() == false)
			Core.Alert("_Matrix property not found, assuming Rec601.");
		else if (auto Matrix = static_cast<int>(InputFrame["_Matrix"]); Matrix != 6)
			throw std::runtime_error{ "unrecognized _Matrix!" };
		if (InputFrame["_ColorRange"].Exists() == false)
			Core.Alert("_ColorRange property not found, assuming full range.");
		else if (auto ColorRange = static_cast<int>(InputFrame["_ColorRange"]); ColorRange != 0)
			throw std::runtime_error{ "only full range supported!" };
		for (auto y : Range{ InputClip.Height })
			for (auto x : Range{ InputClip.Width }) {
				auto Kr = 0.299, Kg = 0.587, Kb = 0.114;
				auto Y = InputFrame[0][y][x];
				auto Cb = 2 * InputFrame[1][y][x];
				auto Cr = 2 * InputFrame[2][y][x];
				ProcessedFrame[0][y][x] = Y + (1 - Kr) * Cr;
				ProcessedFrame[1][y][x] = Y - (1 - Kb) * Kb / Kg * Cb - (1 - Kr) * Kr / Kg * Cr;
				ProcessedFrame[2][y][x] = Y + (1 - Kb) * Cb;
			}
		ProcessedFrame.AbsorbPropertiesFrom(InputFrame);
		ProcessedFrame["_Matrix"] = 0;
		ProcessedFrame["_ColorSpace"] = 0;
		return ProcessedFrame;
	}
};