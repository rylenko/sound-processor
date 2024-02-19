<h1 align="center">Welcome to Sound-Processor 🌿</h1>

Simple sound processor, which accepts only PCM mono 44100Hz with 16 bit samples, but it is very extandable!

<h1 align="center">Installation</h1>

**1.** Clone this repository how you like it.

**2.** Generate converters shared object (Default converters in **converter.cpp**:
```
$ ./create-so <path>
```

**3.** Create build directory:
```
$ mkdir build && cd build
$ cmake ..
```

**4.** Get usage information:
```
$ cmake --build .
$ ./sound-processor -h
```

