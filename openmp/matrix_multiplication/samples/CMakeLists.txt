set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} ${OpenMP_CXX_FLAGS}")

set(EXECS seq_base_mul seq_transpose_mul parallel_base_mul parallel_transpose_mul parallel_optimize_mul)

foreach(EXEC IN LISTS EXECS)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE OpenMP::OpenMP_CXX)
endforeach()
