#pragma once
#include "Clip.hxx"

struct ReadonlyItem final {
	self(Map, static_cast<const VSMap*>(nullptr));
	self(Key, "");
	self(Index, 0_ptrdiff);
	struct Iterator final {
		self(State, static_cast<ReadonlyItem*>(nullptr));
		self(Index, 0_ptrdiff);
		auto operator*() const {
			return ReadonlyItem{ .Map = State->Map, .Key = State->Key, .Index = Index };
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
		return Max(VaporGlobals::API->propNumElements(Map, Key), 0);
	}
	auto Exists() {
		return Index < Size();
	}
	auto Type() {
		return VaporGlobals::API->propGetType(Map, Key);
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
		return Clip{ VaporGlobals::API->propGetNode(Map, Key, Index, nullptr) };
	}
	operator double() {
		return VaporGlobals::API->propGetFloat(Map, Key, Index, nullptr);
	}
	operator float() {
		return static_cast<float>(VaporGlobals::API->propGetFloat(Map, Key, Index, nullptr));
	}
	operator std::int64_t() {
		return VaporGlobals::API->propGetInt(Map, Key, Index, nullptr);
	}
	operator int() {
		return static_cast<int>(VaporGlobals::API->propGetInt(Map, Key, Index, nullptr));
	}
	operator bool() {
		return !!VaporGlobals::API->propGetInt(Map, Key, Index, nullptr);
	}
	operator std::string() {
		return std::string{ VaporGlobals::API->propGetData(Map, Key, Index, nullptr) };
	}
};

struct WritableItem final {
	self(Map, static_cast<VSMap*>(nullptr));
	self(Key, "");
	auto Erase() {

	}
	auto& operator=(auto&&) {
		return *this;
	}
	auto& operator+=(auto&&) {
		return *this;
	}
	auto& operator|=(auto&&) {
		return *this;
	}
};

struct ArgumentList final {
	self(InputMap, static_cast<const VSMap*>(nullptr));
	auto operator[](auto&& Parameter) {
		thread_local auto ParameterHolder = ""s;
		if constexpr (std::is_same_v<decltype(Parameter), std::string&&>) {
			ParameterHolder = std::move(Parameter);
			return ReadonlyItem{ .Map = InputMap, .Key = ParameterHolder.data() };
		}
		else
			return ReadonlyItem{ .Map = InputMap, .Key = ExposeCString(Parameter) };
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

template<typename PixelType>
auto Frame<PixelType>::GetProperty(auto&& Key) {
	thread_local auto KeyHolder = ""s;
	auto FetchItem = [this](auto ExposedKey) {
		if constexpr (std::is_const_v<PixelType>)
			return ReadonlyItem{ .Map = PropertyMap, .Key = ExposedKey };
		else
			return WritableItem{ .Map = PropertyMap, .Key = ExposedKey };
	};
	if constexpr (std::is_same_v<decltype(Key), std::string&&>) {
		KeyHolder = std::move(Key);
		return FetchItem(KeyHolder.data());
	}
	else
		return FetchItem(ExposeCString(Key));
}