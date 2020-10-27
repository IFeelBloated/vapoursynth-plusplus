# vsFilterScript
C++ wrapper for VSAPI to help you sketch your filter in the fastest possible way.

## Progress

### Up To Date
Range
Plane
Owner
Metadata
Frame
VideoFrame
AudioFrame
Node
VideoNode
AudioNode

## Compilation

### Linux

```
$ meson build
$ ninja -C build
```

### Manual

```
g++ -shared -std=c++20 -lstdc++ -static -Ofast -o Filter.dll Examples/EntryPoint.cxx vapoursynth.lib
```
