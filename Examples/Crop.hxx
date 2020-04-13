#pragma once
#include "../Include/Interface.hxx"

struct Crop final {
	static constexpr auto Name = "Crop";
	static constexpr auto Parameters = "clip:clip;left:int:opt;right:int:opt;top:int:opt;bottom:int:opt;";
	static constexpr auto Mode = VSFilterMode::fmParallel;
	self(InputClip, Clip{});
	self(Left, 0);
	self(Top, 0);
	self(CroppedWidth, 0);
	self(CroppedHeight, 0);
	auto Initialize(auto Arguments, auto Console) {
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
		CroppedWidth = InputClip.Metadata->Width - Left - Right;
		CroppedHeight = InputClip.Metadata->Height - Top - Bottom;
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.Is444())
			return Console.RaiseError("clips with subsampled format not supported.");
		if (Left < 0 || Right < 0 || Top < 0 || Bottom < 0)
			return Console.RaiseError("cannot crop negative measures!");
		if (CroppedWidth <= 0 || CroppedHeight <= 0)
			return Console.RaiseError("dimensions must be positive after cropping!");
		return true;
	}
	auto RegisterMetadata(auto Core) {
		auto Metadata = InputClip.GetMetadata();
		Metadata.Width = CroppedWidth;
		Metadata.Height = CroppedHeight;
		return Metadata;
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto DrawGenericFrame = [&](auto&& InputFrame) {
			using PixelType = std::decay_t<decltype(InputFrame[0][0][0])>;
			auto ProcessedFrame = Frame<PixelType>{ Core.AllocateFrame(InputFrame.Format, CroppedWidth, CroppedHeight) };
			Core.CopyFrameProperties(InputFrame, ProcessedFrame);
			for (auto c : Range{ InputFrame.Format->numPlanes })
				for (auto y : Range{ CroppedHeight })
					for (auto x : Range{ CroppedWidth })
						ProcessedFrame[c][y][x] = InputFrame[c][y + Top][x + Left];
			return ProcessedFrame.Leak();
		};
		if (InputClip.IsSinglePrecision())
			return DrawGenericFrame(InputClip.GetFrame<const float>(Index, FrameContext));
		else if (InputClip.Metadata->Format->BitsPerSample > 8)
			return DrawGenericFrame(InputClip.GetFrame<const std::uint16_t>(Index, FrameContext));
		else
			return DrawGenericFrame(InputClip.GetFrame<const std::uint8_t>(Index, FrameContext));
	}
};