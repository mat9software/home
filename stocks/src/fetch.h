#ifndef FETCH_H
#define FETCH_H

#include <functional>
#include <vector>

struct fetch_args
{
  const char* symbol_name;
  std::function<void(const std::vector<float>)>& success_cb;
};

void fetch_test();

void fetch1(const fetch_args& args);

#endif //FETCH_H
