//===- DebugInfoCommon.h - Shared Debug Info Types --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines small types common to multiple DebugInfo translation units
// while transitioning to DebugInfoExprs.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_IR_DEBUGINFOCOMMON_H
#define LLVM_IR_DEBUGINFOCOMMON_H

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/Compiler.h>

#include <cstdint>

namespace llvm {

enum class SignedOrUnsignedConstant { SignedConstant, UnsignedConstant };

void appendOffsetImpl(SmallVectorImpl<uint64_t> &Ops, int64_t Offset);

} // end namespace llvm

#endif // LLVM_IR_DEBUGINFOCOMMON_H
