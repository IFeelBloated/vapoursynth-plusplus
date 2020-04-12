#pragma once
#include "Frame.hxx"

template<typename PixelType>
struct Buffer final {
	self(Width, 0_size);
	self(Height, 0_size);
	self(Canvas, std::vector<PixelType*>{});
	Buffer() = default;
	Buffer(auto Width, auto Height) {
		this->Width = Width;
		this->Height = Height;
		auto Origin = new PixelType[Width * Height];
		for (auto y : Range{ Height })
			Canvas.push_back(Origin + y * Width);
	}
	Buffer(const Buffer& OtherBuffer) {
		*this = OtherBuffer;
	}
	Buffer(Buffer&& OtherBuffer) {
		*this = std::move(OtherBuffer);
	}
	auto& operator=(const Buffer& OtherBuffer) {
		if (this != &OtherBuffer) {
			if (Width != OtherBuffer.Width || Height != OtherBuffer.Height) {
				this->~Buffer();
				new(this) Buffer{ OtherBuffer.Width, OtherBuffer.Height };
			}
			std::copy(OtherBuffer.Canvas[0], OtherBuffer.Canvas[0] + Width * Height, Canvas[0]);
		}
		return *this;
	}
	auto& operator=(Buffer&& OtherBuffer) {
		if (this != &OtherBuffer) {
			std::swap(Canvas, OtherBuffer.Canvas);
			Width = OtherBuffer.Width;
			Height = OtherBuffer.Height;
		}
		return *this;
	}
	~Buffer() {
		if (Canvas.size() != 0)
			delete[] Canvas[0];
	}
	auto operator[](auto y) {
		return Canvas[y];
	}
	auto AccessAsPlane(auto&& PaddingPolicy) {
		using PolicyType = std::decay_t<decltype(PaddingPolicy)>;
		return Plane<const PixelType, PolicyType>{ Canvas[0], Width, Height, Width, Forward(PaddingPolicy) };
	}
	auto AccessAsPlane() {
		return AccessAsPlane(PaddingPolicies::Spatial::Default);
	}
};