#pragma once
#include "Frame.hxx"

template<typename Filter>
struct Clip final {
	self(VideoNode, static_cast<VSNodeRef*>(nullptr));
	self(Info, static_cast<const VSVideoInfo*>(nullptr));
	Clip() = default;
	Clip(auto VideoNode) {
		this->VideoNode = VideoNode;
		this->Info = VaporGlobals::API<Filter>->getVideoInfo(VideoNode);
	}
	Clip(const Clip& OtherClip) {
		*this = OtherClip;
	}
	Clip(Clip&& OtherClip) {
		*this = std::move(OtherClip);
	}
	auto& operator=(const Clip& OtherClip) {
		if (this != &OtherClip) {
			this->~Clip();
			VideoNode = VaporGlobals::API<Filter>->cloneNodeRef(OtherClip.VideoNode);
			Info = OtherClip.Info;
		}
		return *this;
	}
	auto& operator=(Clip&& OtherClip) {
		if (this != &OtherClip) {
			std::swap(VideoNode, OtherClip.VideoNode);
			Info = OtherClip.Info;
		}
		return *this;
	}
	~Clip() {
		if (VideoNode != nullptr)
			VaporGlobals::API<Filter>->freeNode(VideoNode);
	}
	auto RequestFrame(auto Index, auto FrameContext) {
		VaporGlobals::API<Filter>->requestFrameFilter(Index, VideoNode, FrameContext);
	}
	auto RequestFrames(auto Index, auto Radius, auto FrameContext) {

	}
	template<typename PixelType>
	auto GetFrame(auto Index, auto FrameContext) {
		auto RawFrame = VaporGlobals::API<Filter>->getFrameFilter(Index, VideoNode, FrameContext);
		return Frame<PixelType, Filter>{ PointerRemoveConstant(RawFrame) };
	}
	template<typename PixelType>
	auto GetFrames(auto Index, auto Radius, auto FrameContext) {

	}
	auto IsSinglePrecision() {
		return And(Info->format->sampleType == stFloat, Info->format->bitsPerSample == 32);
	}
	auto WithConstantDimensions() {
		return And(Info->width != 0, Info->height != 0);
	}
	auto WithConstantFormat() {
		return Info->format != nullptr;
	}
};

namespace TemporalPaddingPolicies {

}