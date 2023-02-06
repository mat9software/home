#include "fetch.h"

#include "common/log.h"
#include "parse.h"

#include <emscripten/fetch.h>
#include <stdio.h>
#include <string.h>

#include <string>

namespace {
//---------------------------------------------
  void on_fetch_failed(emscripten_fetch_t *fetch) {
    LOG_ERROR("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url,
           fetch->status);
    emscripten_fetch_close(fetch);  // Also free data on failure.
  };

//---------------------------------------------
  void on_fetch_success(emscripten_fetch_t *fetch) {
    char* buffer = (char*)malloc(fetch->numBytes+1);
    memcpy(buffer, fetch->data, fetch->numBytes);
    buffer[fetch->numBytes] = 0;

    graph_data& data = *static_cast<graph_data*>(fetch->userData);

    emscripten_fetch_close(fetch);  // Free data associated with the fetch.

    auto ret = parse_request(buffer);

    free(buffer);

    data.update_graph_cb({.values=ret, .index=data.index});
  };
}

//---------------------------------------------
void fetch_stock(fetch_args args) {
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = on_fetch_success;
  attr.onerror = on_fetch_failed;
  attr.userData = &args.data;

  const size_t SIZE = 300;
  char buffer[SIZE]="";
  int cx = snprintf(buffer, 300, "yahoo/v8/finance/chart/%s?metrics=high?&interval=1d&range=%s", args.data.stock_symbol, args.range);
  if(cx >= SIZE) {
    LOG_ERROR("String to long");
  }
#ifdef DEBUG_REQUEST
  LOG_INFO("GET stocks info : %s", buffer);
#endif

  emscripten_fetch(&attr, buffer);
}
