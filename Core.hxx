#pragma once
#include "Frame.hxx"

struct VaporCore final {
	self(Instance, static_cast<VSCore*>(nullptr));
	auto AllocateFrame(auto Format, auto Width, auto Height) {
		return VaporGlobals::API->newVideoFrame(Format, Width, Height, nullptr, Instance);
	}
	auto CreateNewFrameFrom(auto&& ReferenceFrame) {
		using PixelType = std::decay_t<decltype(ReferenceFrame[0][0][0])>;
		auto Format = ReferenceFrame.Format;
		auto Width = ReferenceFrame[0].Width;
		auto Height = ReferenceFrame[0].Height;
		auto Properties = ReferenceFrame.RawFrame;
		auto AllocatedFrame = VaporGlobals::API->newVideoFrame(Format, Width, Height, Properties, Instance);
		return Frame<PixelType>{ AllocatedFrame };
	}
};