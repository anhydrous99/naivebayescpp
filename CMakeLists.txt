cmake_minimum_required(VERSION 3.15)

include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.225.tar.gz"
        SHA1 "4b018ec7b673817475c1fb9f0bd4dfb7c5e7f343"
)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

hunter_add_package(Eigen)
hunter_add_package(Boost COMPONENTS filesystem)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED filesystem)

add_executable(naivebayescpp src/main.cpp src/Parser.cpp src/Parser.h src/WordMatrix.cpp src/WordMatrix.h src/Item.h)
target_link_libraries(naivebayescpp Eigen3::Eigen Boost::filesystem)
configure_file(${CMAKE_SOURCE_DIR}/stop_words.txt ${CMAKE_BINARY_DIR}/stop_words.txt)
