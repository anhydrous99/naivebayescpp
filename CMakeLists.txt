cmake_minimum_required(VERSION 3.15)

function(download_file url filename hash_type hash)
    if (NOT EXISTS ${filename})
        file(DOWNLOAD ${url} ${filename}
                TIMEOUT 60
                EXPECTED_HASH ${hash_type}=${hash}
                TLS_VERIFY ON
                )
    endif ()
endfunction(download_file)

project(naivebayescpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DOWNLOAD_MININEWSGROUPS "Downloads mininewsgroups from internet" OFF)

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

if (DOWNLOAD_MININEWSGROUPS)
    download_file(
            https://archive.ics.uci.edu/ml/machine-learning-databases/20newsgroups-mld/mini_newsgroups.tar.gz
            ${CMAKE_BINARY_DIR}/mini_newsgroups.tar.gz
            SHA1
            4d70ff2ffc53da609f3a7eb527dcf94ee2f23403
    )
    execute_process(
            COMMAND tar -vxf ${CMAKE_BINARY_DIR}/mini_newsgroups.tar.gz
    )
endif ()