#include "fetch.h"

#include <emscripten/fetch.h>
#include <stdio.h>
#include <string.h>

#include "common/log.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#include <string>

namespace {
  void on_fetch_failed(emscripten_fetch_t *fetch) {
    LOG_ERROR("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url,
           fetch->status);
    emscripten_fetch_close(fetch);  // Also free data on failure.
  };

  void on_fetch_success(emscripten_fetch_t *fetch) {

//mdtmp debug
/*
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes,
           fetch->url);
    for (int i = 0; i < fetch->numBytes; i++) {
      printf("%c", fetch->data[i]);
    }
    printf("\n");
*/

    rapidjson::Document document;
    char buffer[fetch->numBytes];
    memcpy(buffer, fetch->data, fetch->numBytes);

    std::function<void(const std::vector<float>)> &user_callback =
        *static_cast<std::function<void(const std::vector<float>)> *>(
            fetch->userData);

    emscripten_fetch_close(fetch);  // Free data associated with the fetch.

    if (document.ParseInsitu(buffer).HasParseError()) {
      LOG_ERROR("Json Parser Error.");
      return;
    }

    if (!document.IsObject()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document.HasMember("chart")) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"].IsObject()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"].HasMember("result")) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"]["result"].IsArray()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!(document["chart"]["result"].Size() > 0)) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"]["result"][0].IsObject()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"]["result"][0].HasMember("indicators")) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }

    if (!document["chart"]["result"][0]["indicators"].IsObject()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!document["chart"]["result"][0]["indicators"].HasMember("quote")) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!document["chart"]["result"][0]["indicators"]["quote"].IsArray()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!(document["chart"]["result"][0]["indicators"]["quote"].Size() > 0)) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!document["chart"]["result"][0]["indicators"]["quote"][0].IsObject()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!document["chart"]["result"][0]["indicators"]["quote"][0].HasMember(
            "close")) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    if (!document["chart"]["result"][0]["indicators"]["quote"][0]["close"]
             .IsArray()) {
      LOG_ERROR("Parsing fetch failed.");
      return;
    }
    const int size =
        document["chart"]["result"][0]["indicators"]["quote"][0]["close"]
            .Size();
    std::vector<float> ret;
    for (int i = 1; i < size; i++) {
      ret.push_back(
          document["chart"]["result"][0]["indicators"]["quote"][0]["close"][i]
              .GetFloat());
    }

    user_callback(ret);
  };
}

void fetch1(std::function<void(const std::vector<float>)> &cb) {
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attr.onsuccess = on_fetch_success;
  attr.onerror = on_fetch_failed;
  attr.userData = &cb;

  printf("Before callback mdtmp\n");
  printf("%p\n", attr.userData);

  emscripten_fetch(&attr,
                   "https://query1.finance.yahoo.com/v8/finance/chart/"
                   "aapl?metrics=high?&interval=1d&range=5d");
}
/*
chart->result[0]->indicators->quote[0]->close[...]
https://github.com/Tencent/rapidjson/blob/master/example/tutorial/tutorial.cpp

//https://query1.finance.yahoo.com/v8/finance/chart/aapl?metrics=high?&interval=1d&range=5d
{
  "chart": {
    "result": [
      {
        "meta": {
          "currency": "USD",
          "symbol": "AAPL",
          "exchangeName": "NMS",
          "instrumentType": "EQUITY",
          "firstTradeDate": 345479400,
          "regularMarketTime": 1675198805,
          "gmtoffset": -18000,
          "timezone": "EST",
          "exchangeTimezoneName": "America/New_York",
          "regularMarketPrice": 144.29,
          "chartPreviousClose": 142.53,
          "priceHint": 2,
          "currentTradingPeriod": {
            "pre": {
              "timezone": "EST",
              "start": 1675155600,
              "end": 1675175400,
              "gmtoffset": -18000
            },
            "regular": {
              "timezone": "EST",
              "start": 1675175400,
              "end": 1675198800,
              "gmtoffset": -18000
            },
            "post": {
              "timezone": "EST",
              "start": 1675198800,
              "end": 1675213200,
              "gmtoffset": -18000
            }
          },
          "dataGranularity": "1d",
          "range": "5d",
          "validRanges": [
            "1d",
            "5d",
            "1mo",
            "3mo",
            "6mo",
            "1y",
            "2y",
            "5y",
            "10y",
            "ytd",
            "max"
          ]
        },
        "timestamp": [
          1674657000,
          1674743400,
          1674829800,
          1675089000,
          1675198805
        ],
        "indicators": {
          "quote": [
            {
              "close": [
                141.86000061035156,
                143.9600067138672,
                145.92999267578125,
                143,
                144.2899932861328
              ],
              "open": [
                140.88999938964844,
                143.1699981689453,
                143.16000366210938,
                144.9600067138672,
                142.6999969482422
              ],
              "low": [
                138.80999755859375,
                141.89999389648438,
                143.0800018310547,
                142.85000610351562,
                142.27999877929688
              ],
              "high": [
                142.42999267578125,
                144.25,
                147.22999572753906,
                145.5500030517578,
                144.33999633789062
              ],
              "volume": [
                65799300,
                54105100,
                70492800,
                63947600,
                60339047
              ]
            }
          ],
          "adjclose": [
            {
              "adjclose": [
                141.86000061035156,
                143.9600067138672,
                145.92999267578125,
                143,
                144.2899932861328
              ]
            }
          ]
        }
      }
    ],
    "error": null
  }
}
*/
