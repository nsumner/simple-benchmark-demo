
add_executable(sequenceTest sequenceTest.cpp)
set_target_properties(sequenceTest
  PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 17
  PREFIX ""
)

target_link_libraries(sequenceTest
  benchmark::benchmark_main
  ${CMAKE_THREAD_LIBS_INIT}
)

