#pragma once
#include "VapourSynth.h"

namespace VaporGlobals {
	auto API = static_cast<const VSAPI*>(nullptr);
	auto Arguments = static_cast<const VSMap*>(nullptr);
	auto Outputs = static_cast<VSMap*>(nullptr);
}

auto RefreshGlobals(auto API, auto Arguments, auto Outputs) {
	VaporGlobals::API = API;
	VaporGlobals::Arguments = Arguments;
	VaporGlobals::Outputs = Outputs;
}