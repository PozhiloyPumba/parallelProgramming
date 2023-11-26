set(CMAKE_CXX_FLAGS "${CMAKE_CXXFLAGS} ${OpenMP_CXX_FLAGS}")

set(EXECS_OMP seq_base_mul parallel_block_mul)

foreach(EXEC IN LISTS EXECS_OMP)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE OpenMP::OpenMP_CXX opencl_simple_interface)
    set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)
endforeach()

set(EXECS_OPENCL opencl_base opencl_cache)

foreach(EXEC IN LISTS EXECS_OPENCL)
    add_executable(${EXEC} ${EXEC}.cpp)
    target_link_libraries(${EXEC} PRIVATE opencl_simple_interface)
    set_target_properties(${EXEC} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)

    if(NVIDIA)
        target_compile_definitions(${EXEC} PRIVATE NVIDIA)
    endif()
endforeach()

add_custom_target(check_correct_all)

foreach(EXEC IN LISTS EXECS_OMP EXECS_OPENCL)
    add_custom_target(run_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "-------------- Running ${EXEC} App --------------"
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}$<IF:${WIN32},.exe,>
    )
    add_dependencies(run_${EXEC} ${EXEC})

    add_executable(${EXEC}_test ${EXEC}.cpp)

    target_compile_definitions(${EXEC}_test PRIVATE TEST) 

    target_link_libraries(${EXEC}_test PRIVATE OpenMP::OpenMP_CXX opencl_simple_interface)
    set_target_properties(${EXEC}_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/$<0:>)

    add_custom_target(
        check_correct_${EXEC}
        COMMAND ${CMAKE_CTEST_COMMAND} -L CORRECT_TESTS_${EXEC}
    )

    add_dependencies(check_correct_all check_correct_${EXEC})

    add_custom_target(runTest1_${EXEC}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}_test$<IF:${WIN32},.exe,> < ${CMAKE_SOURCE_DIR}/tests/test1/src.txt > ans.txt &&
                ${CMAKE_COMMAND} -E compare_files ans.txt ${CMAKE_SOURCE_DIR}/tests/test1/answer.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${EXEC}_test
    )
    add_test(
        NAME Test1_${EXEC}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target runTest1_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(runTest2_${EXEC}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}_test$<IF:${WIN32},.exe,> < ${CMAKE_SOURCE_DIR}/tests/test2/src.txt > ans.txt &&
                ${CMAKE_COMMAND} -E compare_files ans.txt ${CMAKE_SOURCE_DIR}/tests/test2/answer.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${EXEC}_test
    )
    add_test(
        NAME Test2_${EXEC}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target runTest2_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    add_custom_target(runTest3_${EXEC}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC}_test$<IF:${WIN32},.exe,> < ${CMAKE_SOURCE_DIR}/tests/test3/src.txt > ans.txt &&
                ${CMAKE_COMMAND} -E compare_files ans.txt ${CMAKE_SOURCE_DIR}/tests/test3/answer.txt
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${EXEC}_test
    )
    add_test(
        NAME Test3_${EXEC}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target runTest3_${EXEC}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    SET_TESTS_PROPERTIES(Test1_${EXEC} Test2_${EXEC} Test3_${EXEC} PROPERTIES LABELS "CORRECT_TESTS_${EXEC}")

endforeach()