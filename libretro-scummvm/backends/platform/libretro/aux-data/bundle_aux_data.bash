#!/bin/bash

#######################################
# Variables                           #
#######################################

SCRIPT_DIR=$(dirname $(which $0))

BUNDLE_DIR="scummvm"
BUNDLE_EXTRA_DIR="${BUNDLE_DIR}/extra"
BUNDLE_THEME_DIR="${BUNDLE_DIR}/theme"
BUNDLE_ZIP_FILE="${BUNDLE_DIR}.zip"

MODERN_THEME_FILE="$(readlink -f "${SCRIPT_DIR}/../../../../gui/themes/scummmodern.zip")"
REMASTERED_THEME_FILE="$(readlink -f "${SCRIPT_DIR}/../../../../gui/themes/scummremastered.zip")"
SOUNDFONT_FILE="${SCRIPT_DIR}/soundfont/Roland_SC-55.sf2"

ENGINE_DATA_DIR="$(readlink -f "${SCRIPT_DIR}/../../../../dists/engine-data")"
ENGINE_DATA_LIST_FILE="${SCRIPT_DIR}/engine_data_list.txt"
declare -a ENGINE_DATA_LIST

#######################################
# Read Engine Data List               #
#######################################

FILE_INDEX=0
while read LINE
do
	
	ENGINE_DATA_LIST[FILE_INDEX]="$LINE"
	((FILE_INDEX++))
	
done < "$ENGINE_DATA_LIST_FILE"

#######################################
# Generate Auxiliary Data Bundle      #
#######################################

cd "$SCRIPT_DIR"

# Remove any existing bundle archive
rm -rf "$BUNDLE_ZIP_FILE"

# Make temporary directories
mkdir -p "$BUNDLE_EXTRA_DIR"
mkdir -p "$BUNDLE_THEME_DIR"

# Copy theme files
cp -v "$MODERN_THEME_FILE" "$BUNDLE_THEME_DIR"
cp -v "$REMASTERED_THEME_FILE" "$BUNDLE_THEME_DIR"

# Copy soundfont file
cp -v "$SOUNDFONT_FILE" "$BUNDLE_EXTRA_DIR"

# Copy engine data files
for FILE_INDEX in $(seq 0 $((${#ENGINE_DATA_LIST[@]} - 1)))
do
	cp -v "${ENGINE_DATA_DIR}/${ENGINE_DATA_LIST[FILE_INDEX]}" "$BUNDLE_EXTRA_DIR"
done

# Create archive
zip -r "$BUNDLE_ZIP_FILE" "$BUNDLE_DIR"

# Remove temporary directories
rm -r "$BUNDLE_DIR"
