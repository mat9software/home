#ifndef COMMON_ASSERT_H
#define COMMON_ASSERT_H

#include "log.h"

#include <assert.h>

#define ASSERT(expr)            \
if(expr){}                      \
else{                           \
LOG_ERROR("Assertion failed."); \
assert(expr);                   \
} 

#endif  // COMMON_ASSERT_H
