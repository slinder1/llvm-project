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

static_assert((dwarf::DW_OP_LLVM_lo >> 4) == (dwarf::DW_OP_hi_user + 1));
static_assert((dwarf::DW_OP_LLVM_hi - dwarf::DW_OP_LLVM_lo) < 0xff);

static constexpr size_t ExprOpCount = 0x1ff + 1;
static constexpr unsigned ExprOpKnownMask = 0x10ff;
static constexpr unsigned ExprOpUnknownMask = ~ExprOpKnownMask;

static constexpr bool isUnknownOp(unsigned Op) {
  return Op & ExprOpUnknownMask;
}

static constexpr unsigned indexOf(unsigned Op) {
  unsigned HighBit = (Op & dwarf::DW_OP_LLVM_lo) >> 4;
  return HighBit | (Op & 0xff);
}

static constexpr std::array<uint8_t, ExprOpCount> getExprSizeMap() {
  std::array<uint8_t, ExprOpCount> Ret = {};
  for (size_t I = 0; I < ExprOpCount; ++I)
    Ret[I] = 1u;
  for (unsigned I = dwarf::DW_OP_breg0; I <= dwarf::DW_OP_breg31; ++I)
    Ret[indexOf(I)] = 2u;
  Ret[indexOf(dwarf::DW_OP_constu)] = 2u;
  Ret[indexOf(dwarf::DW_OP_consts)] = 2u;
  Ret[indexOf(dwarf::DW_OP_deref_size)] = 2u;
  Ret[indexOf(dwarf::DW_OP_plus_uconst)] = 2u;
  Ret[indexOf(dwarf::DW_OP_LLVM_tag_offset)] = 2u;
  Ret[indexOf(dwarf::DW_OP_LLVM_entry_value)] = 2u;
  Ret[indexOf(dwarf::DW_OP_LLVM_arg)] = 2u;
  Ret[indexOf(dwarf::DW_OP_regx)] = 2u;
  Ret[indexOf(dwarf::DW_OP_LLVM_convert)] = 3u;
  Ret[indexOf(dwarf::DW_OP_LLVM_fragment)] = 3u;
  Ret[indexOf(dwarf::DW_OP_LLVM_extract_bits_sext)] = 3u;
  Ret[indexOf(dwarf::DW_OP_LLVM_extract_bits_zext)] = 3u;
  Ret[indexOf(dwarf::DW_OP_bregx)] = 3u;
  return Ret;
}

static constexpr std::array<uint8_t, ExprOpCount> ExprSizeMap =
    getExprSizeMap();

unsigned ExprOperand::getSize() const {
  uint64_t Op = getOp();
  return isUnknownOp(Op) ? 1u : ExprSizeMap[indexOf(Op)];
}

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
