cmake_minimum_required(VERSION 3.10)

project(ExtraTasks LANGUAGES C)

find_package(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_CFLAGS} ${OpenMP_C_FLAGS}")

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(EXECS nested balance)

foreach(EXEC IN LISTS EXECS)
    add_executable(${EXEC} ${EXEC}.c)
    target_link_libraries(${EXEC} PRIVATE OpenMP::OpenMP_C)
endforeach()
