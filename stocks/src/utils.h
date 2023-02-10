#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "common/log.h"

// Split string with delimiter. Returns a list of not null string in this format[begin1, end1, begin2, end2]
std::vector<const char*> str_split_delimiter(const char* str, std::vector<char> delimiter);

#define FMT_STACK_STR(var_name, size, ...)          \
  char var_name[size] = "";                         \
  {                                                 \
    int cx = snprintf(var_name, size, __VA_ARGS__); \
    if (cx >= size) {                               \
      LOG_ERROR("String to long");                  \
    }                                               \
  }

#endif //UTILS_H
