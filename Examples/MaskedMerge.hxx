#pragma once
#include "Core.vxx"

struct MaskedMerge {
	field(Background, VideoNode{});
	field(Foreground, VideoNode{});
	field(Mask, VideoNode{});

public:
	static constexpr auto Signature = "[clipa, clipb, mask]: vnode";

public:
	MaskedMerge(auto Arguments) {
		Background = Arguments["clipa"];
		Foreground = Arguments["clipb"];
		Mask = Arguments["mask"];
		if (!Background.WithConstantFormat() || !Background.WithConstantDimensions() || !Background.IsSinglePrecision() || !Background.Is444())
			throw std::runtime_error{ "only non-subsampled single precision floating point clips with constant format and dimensions supported." };
		if (Background.ExtractFormat() != Foreground.ExtractFormat() || Background.Width != Foreground.Width || Background.Height != Foreground.Height)
			throw std::runtime_error{ "clipa and clipb must have the same format and dimensions." };
		if (!Mask.WithConstantFormat() || Mask.ColorFamily != ColorFamilies::Gray || !Mask.IsSinglePrecision() || Mask.Width != Background.Width || Mask.Height != Background.Height)
			throw std::runtime_error{ "mask must be GrayS and have the same dimensions as clipa." };
	}
	auto SpecifyMetadata() {
		return Background.ExtractMetadata();
	}
	auto GenerateFrame(auto Index, auto GeneratorContext, auto Core) {
		auto [BackgroundFrame, ForegroundFrame, MaskFrame] = Node::AcquireFrameGiven<const float>(Index, GeneratorContext).From(Background, Foreground, Mask);
		auto ProcessedFrame = Core.CreateBlankFrameFrom(BackgroundFrame);
		for (auto c : Range{ BackgroundFrame.PlaneCount })
			for (auto y : Range{ BackgroundFrame[c].Height })
				for (auto x : Range{ BackgroundFrame[c].Width })
					ProcessedFrame[c][y][x] = ForegroundFrame[c][y][x] * MaskFrame[0][y][x] + BackgroundFrame[c][y][x] * (1 - MaskFrame[0][y][x]);
		return ProcessedFrame;
	}
};