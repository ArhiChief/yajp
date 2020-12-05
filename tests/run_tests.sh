#!/bin/bash

SOURCE_DIR=$1
BUILD_DIR=$2

echo ${BUILD_DIR}
echo ${SOURCE_DIR}

ctest --build-and-test ${SOURCE_DIR} ${BUILD_DIR} --build-generator "Unix Makefiles" --test-command ctest