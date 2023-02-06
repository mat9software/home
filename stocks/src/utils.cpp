#include "utils.h"

#include "common/log.h"
#include "common/assert.h"

//--------------------------------------------------
std::vector<const char*> str_split_delimiter(
    const char* str, std::vector<char> delimiter) {
  std::vector<const char*> ret;

  //LOG_TRACE("START Testing str_split_delimiter algorithm");
  //LOG_TRACE("String to split : %s", str);
  const char* end = str;
  while (*end != 0) {
    end++;
  }

  const char* itr = str;
  const char* begin = itr;
  while (true) {
    if (*itr == ' ' || *itr == ',' || itr == end) {
      if (itr == begin) {
        // empty word
        if (itr == end) break;

        itr++;
        begin = itr;
        continue;
      }

      // found a word
      //LOG_TRACE("begin:%p", begin);
      //LOG_TRACE("end:%p", itr);
      ret.push_back(begin);
      ret.push_back(itr); //end

      if (itr == end) break;
      itr++;
      begin = itr;
      continue;
    }

    itr++;
  }

  
  ASSERT(ret.size() % 2 == 0);
//#ifdef DEBUG_TRACE
//  for(int i=0; i < ret.size(); i+=2) {
//    const char* itr = ret[i];
//    const char* end = ret[i+1];
//    while(itr != end) {
//      putchar(*itr);
//      itr++;
//    }
//    putchar('\n');
//  }
//#endif
  //LOG_TRACE("END Testing str_split_delimiter algorithm");

  return ret;
}
