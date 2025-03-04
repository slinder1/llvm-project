#!/bin/bash

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export DIEXPR_BENCH_ROOT="$ROOT"

dogfood() {
local CFG="$1"; shift
rm -rf $ROOT/tmp
mkdir $ROOT/tmp
rm -rf $ROOT/build-dogfood
export CC=$ROOT/../build/bin/clang
export CXX=$ROOT/../build/bin/clang++
cmake -S $ROOT/../llvm -B $ROOT/build-dogfood -G Ninja -DCMAKE_BUILD_TYPE=$CFG -DLLVM_ENABLE_PROJECTS='llvm;clang;lld' -DLLVM_TARGETS_TO_BUILD=all -DLLVM_CCACHE_BUILD=Off -DLLVM_OPTIMIZED_TABLEGEN=Off || exit 1
cmake --build $ROOT/build-dogfood --config $CFG || exit 1
rm -rf $ROOT/$CFG
mv $ROOT/tmp $ROOT/$CFG
rm -rf $ROOT/build-dogfood
}

dogfood RelWithDebInfo
#dogfood Debug
