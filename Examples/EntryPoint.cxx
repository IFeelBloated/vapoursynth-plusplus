#include "../Include/Interface.hxx"
#include "GaussBlur.hxx"
#include "TemporalMedian.hxx"
#include "Crop.hxx"
#include "Rec601ToRGB.hxx"

VS_EXTERNAL_API(auto) VapourSynthPluginInit(VSConfigPlugin configFunc, VSRegisterFunction registerFunc, VSPlugin* plugin) {
	VaporGlobals::Identifier = "com.vsfilterscript.test";
	VaporGlobals::Namespace = "test";
	VaporGlobals::Description = "Test filters for vsFilterScript";
	VaporInterface::RegisterPlugin(configFunc, plugin);
	VaporInterface::RegisterFilter<GaussBlur>(registerFunc, plugin);
	VaporInterface::RegisterFilter<TemporalMedian>(registerFunc, plugin);
	VaporInterface::RegisterFilter<Crop>(registerFunc, plugin);
	VaporInterface::RegisterFilter<Rec601ToRGB>(registerFunc, plugin);
}