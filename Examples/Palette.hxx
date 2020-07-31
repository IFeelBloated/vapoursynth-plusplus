#pragma once
#include "Interface.vxx"

struct Palette final {
	static constexpr auto Name = "Palette";
	static constexpr auto Parameters = "shades:float[];width:int:opt;height:int:opt;";
	self(Shades, std::vector<double>{});
	self(Width, 640);
	self(Height, 480);
	auto Initialize(auto Arguments, auto Console) {
		for (auto&& x : Arguments["shades"])
			Shades.push_back(x);
		if (Arguments["width"].Exists())
			Width = Arguments["width"];
		if (Arguments["height"].Exists())
			Height = Arguments["height"];
		if (Width <= 0 || Height <= 0)
			return Console.RaiseError("spatial dimensions must be positive!");
		return true;
	}
	auto RegisterVideoInfo(auto Core) {
		auto VideoInfo = VSVideoInfo{
			.Format = Core.FetchFormat(VSPresetFormat::pfGrayS),
			.FrameRateNumerator = 30000, .FrameRateDenominator = 1001,
			.Width = Width, .Height = Height,
			.FrameCount = 1
		};
		return std::vector{ Shades.Size(), VideoInfo };
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto ProcessedFrame = Frame<float>{ Core.AllocateFrame(VSPresetFormat::pfGrayS, Width, Height) };
		for (auto y : Range{ Height })
			for (auto x : Range{ Width })
				ProcessedFrame[0][y][x] = Shades[FrameContext.RevealOutputIndex()];
		return ProcessedFrame.Leak();
	}
};