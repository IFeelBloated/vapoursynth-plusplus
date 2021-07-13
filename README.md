# Under Construction
Vaporsynth C++ API

## Compilation
requires a C++20 compiler (GCC11 or the latest version of MSVC)

### Linux

```
$ meson build
$ ninja -C build
```

### Manual

```
g++ -shared -std=c++2b -lstdc++ -static -O3 -flto -march=native -finline-limit=1000000000000000000000000000 -funroll-all-loops -funsafe-loop-optimizations -o Filter.dll Examples/EntryPoint.cxx vapoursynth.lib
```
