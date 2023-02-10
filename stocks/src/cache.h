#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <optional>

//TODOS
// 1- Add Date to cached filename
// 2- In cache_init() Auto clean file cached more then one day. (file that is not the same day as today)
// To get date see : https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm 
void cache_init();
std::optional<std::string> cache_get(const char* key);
void cache_put(const char* key, const char* buffer);

void cache_rm_all();

#endif //CACHE_H
