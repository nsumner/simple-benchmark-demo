# NOTE: This requires google/benchmark to be installed first....
project(SimpleBenchmarkingDemo)
cmake_minimum_required(VERSION 3.0)

set(PACKAGE_NAME SimpleBenchmarkingDemo)
set(PACKAGE_VERSION 0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "wsumner@sfu.ca")

if (NOT CMAKE_BUILD_TYPE MATCHES Release)
  message("*****************************************************\nWARNING!!")
  message("\nBuilding performance benchmark in a mode other than Release.\n")
  message("WARNING!!\n*****************************************************")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)

add_subdirectory(benchmarks)

