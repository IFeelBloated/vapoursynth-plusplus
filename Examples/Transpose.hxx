#pragma once
#include "../Include/Interface.hxx"

struct Transpose final {
	static constexpr auto Name = "Transpose";
	static constexpr auto Parameters = "clip:clip;";
	self(InputClip, Clip{});
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions())
			return Console.RaiseError("only clips with constant format and dimensions supported.");
		return true;
	}
	auto Preprocess(auto Core, auto Console) {
		InputClip = Core["std"]["Transpose"]("clip", InputClip);
		Console.Receive(InputClip);
	}
};