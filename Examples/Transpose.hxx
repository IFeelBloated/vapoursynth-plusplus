#pragma once
#include "../Include/Interface.hxx"

struct Transpose final {
	static constexpr auto Name = "Transpose";
	static constexpr auto Parameters = "clip:clip;";
	self(InputClip, Clip{});
	auto Initialize(auto Arguments, auto Console) {
		InputClip = Arguments["clip"];
		return true;
	}
	auto Preprocess(auto Core, auto Console) {
		auto TransposeFilter = Core["std"]["Transpose"];
		if (auto EvaluatedClip = TransposeFilter("clip", InputClip); TransposeFilter.EvaluationFailed())
			return Console.RaiseError(TransposeFilter.ErrorMessage);
		else
			return Console.Receive(EvaluatedClip);
	}
};