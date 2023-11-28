set(EXECS seq par_task par_for)

foreach(EXEC IN LISTS EXECS)
    add_executable(${EXEC} main.cpp)
    target_link_libraries(${EXEC} PRIVATE sieve_${EXEC})
    set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)

    add_custom_target(run_perf_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "-------------- Running ${EXEC} App --------------"
        COMMAND echo 100000000 | ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}$<IF:${WIN32},.exe,>
    )
    add_dependencies(run_perf_${EXEC} ${EXEC})

    add_custom_target(run_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "-------------- Running ${EXEC} App --------------"
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}$<IF:${WIN32},.exe,>
    )
    add_dependencies(run_${EXEC} ${EXEC})
endforeach()