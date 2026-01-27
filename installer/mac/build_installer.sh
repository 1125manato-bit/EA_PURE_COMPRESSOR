#!/bin/bash

# Ensure we are in the script directory
cd "$(dirname "$0")"

# Define paths
BUILD_DIR="../../build/EA_PURE_COMPRESSOR_artefacts/Release"
VST3_SOURCE="$BUILD_DIR/VST3/EA PURE COMPRESSOR.vst3"
AU_SOURCE="$BUILD_DIR/AU/EA PURE COMPRESSOR.component"
OUTPUT_DIR="Output"

mkdir -p "$OUTPUT_DIR"

if [ ! -d "$VST3_SOURCE" ]; then
    echo "Error: VST3 not found at $VST3_SOURCE. Please build the project first."
    exit 1
fi

if [ ! -d "$AU_SOURCE" ]; then
    echo "Error: AU not found at $AU_SOURCE. Please build the project first."
    exit 1
fi

echo "Building Component Packages..."

# Build VST3 Package
pkgbuild --identifier com.emuaudio.eapurecompressor.vst3 \
         --version 0.0.1 \
         --component "$VST3_SOURCE" \
         --install-location "/Library/Audio/Plug-Ins/VST3" \
         "$OUTPUT_DIR/EA_PURE_COMPRESSOR_VST3.pkg" || exit 1

# Build AU Package
pkgbuild --identifier com.emuaudio.eapurecompressor.au \
         --version 0.0.1 \
         --component "$AU_SOURCE" \
         --install-location "/Library/Audio/Plug-Ins/Components" \
         "$OUTPUT_DIR/EA_PURE_COMPRESSOR_AU.pkg" || exit 1

echo "Creating Distribution Package..."

# Create a simple distribution xml
# We assume the user wants both installed by default
productbuild --synthesize \
             --package "$OUTPUT_DIR/EA_PURE_COMPRESSOR_VST3.pkg" \
             --package "$OUTPUT_DIR/EA_PURE_COMPRESSOR_AU.pkg" \
             "$OUTPUT_DIR/distribution.xml"

# Build final installer
productbuild --distribution "$OUTPUT_DIR/distribution.xml" \
             --package-path "$OUTPUT_DIR" \
             "$OUTPUT_DIR/EA_PURE_COMPRESSOR_Mac_Installer.pkg"

echo "-------------------------------------------------------"
echo "SUCCESS!"
echo "Installer created at: $PWD/$OUTPUT_DIR/EA_PURE_COMPRESSOR_Mac_Installer.pkg"
echo "-------------------------------------------------------"
