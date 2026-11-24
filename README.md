# AUREON

AUREON is an early-stage piano-instrument project built with C++ and JUCE.

## Planned formats
- VST3 instrument for Windows and macOS
- Standalone application for Windows and macOS
- MIDI input through the host in plugin mode
- Audio/MIDI device selection in Standalone mode

## Current status — prototype
The current code is a **development prototype**, not a finished sampled piano. It has a basic MIDI-triggered test-tone voice, a dark/gold interface, and master, brightness and reverb controls. It does not yet contain recorded piano samples, velocity layers, pedal behavior or a production sample-mapping engine.

A convincing acoustic piano depends primarily on a properly recorded and licensed sample library: broad note coverage, multiple velocity layers, consistent tuning, release behavior and preferably round robins. No Keyzone Classic samples or other unlicensed content are included.

## Build prerequisites
- CMake 3.22+
- C++17 compiler
- Git
- JUCE 8.0.6 (downloaded automatically by CMake FetchContent)
- Windows: Visual Studio 2022 with C++ desktop workload
- macOS: Xcode command line tools

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The GitHub Actions workflow attempts Windows and macOS builds on pushes and pull requests. A successful workflow is required before calling a build verified.

## REAPER
In REAPER, load AUREON as an instrument on a track and route your MIDI keyboard to that track. In plugin mode, the audio device and driver are controlled by REAPER, not by AUREON. Standalone mode will use JUCE's audio/MIDI device setup.

## Sample library
See [Samples/README.txt](Samples/README.txt). Only use recordings you own or are explicitly licensed to use.

## License
All rights reserved unless a later LICENSE file states otherwise. Do not redistribute this code or future sample content without checking its license.
