#pragma once
#include "../Include/Interface.hxx"

struct Rec601ToRGB final {
	static constexpr auto Name = "Rec601ToRGB";
	static constexpr auto Parameters = "clip:clip;";
	self(InputClip, Clip{});
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision() || !InputClip.IsYUV() || !InputClip.Is444())
			return Console.RaiseError("only YUV444PS clips supported.");
		return true;
	}
	auto RegisterVideoInfo(auto Core) {
		auto VideoInfo = InputClip.ExposeVideoInfo();
		VideoInfo.Format = Core.FetchFormat(VSPresetFormat::pfRGBS);
		return VideoInfo;
	}
	auto RequestReferenceFrames(auto Index, auto FrameContext) {
		InputClip.RequestFrame(Index, FrameContext);
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto InputFrame = InputClip.GetFrame<const float>(Index, FrameContext);
		auto ProcessedFrame = Frame<float>{ Core.AllocateFrame(Core.FetchFormat(VSPresetFormat::pfRGBS), InputClip.Width, InputClip.Height) };
		if (InputFrame["_Matrix"].Exists() == false)
			return FrameContext.RaiseError("_Matrix property not found!");
		if (InputFrame["_ColorRange"].Exists() == false)
			return FrameContext.RaiseError("_ColorRange property not found!");
		if (auto Matrix = static_cast<int>(InputFrame["_Matrix"]); Matrix != 6)
			return FrameContext.RaiseError("unrecognized _Matrix!");
		if (auto ColorRange = static_cast<int>(InputFrame["_ColorRange"]); ColorRange != 0)
			return FrameContext.RaiseError("only full range supported!");
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
		Core.CopyFrameProperties(InputFrame, ProcessedFrame);
		ProcessedFrame["_Matrix"] = 0;
		ProcessedFrame["_ColorSpace"] = 0;
		return ProcessedFrame.Leak();
	}
};