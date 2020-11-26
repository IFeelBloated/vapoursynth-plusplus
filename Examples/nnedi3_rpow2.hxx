#pragma once
#include "Interface.vxx"

struct nnedi3_rpow2 final {
    static constexpr auto Name = "nnedi3_rpow2";
    static constexpr auto Parameters = "clip:clip;rfactor:int;nsize:int:opt;nns:int:opt;qual:int:opt;etype:int:opt;pscrn:int:opt;";
    self(InputClip, VideoNode{});
    self(RFactor, 0);
    self(NSize, 0);
    self(NNS, 3);
    self(Qual, 1);
    self(EType, 0);
    self(PSCRN, 2);
    auto LogarithmizeRFactor() {
        auto LinearFactor = 0;
        for (auto ExponentialFactor = 1; ExponentialFactor < RFactor; ExponentialFactor <<= 1)
            ++LinearFactor;
        return LinearFactor;
    }
    nnedi3_rpow2(auto Arguments) {
        InputClip = Arguments["clip"];
        if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.Is444())
            throw RuntimeError{ "clips with subsampled format not supported!" };
        RFactor = Arguments["rfactor"];
        if (RFactor != 1 << LogarithmizeRFactor())
            throw RuntimeError{ "rfactor must be a power of 2" };
        if (Arguments["nsize"].Exists())
            NSize = Arguments["nsize"];
        if (Arguments["nns"].Exists())
            NNS = Arguments["nns"];
        if (Arguments["qual"].Exists())
            Qual = Arguments["qual"];
        if (Arguments["etype"].Exists())
            EType = Arguments["etype"];
        if (Arguments["pscrn"].Exists())
            PSCRN = Arguments["pscrn"];
        else if (InputClip.IsSinglePrecision())
            PSCRN = 1;
    }
    auto RegisterInvokingSequence(auto Core, auto&& SelfInvoker) {
        for (auto x : Range{ LogarithmizeRFactor() }) {
            auto Field = x == 0 ? 1 : 0;
            InputClip = Core["nnedi3"]["nnedi3"]("clip", InputClip, "field", Field, "dh", true, "nsize", NSize, "nns", NNS, "qual", Qual, "etype", EType, "pscrn", PSCRN);
            InputClip = Core["std"]["Transpose"]("clip", InputClip);
            InputClip = Core["nnedi3"]["nnedi3"]("clip", InputClip, "field", Field, "dh", true, "nsize", NSize, "nns", NNS, "qual", Qual, "etype", EType, "pscrn", PSCRN);
            InputClip = Core["std"]["Transpose"]("clip", InputClip);
        }
        return InputClip;
    }
};