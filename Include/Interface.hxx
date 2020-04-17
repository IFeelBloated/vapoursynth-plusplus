#pragma once
#include "Core.hxx"
#include "Buffer.hxx"

namespace VaporInterface {
	template<typename FilterType>
	auto Initialize(auto, auto, auto InstanceData, auto Node, auto Core, auto...) {
		auto Data = reinterpret_cast<FilterType*>(*InstanceData);
		auto VideoInfo = Data->RegisterVideoInfo(VaporCore{ .Instance = Core });
		VaporGlobals::API->setVideoInfo(&VideoInfo, 1, Node);
	}

	template<typename FilterType>
	auto Delete(auto InstanceData, auto...) {
		auto Data = reinterpret_cast<FilterType*>(InstanceData);
		delete Data;
	}

	template<typename FilterType>
	auto Evaluate(auto Index, auto ActivationReason, auto InstanceData, auto, auto FrameContext, auto Core, auto...) {
		auto Data = reinterpret_cast<FilterType*>(*InstanceData);
		auto NullFrame = static_cast<const VSFrameRef*>(nullptr);
		if (ActivationReason == VSActivationReason::arInitial)
			Data->RequestReferenceFrames(Index, FrameContext);
		else if (ActivationReason == VSActivationReason::arAllFramesReady)
			return Data->DrawFrame(Index, VaporCore{ .Instance = Core }, VaporFrameContext<FilterType>{ .Context = FrameContext });
		return NullFrame;
	}

	template<typename FilterType>
	auto Create(auto InputMap, auto OutputMap, auto, auto Core, auto...) {
		auto Data = new FilterType{};
		auto Arguments = ArgumentList{ InputMap };
		auto Console = Controller<FilterType>{ OutputMap };
		if (auto InitializationStatus = Data->Initialize(Arguments, Console); InitializationStatus == false) {
			delete Data;
			return;
		}
		if constexpr (hasattr(Data, Preprocess))
			Data->Preprocess(VaporCore{ .Instance = Core }, Console);
		if constexpr (hasattr(Data, DrawFrame))
			VaporGlobals::API->createFilter(InputMap, OutputMap, FilterType::Name, Initialize<FilterType>, Evaluate<FilterType>, Delete<FilterType>, FilterType::Mode, 0, Data, Core);
		else
			delete Data;
	}

	template<typename FilterType>
	auto RegisterFilter(auto Broker, auto Plugin) {
		Broker(FilterType::Name, FilterType::Parameters, Create<FilterType>, nullptr, Plugin);
	}

	auto RegisterPlugin(auto Broker, auto Plugin) {
		VaporGlobals::API = getVapourSynthAPI(VAPOURSYNTH_API_VERSION);
		Broker(VaporGlobals::Identifier, VaporGlobals::Namespace, VaporGlobals::Description, VAPOURSYNTH_API_VERSION, 1, Plugin);
	}
}