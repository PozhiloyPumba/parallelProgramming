set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} ${OpenMP_CXX_FLAGS}")

set(EXECS_MPI 1a)
set(EXECS_OMP etalon 2b 3c)

find_package(Python3)
if (Python3_FOUND)
    execute_process(
        COMMAND pip show matplotlib
        RESULT_VARIABLE EXIT_CODE
        OUTPUT_QUIET
    )

    if (${EXIT_CODE} EQUAL 0)
        add_custom_target(run_execs_and_show_graphics
            COMMENT "Run and show graphics"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/perf.py
        )
    else()
        add_custom_target(run_execs_and_show_graphics
            COMMENT "You cannot run this target because of you don\'t have matplotlib"
            COMMAND pip show matplotlib
        )
    endif()
endif()

add_custom_target(run_all)

foreach(EXEC IN LISTS EXECS_OMP)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE OpenMP::OpenMP_CXX)
    add_custom_target(run_${EXEC}
        COMMENT "---------- run ${EXEC} ----------"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}
    )

    add_dependencies(run_execs_and_show_graphics ${EXEC})
    add_dependencies(run_${EXEC} ${EXEC})
    add_dependencies(run_all run_${EXEC})
endforeach()

foreach(EXEC IN LISTS EXECS_MPI)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE MPI::MPI_CXX)
    add_custom_target(run_${EXEC}
        COMMENT "---------- run ${EXEC} ----------"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND mpiexec -np 4 ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}
    )
    add_dependencies(run_execs_and_show_graphics ${EXEC})
    add_dependencies(run_${EXEC} ${EXEC})
    add_dependencies(run_all run_${EXEC})
endforeach()
