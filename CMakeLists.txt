cmake_minimum_required(VERSION 3.15)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE BOOL "Build nlohmann's json tests" FORCE)
add_subdirectory(extern/eigen-git-mirror)
add_subdirectory(extern/curl)
add_subdirectory(extern/json)
add_subdirectory(extern/cxxopts)

add_executable(naivebayescpp src/main.cpp src/Parser.cpp src/Parser.h src/WordMatrix.cpp src/WordMatrix.h src/Item.h)
target_link_libraries(naivebayescpp stdc++fs eigen libcurl nlohmann_json cxxopts)
configure_file(${CMAKE_SOURCE_DIR}/stop_words.txt ${CMAKE_BINARY_DIR}/stop_words.txt)
