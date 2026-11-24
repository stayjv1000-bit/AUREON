# AUREON

AUREON is an early-stage sampled-piano instrument project built with C++ and JUCE.

## Targets
- VST3 instrument for Windows and macOS
- Standalone application for Windows and macOS
- MIDI input through the host in plugin mode
- Audio/MIDI device selection through JUCE in Standalone mode
- Sample-based piano engine designed for velocity-layered WAV content

## Current development status
This repository contains the initial application/plugin framework. It is **not yet a finished piano instrument**. A realistic piano requires a properly recorded and licensed sample library (multiple velocity layers, note ranges, release behavior and, ideally, round robins). No Keyzone Classic samples or other unlicensed content are included.

The initial engine looks for WAV files in a `Samples` directory next to the application/plugin resources. The sample mapping and playback engine will be expanded as the library format is finalized.

## Build prerequisites
- CMake 3.22+
- C++17 compiler
- Git
- JUCE (downloaded automatically by CMake FetchContent)
- Windows: Visual Studio 2022 with C++ desktop workload
- macOS: Xcode command line tools

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The build creates VST3 and Standalone targets where supported by JUCE.

## REAPER
In REAPER, select AUREON as an instrument on a track and route a MIDI keyboard to that track. The audio device and driver are controlled by REAPER, not by the plugin. In Standalone mode, AUREON uses JUCE's audio/MIDI device setup.

## License
All rights reserved unless a later LICENSE file states otherwise. Do not redistribute this code or any future sample content without checking its license.
