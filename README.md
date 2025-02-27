# essentia-vst

This is a repository to show how to build VST plugins linking [JUCE](https://juce.com/) and essentia library via [ProJucer](https://docs.juce.com/master/tutorial_new_projucer_project.html). It contains a VST template as example and also stores VSTs developed here at MTG.

## Table of Content

## Installation

1. Clone our repository in the work directory

```bash
git clone https://github.com/MTG/essentia-vst
```

2. Clone essentia repository in the work director and install dependencies

   1. Check here for more details: <https://essentia.upf.edu/installing.html#installing-dependencies-on-macos>

```bash
git clone https://github.com/MTG/essentia
```

3. Define `ESSENTIA_DIR` in `.env` (if needed)
4. Compile essentia in a static library (libessentia)

```bash
bash scripts/build-essentia-osx.sh
```

5. Download JUCE and Projucer package from <https://juce.com/download/>
   1. The default `JUCE_DIR` in MacOS is `/Users/$USER/dev/JUCE`
6. Redefine the `JUCE_DIR` in `.env` file (if needed)
7. Run the *vst-template* project with Projucer to test all is okay
8. Copy the *vst-template* folder and rename the folder and the Projucer project.
9. Fix the issues after replacing classnames?

## Usage

1. Install Essentia dependencies on your system.
2. Compile Essentia library running `./scripts/build-essentia-osx.sh`
3. Run the *vst-template* project to ensure all is working fine.
4. Copy the *vst-template* folder and rename files and classes.
