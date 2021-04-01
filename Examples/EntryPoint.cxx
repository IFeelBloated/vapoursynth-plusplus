#include "Interface.vxx"
#include "GaussBlur.hxx"
#include "GaussBlurFast.hxx"
#include "TemporalMedian.hxx"
#include "Crop.hxx"
#include "Rec601ToRGB.hxx"
#include "SeparableConvolution.hxx"
#include "ModifyFrame.hxx"
#include "Palette.hxx"

VS_EXTERNAL_API(auto) VapourSynthPluginInit(VSConfigPlugin configFunc, VSRegisterFunction, VSPlugin* plugin) {
	VaporGlobals::Identifier = "com.vsfilterscript.test";
	VaporGlobals::Namespace = "test";
	VaporGlobals::Description = "Test filters for vsFilterScript";
	VaporGlobals::PluginHandle = plugin;
	VaporInterface::RegisterPlugin(configFunc, plugin);
	PluginDevelopmentKit::RegisterFilter<GaussBlur>();
	PluginDevelopmentKit::RegisterFilter<GaussBlurFast>();
	PluginDevelopmentKit::RegisterFilter<TemporalMedian>();
	PluginDevelopmentKit::RegisterFilter<Crop>();
	PluginDevelopmentKit::RegisterFilter<Rec601ToRGB>();
	PluginDevelopmentKit::RegisterFilter<SeparableConvolution>();
	PluginDevelopmentKit::RegisterFilter<ModifyFrame>();
	PluginDevelopmentKit::RegisterFilter<Palette>();
}