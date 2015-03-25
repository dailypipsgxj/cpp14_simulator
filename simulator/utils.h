#ifndef UTILS_H
#define UTILS_H

#include <vector>

float sum_vector(std::vector<float>);
[[ noreturn ]] void bail(const char *);
int get_nanoseconds();
void trim_strings(std::vector<std::string> &);

#endif
