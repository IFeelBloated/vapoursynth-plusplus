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
		auto StandardFilter = VaporGlobals::API->getPluginByNs("std", Core);
		auto Arguments = VaporGlobals::API->createMap();
		VaporGlobals::API->propSetNode(Arguments, "clip", InputClip.VideoNode, VSPropAppendMode::paReplace);
		auto ResultMap = VaporGlobals::API->invoke(StandardFilter, "Transpose", Arguments);
		InputClip = Clip{ VaporGlobals::API->propGetNode(ResultMap, "clip", 0, nullptr) };
		VaporGlobals::API->freeMap(Arguments);
		VaporGlobals::API->freeMap(ResultMap);
		VaporGlobals::API->propSetNode(Console, "clip", InputClip.VideoNode, VSPropAppendMode::paReplace);
	}
};