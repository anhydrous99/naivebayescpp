//
// Created by armando on 11/16/19.
//

#ifndef NAIVEBAYESCPP_FILESYSTEM_H
#define NAIVEBAYESCPP_FILESYSTEM_H

#if __cplusplus > 201402L
#include <filesystem>
namespace fs = std::filesystem;
#else
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#endif //NAIVEBAYESCPP_FILESYSTEM_H
