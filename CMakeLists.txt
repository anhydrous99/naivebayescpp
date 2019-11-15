cmake_minimum_required(VERSION 3.15)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/eigen-git-mirror)

add_executable(naivebayescpp src/main.cpp src/Parser.cpp src/Parser.h src/WordMatrix.cpp src/WordMatrix.h src/Item.h)
target_include_directories(naivebayescpp PRIVATE ${EIGEN_INCLUDE_DIR})
configure_file(${CMAKE_SOURCE_DIR}/stop_words.txt ${CMAKE_BINARY_DIR}/stop_words.txt)
