#ifndef FETCH_H
#define FETCH_H

#include <functional>
#include <vector>

void fetch_test();

void fetch1(std::function<void(const std::vector<float>)>& cb);

#endif //FETCH_H
