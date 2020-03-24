#pragma once
#include "Frame.hxx"

struct VaporCore final {
	self(Instance, static_cast<VSCore*>(nullptr));
	auto AllocateFrame(auto Format, auto Width, auto Height, auto&& Properties) {
		return VaporGlobals::API->newVideoFrame(Format, Width, Height, Properties, Instance);
	}
};