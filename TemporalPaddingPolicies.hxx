#pragma once
#include "Globals.hxx"
#include "SpatialPaddingPolicies.hxx"
#include "Frame.hxx"

namespace PaddingPolicies::Temporal {
	template<typename PixelType, typename Filter>
	auto Zero = [](auto& VideoClip, auto Index, auto FrameContext) {
		auto Format = VideoClip.Info->format;
		auto Width = VideoClip.Info->width;
		auto Height = VideoClip.Info->height;
		auto RawFrame = VaporGlobals::API<Filter>->newVideoFrame(Format, Width, Height, nullptr, VaporGlobals::Core<Filter>);
		auto BlankFrame = Frame<PixelType, Filter>{ RawFrame };
		for (auto c : Range{ BlankFrame.Format->numPlanes }) {
			auto CurrentChannel = BlankFrame[c];
			for (auto y : Range{ BlankFrame.Height[c] })
				for (auto x : Range{ BlankFrame.Width[c] })
					CurrentChannel[y][x] = static_cast<PixelType>(0);
		}
		return BlankFrame.Leak();
	};

	template<typename Filter>
	auto Repeat = [](auto& VideoClip, auto Index, auto FrameContext) {
		auto FrameCount = VideoClip.Info->numFrames;
		Index = Min(Max(Index, 0), FrameCount - 1);
		return VaporGlobals::API<Filter>->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};

	template<typename Filter>
	auto Reflect = [](auto& VideoClip, auto Index, auto FrameContext) {
		auto FrameCount = VideoClip.Info->numFrames;
		Index = ReflectCoordinate(Index, FrameCount);
		return VaporGlobals::API<Filter>->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};
}