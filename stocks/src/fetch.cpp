#include "fetch.h"

#include "common/log.h"
#include "parse.h"
#include "cache.h"
#include "utils.h"

#include <emscripten/fetch.h>
#include <stdio.h>
#include <string.h>

#include <string>

namespace {
//---------------------------------------------
  void on_fetch_failed(emscripten_fetch_t *fetch) {
    LOG_ERROR("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url,
           fetch->status);

    int error_code = fetch->status;
    fetch_args& args = *static_cast<fetch_args*>(fetch->userData);
    emscripten_fetch_close(fetch);  // Also free data on failure.

    args.on_failure({.values={}, .index=args.index}, error_code);
  };

//---------------------------------------------
  void on_fetch_success(emscripten_fetch_t *fetch) {
    // Copy empscripten buffer. I don't know why... Should use same buffer...
    char* buffer = (char*)malloc(fetch->numBytes+1);
    memcpy(buffer, fetch->data, fetch->numBytes);
    buffer[fetch->numBytes] = 0;

    fetch_args& args = *static_cast<fetch_args*>(fetch->userData);
    cache_put(args.stock_symbol, buffer);

    emscripten_fetch_close(fetch);  // Free data associated with the fetch.

    auto ret = parse_request(buffer);

    free(buffer);

    args.on_success({.values=ret, .index=args.index});
  };
}

//---------------------------------------------
void fetch_stock(fetch_args& args) {
  auto file_content = cache_get(args.stock_symbol);
  if(file_content) {
    auto ret = parse_request(&(*file_content)[0]);
    args.on_success({.values=ret, .index=args.index});
    return;
  }

  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = on_fetch_success;
  attr.onerror = on_fetch_failed;
  attr.userData = &args;

// URL behind proxy.
// yahoo finance data query
// https://query1.finance.yahoo.com/v8/finance/chart/%s?metrics=high...
  FMT_STACK_STR(buffer, 300, "yahoo/v8/finance/chart/%s?metrics=high?&interval=%s&range=%s", args.stock_symbol, args.interval, args.range);

#ifdef DEBUG_REQUEST
  LOG_INFO("GET stocks info : %s", buffer);
#endif

  emscripten_fetch(&attr, buffer);
}
