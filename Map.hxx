#pragma once
#include "Clip.hxx"

struct FilterArguments final {
	self(InputMap, static_cast<const VSMap*>(nullptr));
	auto FetchValue(auto Broker, auto&& Name, auto Index) {
		auto ErrorState = 0;
		auto Value = Broker(InputMap, ExposeCString(Name), Index, &ErrorState);
		return std::tuple{ Value, ErrorState != 0 };
	}
	auto Fetch(auto& Parameter, auto&& Name) {
		if constexpr (isinstance(Parameter, Clip)) {
			if (auto [Value, Error] = FetchValue(VaporGlobals::API->propGetNode, Name, 0); Error == false)
				Parameter = Clip{ Value };
		}
		else if constexpr (isinstance(Parameter, double)) {
			if (auto [Value, Error] = FetchValue(VaporGlobals::API->propGetFloat, Name, 0); Error == false)
				Parameter = Value;
		}
	}
};

struct FilterOutputs final {
	self(OutputMap, static_cast<VSMap*>(nullptr));
	auto RaiseError(auto&& ErrorMessage) {
		VaporGlobals::API->setError(OutputMap, ExposeCString(ErrorMessage));
	}
};