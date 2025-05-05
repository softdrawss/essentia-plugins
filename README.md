# Essentia Plugins

This repository contains a collection of VST plugins built using the Essentia library. The plugins are designed to be used in digital audio workstations (DAWs) and provide various audio analysis, audio effects options.
The plugins are built using the JUCE framework, which provides a powerful and flexible platform for developing audio applications.

In addition, we include plugin templates for building VST3 plugins using the Essentia library using CMake and Projucer.

## Prerequisites
- CMake
- JUCE
- Essentia
- Python

# Worflow

The workflow for building the plugins is as follows:
1. Clone the repository with the submodules
```bash
git clone --recurse-submodules https://github.com/MTG/essentia-plugins.git
```
   If already cloned without --recurse-submodule
```bash
cd essentia-plgins
git submodule update --init --recursive
```
2. Install the dependencies
   - JUCE
   - Essentia (as a static library)
   - Google Test
3. Build the plugins

# Building essentia

Details on how to build Essentia can be found in the [Essentia documentation](https://essentia.upf.edu/documentation/). The following instructions are a summary of the official documentation.

There are two ways to build Essentia: using CMake or waf build system. As of now essentia has no official CMake support.

The plugins were tested using the official Essentia build system (waf) as it contains all its dependencies in the static library. The CMake build system is still under development and may not work as expected when exporting the plugins to a different computer.

#### Essentia with CMake
There are some attempts to migrate Essentia to CMake: 
https://github.com/wo80/essentia/tree/cmake
If you want to use the CMake build system, you can add a new `ExternalProject_Add` to the exteral/CMakeLists.txt with the repository that has the CMake support. 



## Build Dependencies

JUCE and Google test (Cmake)
```bash
mkdir build_external && cd build_external
cmake ../external && cmake --build . -- -j$(nproc)
```
For essentia:
```bash
cd external/essentia
python3 waf configure --build-static --static-dependencies --lightweight= --out=${PWD}/build --prefix=${PWD}../../../build_external/install
python3 waf build --progress
python3 waf install --progress
```

## Dependecies with bash scripts

For all cmake dependencies:
```bash
bash scripts/build-external.sh
```
For essentia as static library using wscript:
```bash
bash scripts/build-essentia-osx.sh
```

# Building the plugins
The plugins are built using CMake. The build process is as follows:

All plgins:
```bash
PATH_INSTALL_EXTERNALS="${PWD}/build_external/install"
PATH_INSTALL_EIGEN3="path/to/include/eigen3"
mkdir build && cd build
cmake .. \
   -DCMAKE_PREFIX_PATH="${PATH_INSTALL_EXTERNALS};${PATH_INSTALL_EIGEN3}" \
   -DCOPY_PLUGIN_AFTER_BUILD:STRING=ON 
cmake --build .
```


# Plugin templates
There are two plugin templates available in the `templates` folder:
- `cmake/plugin_template`: A template for building a VST3 plugin using CMake.
- `projucer/plugin_template`: A template for building a VST3 plugin using Projucer.

Just copy the folder and rename it to your plugin name. Then, open the CMakeLists.txt or Projucer file and modify the plugin name and other settings as needed.

# Plugins
The currently available plugins are:

| Plugin Name | Description | Build System | Dependencies |
| ----------- | ----------- | ---------- | ------------ |
| [RMS](plugins/RMS) | Computes the RMS of the input signal. | CMake | Essentia |
| [Energy](plugins/Energy) | Computes the energy of the input signal. | CMake | Essentia |
| [RMS_projucer](plugins/RMS_projucer) | Computes the RMS of the input signal. | Projucer | Essentia |
| [Audio2Midi](plugins/Audio2Midi) | Converts audio to MIDI. | Projucer | Essentia |

