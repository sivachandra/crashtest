//===-- Implementation of invokeFunction ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "InvokeFunction.h"

#include <setjmp.h>
#include <signal.h>

#include <unordered_map>

namespace crashtest {

class Caller {
  using SigHandlerType = void(int);
  std::unordered_map<int, SigHandlerType *> saved_handlers;

  static jmp_buf jump_buffer;
  static CrashDescriptor descriptor;

  static void signalHandler(int sig) {
    descriptor.crashed = true;
    descriptor.sig = sig;
    longjmp(jump_buffer, -1);
  }

public:
  Caller() {
    constexpr int all_signals[] = {SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};
    for (int sig : all_signals) {
      saved_handlers[sig] = signal(sig, &signalHandler);
    }
  }

  ~Caller() {
    for (auto pair : saved_handlers) {
      signal(pair.first, pair.second);
    }
  }

  void call(Callable func, CrashDescriptor &desc) {
    descriptor.crashed = false;
    int retval = -1;
    if (setjmp(jump_buffer) == 0)
      retval = func();
    if (descriptor.crashed) {
      desc.crashed = true;
      desc.sig = descriptor.sig;
    } else {
      desc.crashed = false;
      desc.return_value = retval;
    }
  }
};

jmp_buf Caller::jump_buffer;
CrashDescriptor Caller::descriptor;

void invokeFunction(Callable func, CrashDescriptor &desc) {
  Caller caller;
  caller.call(func, desc);
}

} // namespace crashtest
