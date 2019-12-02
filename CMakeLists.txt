cmake_minimum_required(VERSION 3.15)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For brew installed openssl
if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif ()

set(HTTP_ONLY ON CACHE BOOL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)" FORCE)
set(JSON_BuildTests OFF CACHE BOOL "Build nlohmann's json tests" FORCE)
add_subdirectory(extern/eigen-git-mirror)
add_subdirectory(extern/curl)
add_subdirectory(extern/json)
add_subdirectory(extern/cxxopts)

add_executable(naivebayescpp
        src/main.cpp
        src/Parser.cpp
        src/Parser.h
        src/WordMatrix.cpp
        src/WordMatrix.h
        src/NewsItem.h
        src/utils.h
        src/utils.cpp
        src/WebHandler.cpp
        src/WebHandler.h
        src/filesystem.h
        src/NewsItem.cpp)
target_link_libraries(naivebayescpp stdc++fs eigen libcurl cxxopts nlohmann_json)
configure_file(${CMAKE_SOURCE_DIR}/stop_words.txt ${CMAKE_BINARY_DIR}/stop_words.txt COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/webconfig.json ${CMAKE_BINARY_DIR}/webconfig.json COPYONLY)
