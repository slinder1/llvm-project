//===- DebugInfoCommon.cpp - Shared Debug Info Types ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements DebugInfoCommon types.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/DebugInfoCommon.h"
#include "llvm/BinaryFormat/Dwarf.h"

using namespace llvm;

void llvm::appendOffsetImpl(SmallVectorImpl<uint64_t> &Ops, int64_t Offset) {
  if (Offset > 0) {
    Ops.push_back(dwarf::DW_OP_plus_uconst);
    Ops.push_back(Offset);
  } else if (Offset < 0) {
    Ops.push_back(dwarf::DW_OP_constu);
    // Avoid UB when encountering LLONG_MIN, because in 2's complement
    // abs(LLONG_MIN) is LLONG_MAX+1.
    uint64_t AbsMinusOne = -(Offset + 1);
    Ops.push_back(AbsMinusOne + 1);
    Ops.push_back(dwarf::DW_OP_minus);
  }
}
