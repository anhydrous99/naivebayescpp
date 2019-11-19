//
// Created by Armando Herrera on 11/15/19.
//
/**
 * Contains utility functions used by the rest of the program
 */

#ifndef NAIVEBAYESCPP_UTILS_H
#define NAIVEBAYESCPP_UTILS_H

#include <vector>
#include <string>
#include <random>
#include <iterator>

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
void part1(const std::string &newsgroup_path, bool print_latex, bool save_csv);

/*!
 * Implements the Reservoir Random Sampling Algorithm
 * @tparam Container The container type, must have random access
 * @tparam Itr A container's iterator
 * @tparam URNG A random number generator type
 * @param first The start of the container we are sampling from
 * @param last The end of the container we are sampling from
 * @param k The number of elements to sample
 * @param g The random number generator
 * @return A container of length k type Container that was randomly sampled
 */
template<typename Itr, typename URNG>
std::vector<typename std::iterator_traits<Itr>::value_type>
sample(Itr first, Itr last, typename std::iterator_traits<Itr>::difference_type k, URNG &&g) {
    typedef typename std::iterator_traits<Itr>::difference_type diff_type;
    typedef typename std::iterator_traits<Itr>::value_type val_type;
    diff_type i = 0;
    diff_type n = last - first;
    std::vector<val_type> reservoir(k);
    for (; i < k; i++)
        reservoir[i] = first[i];
    for (; i < n; i++) {
        std::uniform_int_distribution<diff_type> d(0, i);
        diff_type j = d(g);
        if (j < k)
            reservoir[j] = first[i];
    }
    return reservoir;
}

#endif //NAIVEBAYESCPP_UTILS_H
