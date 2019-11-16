//
// Created by Armando Herrera on 11/15/19.
//
/**
 * Contains utility functions used by the rest of the program
 */

#ifndef NAIVEBAYESCPP_UTILS_H
#define NAIVEBAYESCPP_UTILS_H

#include <string>

/*!
 * Prints a string to a file
 * @param to_print What to print
 * @param path The path to the file to print
 */
void print_to_file(const std::string &to_print, const std::string &path);

/*!
 * Runs the test created for part 1 of the project
 * @param newsgroup_path The path to the news group dataset
 * @param print_latex Whether to print in latex syntax
 */
void part1(const std::string &newsgroup_path, bool print_latex);


#endif //NAIVEBAYESCPP_UTILS_H
