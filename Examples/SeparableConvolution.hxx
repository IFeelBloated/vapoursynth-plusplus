#pragma once
#include "Interface.vxx"

struct SeparableConvolution {
    field(InputClip, VideoNode{});
    field(HorizontalKernel, std::array{ 1., 2., 1. });
    field(VerticalKernel, std::array{ 0., 0., 0. });

public:
    static constexpr auto Name = "SeparableConvolution";
    static constexpr auto Signature = "clip:clip;h_kernel:float[]:opt;v_kernel:float[]:opt;";

public:
    SeparableConvolution(auto Arguments) {
        InputClip = Arguments["clip"];
        if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
            throw RuntimeError{ "only single precision floating point clips with constant format and dimensions supported." };
        if (Arguments["h_kernel"].Exists())
            if (Arguments["h_kernel"].size() == HorizontalKernel.size())
                for (auto x : Range{ HorizontalKernel.size() })
                    HorizontalKernel[x] = Arguments["h_kernel"][x];
            else
                throw RuntimeError{ "h_kernel must contain 3 scalar values." };
        if (Arguments["v_kernel"].Exists())
            if (Arguments["v_kernel"].size() == VerticalKernel.size())
                for (auto x : Range{ VerticalKernel.size() })
                    VerticalKernel[x] = Arguments["v_kernel"][x];
            else
                throw RuntimeError{ "v_kernel must contain 3 scalar values." };
        else
            VerticalKernel = HorizontalKernel;
    }
    auto RegisterMetadata(auto Core) {
        return InputClip.ExtractMetadata();
    }
    auto RequestReferenceFrames(auto Index, auto FrameContext) {
        InputClip.RequestFrame(Index, FrameContext);
    }
    auto DrawFrame(auto Index, auto Core, auto FrameContext) {
        auto InputFrame = InputClip.FetchFrame<const float>(Index, FrameContext);
        auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrame);
        auto HorizontalConvolution = [this](auto Center) {
            auto [LeftWeight, CenterWeight, RightWeight] = HorizontalKernel;
            auto NormalizationFactor = LeftWeight + CenterWeight + RightWeight;
            auto Conv = Center[0][-1] * LeftWeight + Center[0][0] * CenterWeight + Center[0][1] * RightWeight;
            return Conv / NormalizationFactor;
        };
        for (auto c : Range{ InputFrame.PlaneCount })
            for (auto y : Range{ InputFrame[c].Height })
                for (auto x : Range{ InputFrame[c].Width })
                    ProcessedFrame[c][y][x] = HorizontalConvolution(InputFrame[c].View(y, x));
        return ProcessedFrame.Leak();
    }
    auto RegisterInvokingSequence(auto&& Self, auto Core) {
        InputClip = Self("clip", InputClip, "h_kernel", HorizontalKernel);
        InputClip = Core["std"]["Transpose"]("clip", InputClip);
        InputClip = Self("clip", InputClip, "h_kernel", VerticalKernel);
        InputClip = Core["std"]["Transpose"]("clip", InputClip);
        return InputClip;
    }
};