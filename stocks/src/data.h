#ifndef DATA_H
#define DATA_H

#include <vector>
#include <functional>

struct graph_cb_args
{
  const std::vector<float> values;
  size_t index;
};

const size_t STOCK_SYMBOL_LENGTH = 10;
struct graph_data {
   std::vector<float> values = {};
   char stock_symbol[STOCK_SYMBOL_LENGTH] = "";
   float min = -1.0f;
   float max = 1.0f;
   float ratio_begin_to_end = 0.0f;
   float ratio_min_to_max = 0.0f;
};



#endif //DATA_H
