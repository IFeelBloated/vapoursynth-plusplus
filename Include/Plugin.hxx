#pragma once
#include "Map.hxx"

struct VaporPlugin final {
	self(Plugin, static_cast<VSPlugin*>(nullptr));
	struct VaporFilter final {
		self(Plugin, static_cast<VSPlugin*>(nullptr));
		self(Name, ""s);
		auto ForwardArguments(auto ArgumentMap, auto&& Parameter, auto&& Argument, auto&& ...ArgumentQueue) {
			auto MaterializedParameter = WritableItem{ .Map = ArgumentMap, .Key = Forward(Parameter) };
			if constexpr (hasattr(Argument, Begin) && !isinstance(Argument, std::string) && !isinstance(Argument, std::string_view))
				for (auto&& x : Argument)
					MaterializedParameter += x;
			else
				MaterializedParameter = Argument;
			if constexpr (sizeof...(ArgumentQueue) != 0)
				ForwardArguments(ArgumentMap, Forward(ArgumentQueue)...);
		}
		auto operator()(auto&& ...Arguments) {
			auto ArgumentMap = VaporGlobals::API->createMap();
			if constexpr (sizeof...(Arguments) != 0)
				ForwardArguments(ArgumentMap, Forward(Arguments)...);
			auto EvaluatedItems = VaporGlobals::API->invoke(Plugin, Name.data(), ArgumentMap);
			auto EvaluatedClip = VaporGlobals::API->propGetNode(EvaluatedItems, "clip", 0, nullptr);
			VaporGlobals::API->freeMap(EvaluatedItems);
			VaporGlobals::API->freeMap(ArgumentMap);
			return Clip{ EvaluatedClip };
		}
	};
	auto operator[](auto&& FilterName) {
		return VaporFilter{ .Plugin = Plugin, .Name = Forward(FilterName) };
	}
};