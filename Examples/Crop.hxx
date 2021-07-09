#pragma once
#include "Core.vxx"

struct Crop {
	field(InputClip, VideoNode{});
	field(Left, 0);
	field(Top, 0);
	field(CroppedWidth, 0);
	field(CroppedHeight, 0);

public:
	static constexpr auto Signature = "clip: vnode, [left, right, top, bottom]: int?";

public:
	Crop(auto Arguments) {
		auto Right = 0, Bottom = 0;
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
			throw std::runtime_error{ "clips with subsampled format not supported." };
		if (Left < 0 || Right < 0 || Top < 0 || Bottom < 0)
			throw std::runtime_error{ "cannot crop negative measures!" };
		if (CroppedWidth <= 0 || CroppedHeight <= 0)
			throw std::runtime_error{ "dimensions must be positive after cropping!" };
	}
	auto SpecifyMetadata() {
		auto Metadata = InputClip.ExtractMetadata();
		Metadata.Width = CroppedWidth;
		Metadata.Height = CroppedHeight;
		return Metadata;
	}
	auto GenerateFrame(auto Index, auto GeneratorContext, auto Core) {
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
			return DrawGenericFrame(InputClip.AcquireFrame<const float>(Index, GeneratorContext));
		else if (InputClip.BitsPerSample > 8)
			return DrawGenericFrame(InputClip.AcquireFrame<const std::uint16_t>(Index, GeneratorContext));
		else
			return DrawGenericFrame(InputClip.AcquireFrame<const std::uint8_t>(Index, GeneratorContext));
	}
};