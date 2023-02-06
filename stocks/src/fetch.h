#ifndef FETCH_H
#define FETCH_H

#include "data.h"

#include <functional>
#include <vector>

struct fetch_args
{
  graph_data& data;
  const char* range; //How far in the past we fetch historyic data.
};

void fetch_stock(fetch_args args);

#endif //FETCH_H
