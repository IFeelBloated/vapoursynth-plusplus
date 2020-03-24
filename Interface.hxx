#pragma once
#include "Map.hxx"
#include "Core.hxx"

namespace VaporInterface {
	template<typename FilterType>
	auto Initialize(auto, auto, auto InstanceData, auto Node, auto...) {
		auto Data = reinterpret_cast<FilterType*>(*InstanceData);
		auto [VideoInfo, NumberOfOutputs] = Data->ProvideMetadataForOutputs();
		VaporGlobals::API->setVideoInfo(VideoInfo, NumberOfOutputs, Node);
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
			return Data->DrawFrame(Index, VaporCore{ .Instance = Core }, FrameContext).Leak();
		return NullFrame;
	}

	template<typename FilterType>
	auto Create(auto InputMap, auto OutputMap, auto, auto Core, auto API) {
		VaporGlobals::API = API;
		auto Data = new FilterType{};
		auto Arguments = FilterArguments{ InputMap };
		auto Outputs = FilterOutputs{ OutputMap };
		if (auto InitializationStatus = Data->Initialize(Arguments, Outputs); InitializationStatus == false) {
			delete Data;
			return;
		}
		API->createFilter(InputMap, OutputMap, FilterType::Name, Initialize<FilterType>, Evaluate<FilterType>, Delete<FilterType>, FilterType::Mode, 0, Data, Core);
	}

	template<typename FilterType>
	auto RegisterFilter(auto Broker, auto Plugin) {
		Broker(FilterType::Name, FilterType::PythonInterface, VaporInterface::Create<FilterType>, nullptr, Plugin);
	}

	auto RegisterPlugin(auto Broker, auto Plugin) {
		Broker(VaporGlobals::Identifier, VaporGlobals::Namespace, VaporGlobals::Description, VAPOURSYNTH_API_VERSION, 1, Plugin);
	}
}