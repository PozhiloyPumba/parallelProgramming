cmake_minimum_required(VERSION 3.10)

project(Sorting LANGUAGES CXX)

find_package(OpenMP REQUIRED)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

include_directories(include)

add_subdirectory(samples)
