#pragma once
#include "Clip.hxx"

struct ArgumentList final {
	self(InputMap, static_cast<const VSMap*>(nullptr));
	struct Proxy final {
		self(InputMap, static_cast<const VSMap*>(nullptr));
		self(Parameter, "");
		self(Index, 0_ptrdiff);
		auto Size() {
			return VaporGlobals::API->propNumElements(InputMap, Parameter);
		}
		auto Exists() {
			return Index < Size();
		}
		auto& operator[](auto Index) {
			this->Index = Index;
			return *this;
		}
		operator Clip() {
			return Clip{ VaporGlobals::API->propGetNode(InputMap, Parameter, Index, nullptr) };
		}
		operator double() {
			return VaporGlobals::API->propGetFloat(InputMap, Parameter, Index, nullptr);
		}
		operator float() {
			return static_cast<float>(VaporGlobals::API->propGetFloat(InputMap, Parameter, Index, nullptr));
		}
		operator std::int64_t() {
			return VaporGlobals::API->propGetInt(InputMap, Parameter, Index, nullptr);
		}
		operator int() {
			return static_cast<int>(VaporGlobals::API->propGetInt(InputMap, Parameter, Index, nullptr));
		}
		operator bool() {
			return !!VaporGlobals::API->propGetInt(InputMap, Parameter, Index, nullptr);
		}
		operator std::string() {
			return std::string{ VaporGlobals::API->propGetData(InputMap, Parameter, Index, nullptr) };
		}
	};
	auto operator[](auto&& Parameter) {
		return Proxy{ .InputMap = InputMap, .Parameter = ExposeCString(Parameter) };
	}
};

struct Controller final {
	self(OutputMap, static_cast<VSMap*>(nullptr));
	auto RaiseError(auto&& ErrorMessage) {
		VaporGlobals::API->setError(OutputMap, ExposeCString(ErrorMessage));
	}
};