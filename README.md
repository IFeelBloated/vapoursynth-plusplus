# vsFilterScript
C++ wrapper for VSAPI to help you sketch your filter in the fastest possible way.

## Progress

### Up To Date

```
Utility
Range
Plane
ResourceManager
Metadata
Frame
VideoFrame
AudioFrame
Node
VideoNode
AudioNode
Function
Map (Partial)
Plugin
```

## Compilation

### Linux

```
$ meson build
$ ninja -C build
```

### Manual

```
g++ -shared -std=c++20 -lstdc++ -static -O3 -flto -o Filter.dll Examples/EntryPoint.cxx vapoursynth.lib
```
