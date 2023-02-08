#ifndef FETCH_H
#define FETCH_H

#include "data.h"

#include <functional>
#include <vector>

struct fetch_args
{
  size_t index;
  const char* range; //How far in the past we fetch historyic data.
  const char* stock_symbol;
  std::function<void(const graph_cb_args& add)> on_success;
  std::function<void(const graph_cb_args& add)> on_failure;
};

void fetch_stock(fetch_args& args);

#endif //FETCH_H
