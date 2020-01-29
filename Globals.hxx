#pragma once
#include "VapourSynth.h"

namespace VaporGlobals {
	template<typename Filter>
	auto API = static_cast<const VSAPI*>(nullptr);

	template<typename Filter>
	auto Arguments = static_cast<const VSMap*>(nullptr);

	template<typename Filter>
	auto Outputs = static_cast<VSMap*>(nullptr);
}

template<typename Filter>
auto RefreshGlobals(auto API, auto Arguments, auto Outputs) {
	VaporGlobals::API<Filter> = API;
	VaporGlobals::Arguments<Filter> = Arguments;
	VaporGlobals::Outputs<Filter> = Outputs;
}