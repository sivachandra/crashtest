#include "InvokeFunction.h"

#include <fenv.h>
#include <iostream>
#include <signal.h>

int mydiv(double x, double y) {
  return x / y;
}

int main() {
  feenableexcept(FE_ALL_EXCEPT);

  crashtest::CrashDescriptor cd;
  crashtest::invokeFunction([]() { raise(SIGFPE); return 0; }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "         cd.sig: " << cd.sig << std::endl;
  std::cout << "         SIGFPE: " << SIGFPE << std::endl;

  crashtest::invokeFunction([]() { raise(SIGSEGV); return 0; }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "         cd.sig: " << cd.sig << std::endl;
  std::cout << "         SIGSEGV: " << SIGSEGV << std::endl;

  crashtest::invokeFunction([]() { return mydiv(1, 0); }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "         cd.sig: " << cd.sig << std::endl;
  std::cout << "         SIGFPE: " << SIGFPE << std::endl;

  crashtest::invokeFunction([]() { return mydiv(0, 0); }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "         cd.sig: " << cd.sig << std::endl;
  std::cout << "         SIGFPE: " << SIGFPE << std::endl;

  crashtest::invokeFunction([]() { return 123; }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "cd.return_value: " << cd.return_value << std::endl;

  return 0;
}
