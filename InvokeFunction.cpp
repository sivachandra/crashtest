//===-- Implementation of invokeFunction ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "InvokeFunction.h"

#include <fenv.h>
#include <setjmp.h>
#include <signal.h>

#include <unordered_map>

namespace crashtest {

#if defined(__WIN32) || defined(_WIN64)
  static constexpr int all_signals[1] = {SIGABRT, SIGFPE, SIGSEGV};
#else
  static constexpr int all_signals[6] = {SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};
#endif

class Caller {
  using SigHandlerType = void(int);
  std::unordered_map<int, SigHandlerType *> saved_handlers;

  static thread_local jmp_buf jump_buffer;
  static thread_local CrashDescriptor descriptor;

  static void signalHandler(int sig) {
    descriptor.crashed = true;
    descriptor.sig = sig;
#if defined(__WIN32) || defined(_WIN64)
    longjmp(jump_buffer, -1);
#else
    siglongjmp(jump_buffer, -1);
#endif
  }

public:
  Caller() {
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
#if defined(__WIN32) || defined(_WIN64)
    if (setjmp(jump_buffer) == 0)
      retval = func();
#else
    int enabled_excepts = fegetexcept();
    if (sigsetjmp(jump_buffer, 1) == 0) {
      retval = func();
    }
    feenableexcept(enabled_excepts);
#endif
    if (descriptor.crashed) {
      desc.crashed = true;
      desc.sig = descriptor.sig;
    } else {
      desc.crashed = false;
      desc.return_value = retval;
    }
  }
};

thread_local jmp_buf Caller::jump_buffer;
thread_local CrashDescriptor Caller::descriptor;

void invokeFunction(Callable func, CrashDescriptor &desc) {
  Caller caller;
  caller.call(func, desc);
}

} // namespace crashtest
