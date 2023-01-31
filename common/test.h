#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#include <stdio.h>
#include <assert.h>

#define TEST_ASSERT_MSG(msg)                                   \
  printf("Test Error : %s, %s:%d\n", msg, __FILE__, __LINE__); \
  assert(0);                                               \

#define TEST_ASSERT_TRUE(expr)                                     \
  if (expr) {                                                      \
    printf("Test Success\n");                                      \
  } else {                                                         \
    printf("Test Error : %s, %s:%d\n", #expr, __FILE__, __LINE__); \
    assert(0);                                                 \
  }


#endif // COMMON_TEST_H
