#pragma once
#include "Core.vxx"

struct SeparableConvolution {
    field(InputClip, VideoNode{});
    field(Kernel, std::array{ 0., 0., 0. });

public:
    static constexpr auto Signature = "clip: vnode, [h_kernel, v_kernel]: float[]?";
    static auto InitiateWorkflow(auto Self, auto Arguments, auto Core) {
        auto InputClip = static_cast<VideoNode>(Arguments["clip"]);
        auto HorizontalKernel = std::array{ 1., 2., 1. };
        auto VerticalKernel = std::array{ 0., 0., 0. };
        if (!InputClip.WithConstantFormat() || !InputClip.WithConstantDimensions() || !InputClip.IsSinglePrecision())
            throw std::runtime_error{ "only single precision floating point clips with constant format and dimensions supported." };
        if (Arguments["h_kernel"].Exists())
            if (Arguments["h_kernel"].size() == HorizontalKernel.size())
                HorizontalKernel = Arguments["h_kernel"];
            else
                throw std::runtime_error{ "h_kernel must contain 3 scalar values." };
        if (Arguments["v_kernel"].Exists())
            if (Arguments["v_kernel"].size() == VerticalKernel.size())
                VerticalKernel = Arguments["v_kernel"];
            else
                throw std::runtime_error{ "v_kernel must contain 3 scalar values." };
        else
            VerticalKernel = HorizontalKernel;
        InputClip = Self("clip", InputClip, "kernel", HorizontalKernel);
        InputClip = Core["std"]["Transpose"]("clip", InputClip);
        InputClip = Self("clip", InputClip, "kernel", VerticalKernel);
        return Core["std"]["Transpose"]("clip", InputClip);
    }

public:
    SeparableConvolution(auto Arguments) {
        InputClip = Arguments["clip"];
        Kernel = Arguments["kernel"];
    }
    auto SpecifyMetadata() {
        return InputClip.ExtractMetadata();
    }
    auto GenerateFrame(auto Index, auto FrameContext, auto Core) {
        auto InputFrame = InputClip.AcquireFrame<const float>(Index, FrameContext);
        auto ProcessedFrame = Core.CreateBlankFrameFrom(InputFrame);
        auto HorizontalConvolution = [this](auto Center) {
            auto [WeightOfTheLeftNeighbor, WeightOfTheCenter, WeightOfTheRightNeighbor] = Kernel;
            auto NormalizationFactor = WeightOfTheLeftNeighbor + WeightOfTheCenter + WeightOfTheRightNeighbor;
            auto WeightedSum = WeightOfTheLeftNeighbor * Center[0][-1] + WeightOfTheCenter * Center[0][0] + WeightOfTheRightNeighbor * Center[0][1];
            return WeightedSum / NormalizationFactor;
        };
        for (auto c : Range{ InputFrame.PlaneCount })
            for (auto y : Range{ InputFrame[c].Height })
                for (auto x : Range{ InputFrame[c].Width })
                    ProcessedFrame[c][y][x] = HorizontalConvolution(InputFrame[c].View(y, x));
        return ProcessedFrame;
    }
};