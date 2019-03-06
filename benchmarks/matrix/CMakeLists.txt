
add_executable(matrixTest matrixTest.cpp)
set_target_properties(matrixTest
  PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 17
  PREFIX ""
)

target_link_libraries(matrixTest
  benchmark::benchmark_main
  ${CMAKE_THREAD_LIBS_INIT}
)

