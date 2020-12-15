#pragma once
#include "Interface.vxx"

struct Rec601ToRGB {
	field(InputClip, VideoNode{});

public:
	static constexpr auto Name = "Rec601ToRGB";
	static constexpr auto Signature = "clip:clip;";
	
public:
	Rec601ToRGB(auto Arguments) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision() || !InputClip.IsYUV() || !InputClip.Is444())
			throw RuntimeError{ "only YUV444PS clips supported." };
	}
	auto RegisterMetadata(auto Core) {
		auto Metadata = InputClip.ExtractMetadata();
		Metadata.Format = Core.Query(VideoFormats::RGBS);
		return Metadata;
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.FetchFrame<const float>(Index, FrameContext);
		auto ProcessedFrame = VideoFrame<float>{ Core.AllocateVideoFrame(VideoFormats::RGBS, InputClip.Width, InputClip.Height) };
		if (InputFrame["_Matrix"].Exists() == false)
			Core.Alert("_Matrix property not found, assuming Rec601.");
		else if (auto Matrix = static_cast<int>(InputFrame["_Matrix"]); Matrix != 6)
			throw RuntimeError{ "unrecognized _Matrix!" };
		if (InputFrame["_ColorRange"].Exists() == false)
			Core.Alert("_ColorRange property not found, assuming full range.");
		else if (auto ColorRange = static_cast<int>(InputFrame["_ColorRange"]); ColorRange != 0)
			throw RuntimeError{ "only full range supported!" };
		for (auto y : Range{ InputClip.Height })
			for (auto x : Range{ InputClip.Width }) {
				auto Kr = 0.299;
				auto Kg = 0.587;
				auto Kb = 0.114;
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
		return ProcessedFrame.Leak();
	}
};