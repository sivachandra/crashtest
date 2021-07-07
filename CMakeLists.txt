project(crashtest)

add_library(
  InvokeFunction
  InvokeFunction.cpp
  InvokeFunction.h
)

add_executable(
  test
  test.cpp
)

target_link_libraries(test InvokeFunction)
