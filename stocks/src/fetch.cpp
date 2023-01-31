#include "fetch.h"

#include <stdio.h>
#include <string.h>
#include <emscripten/fetch.h>
#include "rapidjson/document.h"

void fetch_test()
{
//mdtmp
//mdtmp https://query1.finance.yahoo.com/v8/finance/chart/aapl?metrics=high?&interval=1d&range=5d 


//mdtmp #include <stdio.h>
//mdtmp #include <string.h>
//mdtmp #include <emscripten/fetch.h>

auto downloadSucceeded = [](emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
  for(int i = 0; i < fetch->numBytes; i++) {
    printf("%c", fetch->data[i]);
  }
  printf("\n");



  rapidjson::Document document;
  char buffer[fetch->numBytes];
  memcpy(buffer, fetch->data, fetch->numBytes);
  if (!document.ParseInsitu(buffer).HasParseError()) {
    //mdtmp TODO
    // https://github.com/Tencent/rapidjson/blob/master/example/tutorial/tutorial.cpp
  } else {
    printf("Json Parser Error.");
  }
  //snprintf(fetch->data, fetch->numBytes, "%s");
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
};

auto downloadFailed = [](emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
};

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = downloadSucceeded;
  attr.onerror = downloadFailed;
  emscripten_fetch(&attr, "https://query1.finance.yahoo.com/v8/finance/chart/aapl?metrics=high?&interval=1d&range=5d");

}
