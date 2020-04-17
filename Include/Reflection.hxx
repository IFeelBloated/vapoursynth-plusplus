#pragma once
#include "Infrastructure.hxx"

#define RegisterAttributeToInspect(Attribute) \
template<typename FilterType> \
concept Attribute##Constraint = requires(FilterType) { &FilterType::Attribute; }; \
template<Attribute##Constraint FilterType> \
consteval auto Inspect##Attribute() { \
	return true; \
} \
template<typename FilterType> \
consteval auto Inspect##Attribute() { \
	return false; \
}
#define hasattr(Object, Attribute) Inspect##Attribute<std::decay_t<std::remove_pointer_t<decltype(Object)>>>()

RegisterAttributeToInspect(DrawFrame)
RegisterAttributeToInspect(Preprocess)
RegisterAttributeToInspect(Begin)