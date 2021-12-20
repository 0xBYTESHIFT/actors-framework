#!/bin/bash
COMPILER=$1
COMPILER_VER=$2
COMPILER_CXX=$3
COMPILER_C=$4
CMAKE_CXX_STANDARD=$5
workspace=$6
BUILD_TYPE=Release
rm -rf ${workspace}/build
conan profile new default --detect
conan profile update settings.compiler=${COMPILER} default
conan profile update settings.compiler.version=${COMPILER_VER} default
conan profile update env.CXX=${COMPILER_CXX} default
conan profile update env.CC=${COMPILER_C} default
mkdir -p ${workspace}/build
conan install ${workspace}/build/.. -if ${workspace}/build --build=missing -s build_type=Release
cd ${workspace}/build
export CC=${COMPILER_C}
export CXX=${COMPILER_CXX}
cmake -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD} \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DALLOW_EXAMPLES=ON \
    -DALLOW_TESTS=ON \
    -DUSE_SIMPLE_QUEUE=OFF \
    -DUSE_MOODYCAMEL_LOCKFREE_QUEUE=OFF \
    -DUSE_CAF_LOCKFREE_QUEUE=ON \
    -DUSE_TRACY_PROFILER=ON \
    ..
cmake --build . --parallel

