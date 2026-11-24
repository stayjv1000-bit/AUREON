# AUREON

AUREON is a JUCE-based piano instrument for Windows and macOS.

## Targets
- VST3 instrument for Windows and macOS
- Standalone application for Windows and macOS
- MIDI input through the host in plugin mode
- Audio/MIDI device selection through JUCE in Standalone mode
- SoundFont playback powered by the embedded FluidSynth engine

## Piano sound
The development build embeds GeneralUser GS v2.0.3 and selects its General MIDI Acoustic Grand Piano preset by default. The SoundFont and its license are downloaded from a pinned upstream revision during CMake configuration, verified by SHA-256, and embedded into the plugin/application. On first launch, AUREON writes the SoundFont and license into the user's application data folder so FluidSynth can load it without requiring a separate synthesizer installation.

GeneralUser GS is by S. Christian Collins. Its license is included with the generated application. The author notes that some contained samples originate from other freely available SoundFonts and that their full provenance cannot be independently guaranteed; review the included license before redistributing AUREON commercially.

## Controls
- Master: output level
- Brightness: MIDI CC 74 sent to all channels
- Reverb: MIDI CC 91 send level
- MIDI note on/off, controller messages, program changes, pitch bend and sustain pedal are forwarded to FluidSynth

## Build prerequisites
- CMake 3.22+
- C++17 compiler
- Git
- Network access during CMake configuration to retrieve JUCE, FluidSynth and the pinned SoundFont/license assets
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

## Development status
The FluidSynth/SoundFont integration is now in the development branch and must pass Windows and macOS CI builds and real MIDI/audio testing in REAPER before it is considered release-ready. A successful compile alone does not prove correct piano playback or installer packaging.

## Third-party licenses
- JUCE: see the JUCE project license and applicable commercial/open-source terms.
- FluidSynth: GNU LGPL; see the FluidSynth source distribution.
- GeneralUser GS v2.0.3: see the embedded `GeneralUser_GS_LICENSE.txt` file.
