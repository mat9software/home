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
    char buffer[fetch->numBytes];
    memcpy(buffer, fetch->data, fetch->numBytes);

    std::function<void(const std::vector<float>)> &user_callback =
        *static_cast<std::function<void(const std::vector<float>)> *>(
            fetch->userData);

    emscripten_fetch_close(fetch);  // Free data associated with the fetch.

    auto ret = parse_request(buffer);

    user_callback(ret);
  };
}

//---------------------------------------------
void fetch1(const fetch_args& args) {
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = on_fetch_success;
  attr.onerror = on_fetch_failed;
  attr.userData = &args.success_cb;

  char buffer[300]="";
  int cx;  // FIXME Check return value.
  cx = snprintf(buffer, 300, "https://query1.finance.yahoo.com/v8/finance/chart/%s?metrics=high?&interval=1d&range=100d", args.symbol_name);
  LOG_INFO("%s", buffer);

//BUG Sometime doesn't work. Error on parsing.
  emscripten_fetch(&attr, buffer);
  //mdtmp emscripten_fetch(&attr, "https://query1.finance.yahoo.com/v8/finance/chart/aapl?metrics=high?&interval=1d&range=100d");
}
