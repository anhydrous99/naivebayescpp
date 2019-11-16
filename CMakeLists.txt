cmake_minimum_required(VERSION 3.15)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For brew installed openssl
if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif ()

set(JSON_BuildTests OFF CACHE BOOL "Build nlohmann's json tests" FORCE)
add_subdirectory(extern/eigen-git-mirror)
add_subdirectory(extern/curl)
add_subdirectory(extern/tinyxml2)
add_subdirectory(extern/cxxopts)

add_executable(naivebayescpp
        src/main.cpp
        src/Parser.cpp
        src/Parser.h
        src/WordMatrix.cpp
        src/WordMatrix.h
        src/Item.h
        src/utils.h
        src/utils.cpp src/WebHandler.cpp src/WebHandler.h)
target_link_libraries(naivebayescpp stdc++fs eigen libcurl cxxopts tinyxml2)
configure_file(${CMAKE_SOURCE_DIR}/stop_words.txt ${CMAKE_BINARY_DIR}/stop_words.txt)
