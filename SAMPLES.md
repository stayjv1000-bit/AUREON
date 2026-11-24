# Piano SoundFont status

The development branch now integrates FluidSynth and embeds GeneralUser GS v2.0.3 during CMake configuration.

- Default preset: General MIDI bank 0, program 0 (Acoustic Grand Piano)
- MIDI: note on/off, sustain and other CC messages, program changes, pitch bend
- Controls: master level, brightness (CC 74), reverb send (CC 91)
- The SoundFont is pinned to a specific upstream revision and SHA-256 verified before it is embedded.
- The SoundFont's license is embedded alongside it and copied to the user's AUREON application-data folder on first load.

Attribution: GeneralUser GS by S. Christian Collins — https://www.schristiancollins.com/generaluser.php

Important licensing note: the author permits software use and redistribution under the included license, but notes that the complete provenance of some samples cannot be independently guaranteed. The license must remain available with distributed copies.

This integration is not considered release-ready until Windows and macOS CI builds pass and the resulting plugin is tested with real MIDI input and audible output in REAPER.
