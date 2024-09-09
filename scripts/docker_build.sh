#!/bin/bash


# Read projects from JSON file
json_file=$1
upload_folder=$2
bin_folder=$3

projects=$(cat "$json_file" | jq -c '.[]')
git config --global --add safe.directory /__w/Rafael-IoT-SDK-Internal/Rafael-IoT-SDK-Internal

# Build projects
for project in $projects; do
  name=$(echo $project | jq -r '.name')
  example_name=$(echo $project | jq -r '.example_name')
  config_file=$(echo $project | jq -r '.config_file')
  echo "Building $name with example_name=$example_name and config_file=$config_file"

  CONFIG_FILE="examples/$example_name/$config_file.config"
  BUILD_DIR="${example_name}_${config_file}_build"
  UPLOAD_DIR="$upload_folder/$bin_folder/${example_name}_${config_file}/"

  rm -rf $BUILD_DIR && mkdir -p $BUILD_DIR
  
  cmake -DCUSTOM_CONFIG_DIR="$CONFIG_FILE" -S . -B $BUILD_DIR -G Ninja
  if [ $? -ne 0 ]; then
    echo "CMake configuration failed for $name"
    exit 1
  fi

  cmake --build $BUILD_DIR -j16
  if [ $? -ne 0 ]; then
    echo "Build failed for $name"
    exit 1
  fi

  echo "Build directory: $BUILD_DIR"
  echo "Upload directory: $UPLOAD_DIR"

  mkdir -p "$UPLOAD_DIR"
  
  if ls "$BUILD_DIR"/*.bin 1> /dev/null 2>&1; then
    cp -R "$BUILD_DIR"/*.bin "$UPLOAD_DIR"
    if [ $? -ne 0 ]; then
      echo "Failed to copy .bin files for $name"
      exit 1
    fi
  else
    echo "No .bin files found in $BUILD_DIR"
  fi
  
  rm -rf "$BUILD_DIR"
done
