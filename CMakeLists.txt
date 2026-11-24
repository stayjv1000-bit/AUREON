cmake_minimum_required(VERSION 3.22)
project(AUREON VERSION 0.2.0)

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.6
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Build FluidSynth into the plugin so users do not need a separate installation.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(enable-floats ON CACHE BOOL "" FORCE)
set(enable-alsa OFF CACHE BOOL "" FORCE)
set(enable-aufile OFF CACHE BOOL "" FORCE)
set(enable-dbus OFF CACHE BOOL "" FORCE)
set(enable-jack OFF CACHE BOOL "" FORCE)
set(enable-libsndfile OFF CACHE BOOL "" FORCE)
set(enable-midishare OFF CACHE BOOL "" FORCE)
set(enable-network OFF CACHE BOOL "" FORCE)
set(enable-oss OFF CACHE BOOL "" FORCE)
set(enable-portaudio OFF CACHE BOOL "" FORCE)
set(enable-pulseaudio OFF CACHE BOOL "" FORCE)
set(enable-pipewire OFF CACHE BOOL "" FORCE)
set(enable-readline OFF CACHE BOOL "" FORCE)
set(enable-sdl3 OFF CACHE BOOL "" FORCE)
set(enable-openmp OFF CACHE BOOL "" FORCE)
set(enable-native-dls OFF CACHE BOOL "" FORCE)
set(enable-signalsmith OFF CACHE BOOL "" FORCE)
set(enable-threads ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  FluidSynth
  GIT_REPOSITORY https://github.com/FluidSynth/fluidsynth.git
  GIT_TAG v2.6.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(FluidSynth)

# GeneralUser GS is distributed under its own license. Pin the exact upstream
# revision and verify the download before embedding it in the plugin.
set(AUREON_ASSET_DIR "${CMAKE_BINARY_DIR}/aureon-assets")
file(MAKE_DIRECTORY "${AUREON_ASSET_DIR}")

set(AUREON_SF2 "${AUREON_ASSET_DIR}/GeneralUser_GS.sf2")
set(AUREON_SF2_LICENSE "${AUREON_ASSET_DIR}/GeneralUser_GS_LICENSE.txt")
file(DOWNLOAD
  "https://raw.githubusercontent.com/mrbumpy409/GeneralUser-GS/684543d5e5efaef08d02be50dcda8d552478fa60/GeneralUser-GS.sf2"
  "${AUREON_SF2}"
  EXPECTED_HASH SHA256=9575028c7a1f589f5770fccc8cff2734566af40cd26ed836944e9a5152688cfe
  TLS_VERIFY ON
  STATUS AUREON_SF2_DOWNLOAD_STATUS
)
list(GET AUREON_SF2_DOWNLOAD_STATUS 0 AUREON_SF2_DOWNLOAD_CODE)
if(NOT AUREON_SF2_DOWNLOAD_CODE EQUAL 0)
  list(GET AUREON_SF2_DOWNLOAD_STATUS 1 AUREON_SF2_DOWNLOAD_MESSAGE)
  message(FATAL_ERROR "Could not download the AUREON piano SoundFont: ${AUREON_SF2_DOWNLOAD_MESSAGE}")
endif()

file(DOWNLOAD
  "https://raw.githubusercontent.com/mrbumpy409/GeneralUser-GS/684543d5e5efaef08d02be50dcda8d552478fa60/documentation/LICENSE.txt"
  "${AUREON_SF2_LICENSE}"
  EXPECTED_HASH SHA256=7b32efefdf95ce38a043799f0659853ddc00fbaa14d8c50f0aca16b9b8b405be
  TLS_VERIFY ON
  STATUS AUREON_LICENSE_DOWNLOAD_STATUS
)
list(GET AUREON_LICENSE_DOWNLOAD_STATUS 0 AUREON_LICENSE_DOWNLOAD_CODE)
if(NOT AUREON_LICENSE_DOWNLOAD_CODE EQUAL 0)
  list(GET AUREON_LICENSE_DOWNLOAD_STATUS 1 AUREON_LICENSE_DOWNLOAD_MESSAGE)
  message(FATAL_ERROR "Could not download the SoundFont license: ${AUREON_LICENSE_DOWNLOAD_MESSAGE}")
endif()

juce_add_binary_data(AUREON_SOUNDFONT
  SOURCES
    "${AUREON_SF2}"
    "${AUREON_SF2_LICENSE}"
)

juce_add_plugin(AUREON
  COMPANY_NAME "AUREON Audio"
  BUNDLE_ID "com.aureonaudio.instrument"
  PLUGIN_MANUFACTURER_CODE Auro
  PLUGIN_CODE Aur1
  FORMATS VST3 Standalone
  PRODUCT_NAME "AUREON"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(AUREON)

target_sources(AUREON PRIVATE
  Source/PluginProcessor.cpp
  Source/PluginEditor.cpp
)

target_compile_features(AUREON PUBLIC cxx_std_17)

target_compile_definitions(AUREON PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AUREON PRIVATE
  AUREON_SOUNDFONT
  libfluidsynth
  juce::juce_audio_utils
  juce::juce_audio_processors
  juce::juce_dsp
  juce::juce_gui_extra
  PUBLIC
  juce::juce_recommended_config_flags
  juce::juce_recommended_lto_flags
  juce::juce_recommended_warning_flags
)
