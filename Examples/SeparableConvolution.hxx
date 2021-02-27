#pragma once
#include "Interface.vxx"

struct SeparableConvolution {
    field(InputClip, VideoNode{});
    field(HorizontalKernel, std::array{ 1., 2., 1. });
    field(VerticalKernel, std::array{ 0., 0., 0. });

public:
    static constexpr auto Signature = "clip:clip;h_kernel:float[]:opt;v_kernel:float[]:opt;";

public:
    SeparableConvolution(auto Arguments) {
        InputClip = Arguments["clip"];
        if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
            throw RuntimeError{ "only single precision floating point clips with constant format and dimensions supported." };
        if (Arguments["h_kernel"].Exists())
            if (Arguments["h_kernel"].size() == HorizontalKernel.size())
                HorizontalKernel = Arguments["h_kernel"];
            else
                throw RuntimeError{ "h_kernel must contain 3 scalar values." };
        if (Arguments["v_kernel"].Exists())
            if (Arguments["v_kernel"].size() == VerticalKernel.size())
                VerticalKernel = Arguments["v_kernel"];
            else
                throw RuntimeError{ "v_kernel must contain 3 scalar values." };
        else
            VerticalKernel = HorizontalKernel;
    }
    auto SpecifyMetadata() {
        return InputClip.ExtractMetadata();
    }
    auto AcquireResourcesForFrameGenerator(auto Index, auto FrameContext) {
        InputClip.RequestFrame(Index, FrameContext);
    }
    auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
        auto InputFrame = InputClip.FetchFrame<const float>(Index, FrameContext);
        auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrame);
        auto HorizontalConvolution = [this](auto Center) {
            auto [LeftWeight, CenterWeight, RightWeight] = HorizontalKernel;
            auto NormalizationFactor = LeftWeight + CenterWeight + RightWeight;
            auto WeightedSum = LeftWeight * Center[0][-1] + CenterWeight * Center[0][0] + RightWeight * Center[0][1];
            return WeightedSum / NormalizationFactor;
        };
        for (auto c : Range{ InputFrame.PlaneCount })
            for (auto y : Range{ InputFrame[c].Height })
                for (auto x : Range{ InputFrame[c].Width })
                    ProcessedFrame[c][y][x] = HorizontalConvolution(InputFrame[c].View(y, x));
        return ProcessedFrame;
    }
    auto RegisterInvokingSequence(auto&& Self, auto Core) {
        InputClip = Self("clip", InputClip, "h_kernel", HorizontalKernel);
        InputClip = Core["std"]["Transpose"]("clip", InputClip);
        InputClip = Self("clip", InputClip, "h_kernel", VerticalKernel);
        InputClip = Core["std"]["Transpose"]("clip", InputClip);
        return InputClip;
    }
};