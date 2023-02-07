#include "test.h"

#include "utils.h"

#include <stdio.h>

//--------------------------------------------------
void test_run() {
#ifdef RUN_TEST
const char* tmp = "Hey what's up, hola, hola2";
std::vector<const char*> ret = str_split_delimiter(tmp, {' ', ','});

#ifdef DEBUG_TRACE
  for(int i=0; i < ret.size(); i+=2) {
    const char* itr = ret[i];
    const char* end = ret[i+1];
    while(itr != end) {
      putchar(*itr);
      itr++;
    }
    putchar('\n');
  }
#endif


#endif
}

