#pragma once
#include "Interface.vxx"

struct Crop {
	field(InputClip, VideoNode{});
	field(Left, 0);
	field(Top, 0);
	field(CroppedWidth, 0);
	field(CroppedHeight, 0);

public:
	static constexpr auto Signature = "clip:clip;left:int:opt;right:int:opt;top:int:opt;bottom:int:opt;";

public:
	Crop(auto Arguments) {
		auto Right = 0;
		auto Bottom = 0;
		InputClip = Arguments["clip"];
		if (Arguments["left"].Exists())
			Left = Arguments["left"];
		if (Arguments["right"].Exists())
			Right = Arguments["right"];
		if (Arguments["top"].Exists())
			Top = Arguments["top"];
		if (Arguments["bottom"].Exists())
			Bottom = Arguments["bottom"];
		CroppedWidth = InputClip.Width - Left - Right;
		CroppedHeight = InputClip.Height - Top - Bottom;
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.Is444())
			throw RuntimeError{ "clips with subsampled format not supported." };
		if (Left < 0 || Right < 0 || Top < 0 || Bottom < 0)
			throw RuntimeError{ "cannot crop negative measures!" };
		if (CroppedWidth <= 0 || CroppedHeight <= 0)
			throw RuntimeError{ "dimensions must be positive after cropping!" };
	}
	auto SpecifyMetadata() {
		auto Metadata = InputClip.ExtractMetadata();
		Metadata.Width = CroppedWidth;
		Metadata.Height = CroppedHeight;
		return Metadata;
	}
	auto AcquireResourcesForFrameGenerator(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
		auto DrawGenericFrame = [&](auto&& InputFrame) {
			using PixelType = std::decay_t<decltype(InputFrame[0][0][0])>;
			auto ProcessedFrame = VideoFrame<PixelType>{ Core.AllocateVideoFrame(InputFrame.ExtractFormat(), CroppedWidth, CroppedHeight) };
			ProcessedFrame.AbsorbPropertiesFrom(InputFrame);
			for (auto c : Range{ InputFrame.PlaneCount })
				for (auto y : Range{ CroppedHeight })
					for (auto x : Range{ CroppedWidth })
						ProcessedFrame[c][y][x] = InputFrame[c][y + Top][x + Left];
			return ProcessedFrame.Transfer();
		};
		if (InputClip.IsSinglePrecision())
			return DrawGenericFrame(InputClip.FetchFrame<const float>(Index, FrameContext));
		else if (InputClip.BitsPerSample > 8)
			return DrawGenericFrame(InputClip.FetchFrame<const std::uint16_t>(Index, FrameContext));
		else
			return DrawGenericFrame(InputClip.FetchFrame<const std::uint8_t>(Index, FrameContext));
	}
};