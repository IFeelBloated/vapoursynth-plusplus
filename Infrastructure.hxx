#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <new>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <cmath>

#define self(ClassMember, ...) std::decay_t<decltype(__VA_ARGS__)> ClassMember = __VA_ARGS__
#define isinstance(Object, Type) std::is_same_v<std::decay_t<decltype(Object)>, Type>
#define Forward(Object) std::forward<decltype(Object)>(Object)
#define Begin begin
#define End end
#define Size size
#define Width width
#define Height height

using namespace std::literals;

constexpr auto operator""_size(unsigned long long Value) {
	return static_cast<std::size_t>(Value);
}

constexpr auto operator""_ptrdiff(unsigned long long Value) {
	return static_cast<std::ptrdiff_t>(Value);
}

constexpr auto operator""_u64(unsigned long long Value) {
	return static_cast<std::uint64_t>(Value);
}

constexpr auto operator""_i64(unsigned long long Value) {
	return static_cast<std::int64_t>(Value);
}

constexpr auto operator""_u32(unsigned long long Value) {
	return static_cast<std::uint32_t>(Value);
}

constexpr auto operator""_i32(unsigned long long Value) {
	return static_cast<std::int32_t>(Value);
}

auto PointerRemoveConstant(auto Pointer) {
	using UnderlyingType = std::decay_t<decltype(*Pointer)>;
	return const_cast<UnderlyingType*>(Pointer);
}

auto PointerAddConstant(auto Pointer) {
	using UnderlyingType = std::decay_t<decltype(*Pointer)>;
	return const_cast<const UnderlyingType*>(Pointer);
}

auto Max(auto x, auto y) {
	return x > y ? x : y;
}

auto Min(auto x, auto y) {
	return x < y ? x : y;
}

auto ExposeCString(auto&& String) {
	if constexpr (isinstance(String, const char*) || isinstance(String, char*))
		return String;
	else
		return String.data();
}