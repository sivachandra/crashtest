//===-- A utility function to invoke functions which can crash.--*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef __INVOKE_FUNCTION_H__
#define __INVOKE_FUNCTION_H__

namespace crashtest {

using Callable = int(void);

struct CrashDescriptor {
  bool crashed; // Flag which indicates if the function crashed.
  int sig; // The signal with which the function crashed, iff it crashed.
  int return_value; // The value returned by the function, iff it did not crash.
};

void invokeFunction(Callable func, CrashDescriptor &desc);

} // namespace crashtest

#endif // __INVOKE_FUNCTION_H__
