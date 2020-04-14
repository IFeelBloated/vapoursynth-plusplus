#pragma once
#include "Globals.hxx"
#include "SpatialPaddingPolicies.hxx"
#include "Frame.hxx"

namespace PaddingPolicies::Temporal {
	template<typename PixelType>
	constexpr auto Zero = [](auto& VideoClip, auto Index, auto FrameContext, auto Core) {
		auto Format = VideoClip.Format;
		auto Width = VideoClip.Width;
		auto Height = VideoClip.Height;
		auto BlankFrame = Frame<PixelType>{ Core.AllocateFrame(Format, Width, Height) };
		for (auto c : Range{ BlankFrame.PlaneCount })
			for (auto y : Range{ BlankFrame.Height[c] })
				for (auto x : Range{ BlankFrame.Width[c] })
					BlankFrame[c][y][x] = static_cast<PixelType>(0);
		return BlankFrame.Leak();
	};

	constexpr auto Repeat = [](auto& VideoClip, auto Index, auto FrameContext) {
		Index = Min(Max(Index, 0), VideoClip.FrameCount - 1);
		return VaporGlobals::API->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};

	constexpr auto Reflect = [](auto& VideoClip, auto Index, auto FrameContext) {
		Index = ReflectCoordinate(Index, VideoClip.FrameCount);
		return VaporGlobals::API->getFrameFilter(Index, VideoClip.VideoNode, FrameContext);
	};

	constexpr auto Default = Repeat;
}