cmake_minimum_required (VERSION 3.10)

project (Eratosfene_sieve VERSION 1.0)

find_package(OpenMP REQUIRED)

if(NOT WIN32)
    set(WIN32 0)
endif()

# compiler options
if(MSVC)
    add_compile_options("/W4" "$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O2>")
endif()

# required standart
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_subdirectory(sources)
add_subdirectory(samples)