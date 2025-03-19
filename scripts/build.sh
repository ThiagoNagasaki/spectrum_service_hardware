#!/bin/bash

echo "Building: $1 $2 $3"

set -e

if [ $# -lt 2 ]; then
    echo "Please, run # build.sh <TARGET> <BUILD_TYPE> <BUILD_DIR>"
    echo "  TARGET     - Target name to build"
    echo "  BUILD_TYPE - Type of build. E.g.: Debug/Release"
    echo "  BUILD_DIR  - Directory to build target, default = build"
    exit 1
fi

if [ -z "$3" ]; then
    BUILD_DIR="build"
else
    BUILD_DIR=${3}
fi

echo "Build dir: "${BUILD_DIR}

if [ $2 == "Release" ] || [ $2 == "Debug" ]; then
	cmake -B${BUILD_DIR} -H. -DCMAKE_BUILD_TYPE=$2
else
	cmake -B${BUILD_DIR} -H. -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build ${BUILD_DIR} --target $1
