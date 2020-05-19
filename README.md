# vsFilterScript
C++ wrapper for VSAPI to help you sketch your filter in the fastest possible way.


## Compilation

### Linux

```
$ meson build
$ ninja -C build
```

### Manual

```
g++ -shared -std=c++20 -lstdc++ -static -Ofast -Wno-subobject-linkage -o Filter.dll Examples/EntryPoint.cxx vapoursynth.lib
```
