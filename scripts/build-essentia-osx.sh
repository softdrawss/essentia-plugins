#!/bin/bash
# Building essentia library on the build directory

# It is supposed you have already essentia dependencies before installed. Python dependencies are not needed for this build.
# Check here for more information: https://essentia.upf.edu/installing.html#installing-dependencies-on-macos

# define the script directory
SCRIPT_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

# Load .env directories
set -a && source .env && set +a

# TODO: reuse .env paths, make all relative to ESSENTIA_DIR
ROOT_DIR="${SCRIPT_DIR}/../"
BUILD_DIR="${ROOT_DIR}${BUILD_DIR}"

# Change to essentia directory to avoid wscript file error
cd $ESSENTIA_DIR

## Build essentia as an static library

# TODO: define Linux and OSX command if needed

# Configure the installation
config_cmd="python3 waf configure \
--build-static \
--lightweight= \
--out=${BUILD_DIR}"
echo $config_cmd
eval $config_cmd

# Run the installation
install_cmd="python3 waf -v && python3 waf install"
echo $install_cmd
eval $install_cmd
