#pragma once
#include "Frame.hxx"
#include "TemporalPaddingPolicies.hxx"

struct Clip final {
	static constexpr auto DefaultPaddingPolicy = PaddingPolicies::Temporal::Repeat;
	self(VideoNode, static_cast<VSNodeRef*>(nullptr));
	self(Info, static_cast<const VSVideoInfo*>(nullptr));
	template<typename ContainerType>
	struct Sequence final {
		self(Container, ContainerType{});
		self(Radius, 0_ptrdiff);
		struct Offset final {
			self(State, static_cast<Sequence*>(nullptr));
			self(tOffset, 0_ptrdiff);
			auto& operator[](auto t) {
				auto& ReferenceSequence = *State;
				return ReferenceSequence[t + tOffset];
			}
			auto GetCoordinate() {
				return tOffset;
			}
			auto View(auto t) {
				return Offset{ .State = State, .tOffset = tOffset + t };
			}
		};
		auto& operator[](auto t) {
			return Container[t + Radius];
		}
		auto View(auto t) {
			return Offset{ .State = this, .tOffset = t };
		}
	};
	Clip() = default;
	Clip(auto VideoNode) {
		this->VideoNode = VideoNode;
		this->Info = VaporGlobals::API->getVideoInfo(VideoNode);
	}
	auto& operator=(const Clip& OtherClip) {
		if (this != &OtherClip) {
			this->~Clip();
			VideoNode = VaporGlobals::API->cloneNodeRef(OtherClip.VideoNode);
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
	Clip(const Clip& OtherClip) {
		*this = OtherClip;
	}
	Clip(Clip&& OtherClip) {
		*this = std::move(OtherClip);
	}
	~Clip() {
		VaporGlobals::API->freeNode(VideoNode);
	}
	auto RequestFrame(auto Index, auto FrameContext) {
		if (Index >= 0 && Index < Info->numFrames)
			VaporGlobals::API->requestFrameFilter(Index, VideoNode, FrameContext);
	}
	auto RequestFrames(auto Index, auto Radius, auto FrameContext) {
		for (auto Offset : Range{ Index - Radius, Index + Radius + 1 })
			RequestFrame(Offset, FrameContext);
	}
	template<typename PixelType>
	auto GetFrame(auto Index, auto&& PaddingPolicy, auto FrameContext, auto&& ...AuxiliaryArguments) {
		auto WrapAsFrame = [](auto RawFrame) {
			return Frame<PixelType>{ PointerRemoveConstant(RawFrame) };
		};
		if (Index < 0 || Index >= Info->numFrames)
			return WrapAsFrame(PaddingPolicy(*this, Index, FrameContext, Forward(AuxiliaryArguments)...));
		else
			return WrapAsFrame(VaporGlobals::API->getFrameFilter(Index, VideoNode, FrameContext));
	}
	template<typename PixelType>
	auto GetFrame(auto Index, auto FrameContext) {
		return GetFrame<PixelType>(Index, DefaultPaddingPolicy, FrameContext);
	}
	template<typename PixelType>
	auto GetFrames(auto Index, auto Radius, auto&& PaddingPolicy, auto FrameContext, auto&& ...AuxiliaryArguments) {
		using ContainerType = std::vector<Frame<PixelType>>;
		auto Container = ContainerType{};
		for (auto Offset : Range{ Index - Radius, Index + Radius + 1 })
			Container.push_back(GetFrame<PixelType>(Offset, PaddingPolicy, FrameContext, Forward(AuxiliaryArguments)...));
		return Sequence<ContainerType>{.Container = std::move(Container), .Radius = Radius };
	}
	template<typename PixelType>
	auto GetFrames(auto Index, auto Radius, auto FrameContext) {
		return GetFrames<PixelType>(Index, Radius, DefaultPaddingPolicy, FrameContext);
	}
	auto& GetMetadata() {
		return *Info;
	}
	auto IsSinglePrecision() {
		return Info->format->sampleType == stFloat && Info->format->bitsPerSample == 32;
	}
	auto WithConstantDimensions() {
		return Info->width != 0 && Info->height != 0;
	}
	auto WithConstantFormat() {
		return Info->format != nullptr;
	}
};