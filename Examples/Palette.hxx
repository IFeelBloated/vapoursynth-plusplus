#pragma once
#include "Core.vxx"

struct Palette {
	field(Shades, std::vector<double>{});
	field(Width, 640);
	field(Height, 480);

public:
	static constexpr auto Signature = "shades: float[], [width, height]: int?";

public:
	Palette(auto Arguments) {
		Shades = Arguments["shades"];
		if (Arguments["width"].Exists())
			Width = Arguments["width"];
		if (Arguments["height"].Exists())
			Height = Arguments["height"];
		if (Width <= 0 || Height <= 0)
			throw std::runtime_error{ "spatial dimensions must be positive!" };
	}
	auto SpecifyMetadata(auto Core) {
		auto Metadata = VideoInfo{
			.Format = Core.Query(VideoFormats::GrayS),
			.FrameRateNumerator = 30000, .FrameRateDenominator = 1001,
			.Width = Width, .Height = Height,
			.FrameCount = 1
		};
		return std::vector{ Shades.size(), Metadata };
	}
	auto GenerateFrame(auto Index, auto GeneratorContext, auto Core) {
		auto ProcessedFrame = VideoFrame<float>{ Core.AllocateVideoFrame(VideoFormats::GrayS, Width, Height) };
		for (auto y : Range{ Height })
			for (auto x : Range{ Width })
				ProcessedFrame[0][y][x] = Shades[GeneratorContext.QueryOutputIndex()];
		return ProcessedFrame;
	}
};