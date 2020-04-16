#pragma once
#define RegisterAttributeToInspect(Attribute) \
template<typename FilterType> \
concept Attribute##Constraint = requires(FilterType) { &FilterType::Attribute; }; \
\
template<Attribute##Constraint FilterType> \
consteval auto Inspect##Attribute() { \
	return true; \
} \
\
template<typename FilterType> \
consteval auto Inspect##Attribute() { \
	return false; \
} \
\
template<typename FilterType> \
constexpr auto Defined##Attribute = Inspect##Attribute<FilterType>()

namespace VaporReflection {
	RegisterAttributeToInspect(DrawFrame);
	RegisterAttributeToInspect(Preprocess);
}