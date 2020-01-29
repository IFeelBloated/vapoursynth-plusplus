#pragma once
#include "Globals.hxx"
#include "Plane.hxx"

template<typename PixelType, typename Filter>
struct Frame final {
	self(RawFrame, static_cast<VSFrameRef*>(nullptr));
	self(Width, std::array{ 0, 0, 0 });
	self(Height, std::array{ 0, 0, 0 });
	self(Format, static_cast<const VSFormat*>(nullptr));
	Frame() = default;
	Frame(auto RawFrame) {
		this->RawFrame = RawFrame;
		this->Format = VaporGlobals::API<Filter>->getFrameFormat(RawFrame);
		for (auto Index : Range{ Format->numPlanes }) {
			Width[Index] = VaporGlobals::API<Filter>->getFrameWidth(RawFrame, Index);
			Height[Index] = VaporGlobals::API<Filter>->getFrameHeight(RawFrame, Index);
		}
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
			RawFrame = PointerRemoveConstant(VaporGlobals::API<Filter>->cloneFrameRef(OtherFrame.RawFrame));
			Width = OtherFrame.Width;
			Height = OtherFrame.Height;
			Format = OtherFrame.Format;
		}
		return *this;
	}
	auto& operator=(Frame&& OtherFrame) {
		if (this != &OtherFrame) {
			std::swap(RawFrame, OtherFrame.RawFrame);
			Width = std::move(OtherFrame.Width);
			Height = std::move(OtherFrame.Height);
			Format = OtherFrame.Format;
		}
		return *this;
	}
	~Frame() {
		if (RawFrame != nullptr)
			VaporGlobals::API<Filter>->freeFrame(RawFrame);
	}
	auto GetPlane(auto Index, auto&& PaddingPolicy) {
		using PolicyType = std::decay_t<decltype(PaddingPolicy)>;
		auto GetPlanePointer = [&]() {
			if constexpr (std::is_const_v<PixelType>)
				return VaporGlobals::API<Filter>->getReadPtr(RawFrame, Index);
			else
				return VaporGlobals::API<Filter>->getWritePtr(RawFrame, Index);
		};
		return Plane<PixelType, PolicyType>{ GetPlanePointer(), Width[Index], Height[Index], Forward(PaddingPolicy) };
	}
	auto operator[](auto Index) {
		return GetPlane(Index, SpatialPaddingPolicies::Repeat);
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