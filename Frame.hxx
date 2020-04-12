#pragma once
#include "Globals.hxx"
#include "Plane.hxx"
#include "SpatialPaddingPolicies.hxx"

template<typename PixelType>
struct Frame final {
	using DefaultPolicyType = std::decay_t<decltype(PaddingPolicies::Spatial::Default)>;
	using DefaultPlaneType = Plane<PixelType, DefaultPolicyType>;
	self(RawFrame, static_cast<VSFrameRef*>(nullptr));
	self(Planes, std::array{ DefaultPlaneType{}, DefaultPlaneType{}, DefaultPlaneType{} });
	self(Format, static_cast<const VSFormat*>(nullptr));
	self(PropertyMap, static_cast<VSMap*>(nullptr));
	Frame() = default;
	Frame(auto RawFrame) {
		this->RawFrame = RawFrame;
		this->Format = VaporGlobals::API->getFrameFormat(RawFrame);
		for (auto Index : Range{ Format->numPlanes })
			Planes[Index] = GetPlane(Index, PaddingPolicies::Spatial::Default);
		if constexpr (std::is_const_v<PixelType>)
			PropertyMap = PointerRemoveConstant(VaporGlobals::API->getFramePropsRO(RawFrame));
		else
			PropertyMap = VaporGlobals::API->getFramePropsRW(RawFrame);
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
			PropertyMap = OtherFrame.PropertyMap;
		}
		return *this;
	}
	auto& operator=(Frame&& OtherFrame) {
		if (this != &OtherFrame) {
			std::swap(RawFrame, OtherFrame.RawFrame);
			Planes = std::move(OtherFrame.Planes);
			Format = OtherFrame.Format;
			PropertyMap = OtherFrame.PropertyMap;
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
		auto Stride = VaporGlobals::API->getStride(RawFrame, Index) / sizeof(PixelType);
		auto GetPlanePointer = [&]() {
			if constexpr (std::is_const_v<PixelType>)
				return VaporGlobals::API->getReadPtr(RawFrame, Index);
			else
				return VaporGlobals::API->getWritePtr(RawFrame, Index);
		};
		return Plane<PixelType, PolicyType>{ GetPlanePointer(), Width, Height, Stride, Forward(PaddingPolicy) };
	}
	auto GetProperty(auto&&);
	decltype(auto) operator[](auto&& x) {
		if constexpr (isinstance(x, const char*) || isinstance(x, char*) || isinstance(x, std::string) || isinstance(x, std::string_view))
			return GetProperty(Forward(x));
		else
			return Planes[x];
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