set(SIEVE_SEQ_LIB sieve_seq)
set(SIEVE_PAR_FOR_LIB sieve_par_for)
set(SIEVE_PAR_TASKS_LIB sieve_par_task)

add_library(${SIEVE_SEQ_LIB} sieve.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} ${OpenMP_CXX_FLAGS}")
add_library(${SIEVE_PAR_TASKS_LIB} sieve.cpp)
target_compile_definitions(${SIEVE_PAR_TASKS_LIB} PRIVATE PARALLEL_TASKS)
target_link_libraries(${SIEVE_PAR_TASKS_LIB} PRIVATE OpenMP::OpenMP_CXX)

add_library(${SIEVE_PAR_FOR_LIB} sieve.cpp)
target_compile_definitions(${SIEVE_PAR_FOR_LIB} PRIVATE PARALLEL_FOR_BASE)
target_link_libraries(${SIEVE_PAR_FOR_LIB} PRIVATE OpenMP::OpenMP_CXX)

find_package(TBB)

if (TBB_FOUND)
    target_link_libraries(${SIEVE_SEQ_LIB} PRIVATE TBB::tbb)
    target_compile_definitions(${SIEVE_SEQ_LIB} PRIVATE TBB)
    target_link_libraries(${SIEVE_PAR_FOR_LIB} PRIVATE TBB::tbb)
    target_compile_definitions(${SIEVE_PAR_FOR_LIB} PRIVATE TBB)
    target_link_libraries(${SIEVE_PAR_TASKS_LIB} PRIVATE TBB::tbb)
    target_compile_definitions(${SIEVE_PAR_TASKS_LIB} PRIVATE TBB)
endif()