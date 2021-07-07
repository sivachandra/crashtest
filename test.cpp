#include "InvokeFunction.h"
#include "signal.h"

#include <iostream>

int main() {
  exp::CrashDescriptor cd;
  exp::invokeFunction([]() { raise(SIGFPE); return 0; }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "         cd.sig: " << cd.sig << std::endl;
  std::cout << "         SIGFPE: " << SIGFPE << std::endl;

  exp::invokeFunction([]() { return 123; }, cd);
  std::cout << "     cd.crashed: " << cd.crashed << std::endl;
  std::cout << "cd.return_value: " << cd.return_value << std::endl;

  return 0;
}
