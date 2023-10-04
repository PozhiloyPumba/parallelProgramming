set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} ${OpenMP_CXX_FLAGS}")

set(EXECS sort)

foreach(EXEC IN LISTS EXECS)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE OpenMP::OpenMP_CXX)
endforeach()
