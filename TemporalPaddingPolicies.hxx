#pragma once
#include "Globals.hxx"
#include "SpatialPaddingPolicies.hxx"
#include "Frame.hxx"

namespace PaddingPolicies::Temporal {
	template<typename PixelType>
	constexpr auto Zero = [](auto& VideoClip, auto Index, auto FrameContext, auto Core) {
		auto Format = VideoClip.Info->format;
		auto Width = VideoClip.Info->width;
		auto Height = VideoClip.Info->height;
		auto BlankFrame = Frame<PixelType>{ Core.AllocateFrame(Format, Width, Height, nullptr) };
		for (auto c : Range{ BlankFrame.Format->numPlanes })
			for (auto y : Range{ BlankFrame.Height[c] })
				for (auto x : Range{ BlankFrame.Width[c] })
					BlankFrame[c][y][x] = static_cast<PixelType>(0);
		return BlankFrame.Leak();
	};

	constexpr auto Repeat = [](auto& VideoClip, auto Index, auto FrameContext) {
		auto FrameCount = VideoClip.Info->numFrames;
		Index = Min(Max(Index, 0), FrameCount - 1);
		return VaporGlobals::API->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};

	constexpr auto Reflect = [](auto& VideoClip, auto Index, auto FrameContext) {
		auto FrameCount = VideoClip.Info->numFrames;
		Index = ReflectCoordinate(Index, FrameCount);
		return VaporGlobals::API->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};
}