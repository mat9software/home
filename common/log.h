#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <stdio.h>

#define LOG_ERROR(...)                                     \
  fprintf(stderr, "error : %s:%d : ", __FILE__, __LINE__); \
  fprintf(stderr, __VA_ARGS__);                            \
  fprintf(stderr, "\n");

#ifdef DEBUG_TRACE
#define LOG_TRACE(...)                                     \
  fprintf(stdout, "trace : %s:%d : ", __FILE__, __LINE__); \
  fprintf(stdout, __VA_ARGS__);                            \
  fprintf(stdout, "\n");
#else
#define LOG_TRACE(...) do { } while(0)
#endif

#define LOG_INFO(...)                                     \
  fprintf(stdout, "info : %s:%d : ", __FILE__, __LINE__); \
  fprintf(stdout, __VA_ARGS__);                            \
  fprintf(stdout, "\n");

#endif // COMMON_LOG_H
