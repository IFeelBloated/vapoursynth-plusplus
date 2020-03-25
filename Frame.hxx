#pragma once
#include "Globals.hxx"
#include "Plane.hxx"
#include "SpatialPaddingPolicies.hxx"

template<typename PixelType>
struct Frame final {
	static constexpr auto DefaultPaddingPolicy = PaddingPolicies::Spatial::Repeat;
	using DefaultPolicyType = std::decay_t<decltype(DefaultPaddingPolicy)>;
	using DefaultPlaneType = Plane<PixelType, DefaultPolicyType>;
	self(RawFrame, static_cast<VSFrameRef*>(nullptr));
	self(Planes, std::array{ DefaultPlaneType{}, DefaultPlaneType{}, DefaultPlaneType{} });
	self(Format, static_cast<const VSFormat*>(nullptr));
	Frame() = default;
	Frame(auto RawFrame) {
		this->RawFrame = RawFrame;
		this->Format = VaporGlobals::API->getFrameFormat(RawFrame);
		for (auto Index : Range{ Format->numPlanes })
			Planes[Index] = GetPlane(Index, DefaultPaddingPolicy);
	}
	Frame(const Frame& OtherFrame) {
		*this = OtherFrame;
	}
	Frame(Frame&& OtherFrame) {
		*this = std::move(OtherFrame);
	}
	auto& operator=(const Frame& OtherFrame) {
		if (this != &OtherFrame) {
			this->~Frame();
			RawFrame = PointerRemoveConstant(VaporGlobals::API->cloneFrameRef(OtherFrame.RawFrame));
			Planes = OtherFrame.Planes;
			Format = OtherFrame.Format;
		}
		return *this;
	}
	auto& operator=(Frame&& OtherFrame) {
		if (this != &OtherFrame) {
			std::swap(RawFrame, OtherFrame.RawFrame);
			Planes = std::move(OtherFrame.Planes);
			Format = OtherFrame.Format;
		}
		return *this;
	}
	~Frame() {
		VaporGlobals::API->freeFrame(RawFrame);
	}
	auto GetPlane(auto Index, auto&& PaddingPolicy) {
		using PolicyType = std::decay_t<decltype(PaddingPolicy)>;
		auto Width = VaporGlobals::API->getFrameWidth(RawFrame, Index);
		auto Height = VaporGlobals::API->getFrameHeight(RawFrame, Index);
		auto GetPlanePointer = [&]() {
			if constexpr (std::is_const_v<PixelType>)
				return VaporGlobals::API->getReadPtr(RawFrame, Index);
			else
				return VaporGlobals::API->getWritePtr(RawFrame, Index);
		};
		return Plane<PixelType, PolicyType>{ GetPlanePointer(), Width, Height, Forward(PaddingPolicy) };
	}
	auto& operator[](auto Index) {
		return Planes[Index];
	}
	auto Leak() {
		auto LeakedFrame = RawFrame;
		RawFrame = nullptr;
		return PointerAddConstant(LeakedFrame);
	}
	operator auto() {
		if constexpr (std::is_const_v<PixelType>)
			return PointerAddConstant(RawFrame);
		else
			return RawFrame;
	}
};