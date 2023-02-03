#ifndef DATA_H
#define DATA_H

#include <vector>
#include <functional>

struct graph_add
{
  const std::vector<float> values;
  size_t index;
};

struct graph_data {
   std::vector<float> values = {};
   char stock_symbol[10] = "aapl";
   float min = -1.0f;
   float max = 1.0f;
   size_t index;
   //const int index;
   std::function<void(const graph_add& add)> update_graph_cb;
};


#endif //DATA_H
