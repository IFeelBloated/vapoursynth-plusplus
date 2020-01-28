#pragma once
#include "Range.hxx"

template<typename PixelType, typename PolicyType>
struct Plane final {
	self(Width, 0_size);
	self(Height, 0_size);
	self(Canvas, std::vector<PixelType*>{});
	self(PaddingPolicy, PolicyType{});
	struct Proxy final {
		self(State, static_cast<Plane*>(nullptr));
		self(y, 0_ptrdiff);
		self(yOffset, 0_ptrdiff);
		self(xOffset, 0_ptrdiff);
		auto operator[](auto x) {
			auto yAbsolute = y + yOffset;
			auto xAbsolute = x + xOffset;
			if (xAbsolute < 0 || yAbsolute < 0 || xAbsolute >= State->Width || yAbsolute >= State->Height)
				return State->PaddingPolicy(State->Canvas.data(), State->Width, State->Height, yAbsolute, xAbsolute);
			else
				return State->Canvas[yAbsolute][xAbsolute];
		}
	};
	struct Offset final {
		self(State, static_cast<Plane*>(nullptr));
		self(yOffset, 0_ptrdiff);
		self(xOffset, 0_ptrdiff);
		auto operator[](auto y) {
			return Proxy{ .State = State, .y = y, .yOffset = yOffset, .xOffset = xOffset };
		}
		auto GetCoordinates() {
			return std::array{ yOffset, xOffset };
		}
		auto View(auto y, auto x) {
			return Offset{ .State = State, .yOffset = yOffset + y, .xOffset = xOffset + x };
		}
	};
	Plane(auto Pointer, auto Width, auto Height, auto&& PaddingPolicy) {
		this->Width = Width;
		this->Height = Height;
		this->PaddingPolicy = Forward(PaddingPolicy);
		auto Origin = reinterpret_cast<PixelType*>(Pointer);
		for (auto y : Range{ Height })
			Canvas.push_back(Origin + y * Width);
	}
	auto operator[](auto y) {
		if constexpr (std::is_const_v<PixelType>)
			return Proxy{ .State = this, .y = y };
		else
			return Canvas[y];
	}
	auto View(auto y, auto x) {
		return Offset{ .State = this, .yOffset = y, .xOffset = x };
	}
};

namespace PaddingPolicies {
	auto Zero = [](auto Canvas, auto Width, auto Height, auto y, auto x) {
		using PixelType = std::decay_t<decltype(Canvas[0][0])>;
		return static_cast<PixelType>(0);
	};

	auto Repeat = [](auto Canvas, auto Width, auto Height, auto y, auto x) {
		auto Max = [](auto x, auto y) { return x > y ? x : y; };
		auto Min = [](auto x, auto y) { return x < y ? x : y; };
		x = Min(Max(x, 0), Width - 1);
		y = Min(Max(y, 0), Height - 1);
		return Canvas[y][x];
	};

	auto Reflect = [](auto Canvas, auto Width, auto Height, auto y, auto x) {
		auto Bounce = [](auto x, auto Bound) {
			x = std::abs(x);
			x -= Bound - 1;
			x = -std::abs(x);
			x += Bound - 1;
			return x;
		};
		return Canvas[Bounce(y, Height)][Bounce(x, Width)];
	};
}