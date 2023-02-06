#ifndef UTILS_H
#define UTILS_H

#include <vector>

// Split string with delimiter. Returns a list of not null string in this format[begin1, end1, begin2, end2]
std::vector<const char*> str_split_delimiter(const char* str, std::vector<char> delimiter);

#endif //UTILS_H
