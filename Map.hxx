#pragma once
#include "Clip.hxx"

struct ArgumentList final {
	self(InputMap, static_cast<const VSMap*>(nullptr));
	struct Proxy final {
		self(InputMap, static_cast<const VSMap*>(nullptr));
		self(Parameter, "");
		self(Index, 0_ptrdiff);
		struct Iterator final {
			self(State, static_cast<Proxy*>(nullptr));
			self(Index, 0_ptrdiff);
			auto operator*() const {
				return Proxy{ .InputMap = State->InputMap, .Parameter = State->Parameter, .Index = Index };
			}
			auto& operator++() {
				++Index;
				return *this;
			}
			auto operator!=(auto&& OtherIterator) const {
				return Index != OtherIterator.Index;
			}
		};
		auto Size() {
			return std::max(VaporGlobals::API->propNumElements(InputMap, Parameter), 0);
		}
		auto Exists() {
			return Index < Size();
		}
		auto Begin() {
			return Iterator{ .State = this, .Index = 0 };
		}
		auto End() {
			return Iterator{ .State = this, .Index = Size() };
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

template<typename FilterType>
struct Controller final {
	self(OutputMap, static_cast<VSMap*>(nullptr));
	auto RaiseError(auto&& ErrorMessage) {
		auto Caption = FilterType::Name + ": "s;
		auto DecoratedMessage = Caption + ErrorMessage;
		VaporGlobals::API->setError(OutputMap, ExposeCString(DecoratedMessage));
	}
};