#pragma once
#include "Globals.hxx"
#include "Plane.hxx"

template<typename PixelType>
struct Frame final {
	self(RawFrame, static_cast<VSFrameRef*>(nullptr));
	self(Width, std::array{ 0, 0, 0 });
	self(Height, std::array{ 0, 0, 0 });
	self(Format, static_cast<const VSFormat*>(nullptr));
	Frame() = default;
	Frame(auto RawFrame) {
		this->RawFrame = RawFrame;
		this->Format = VaporGlobals::API->getFrameFormat(RawFrame);
		for (auto Index : Range{ Format->numPlanes }) {
			Width[Index] = VaporGlobals::API->getFrameWidth(RawFrame, Index);
			Height[Index] = VaporGlobals::API->getFrameHeight(RawFrame, Index);
		}
	}
	Frame(const Frame&) = delete;
	Frame(Frame&&) = delete;
	auto& operator=(const Frame&) = delete;
	auto& operator=(Frame&&) = delete;
	~Frame() {
		if constexpr (std::is_const_v<PixelType>)
			VaporGlobals::API->freeFrame(RawFrame);
	}
	auto GetPlane(auto Index, auto&& PaddingPolicy) {
		using PolicyType = std::decay_t<decltype(PaddingPolicy)>;
		auto GetPlanePointer = [&]() {
			if constexpr (std::is_const_v<PixelType>)
				return VaporGlobals::API->getReadPtr(RawFrame, Index);
			else
				return VaporGlobals::API->getWritePtr(RawFrame, Index);
		};
		return Plane<PixelType, PolicyType>{ GetPlanePointer(), Width[Index], Height[Index], Forward(PaddingPolicy) };
	}
	auto operator[](auto Index) {
		return GetPlane(Index, PaddingPolicies::Repeat);
	}
};