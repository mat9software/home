#include "parse.h"

#include "common/log.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

//---------------------------------------------
//Extract stock price list.
std::vector<float> parse_request(char* buffer) {
#ifdef DEBUG_REQUEST
  LOG_INFO("Json Buffer to parse");
  char* it = buffer;
  while (*it != 0) {
    putchar(*it);
    it++;
  }
  putchar('\n');
#endif
  rapidjson::Document document;
  std::vector<float> ret;

  if (document.ParseInsitu(buffer).HasParseError()) {
    LOG_ERROR("Json Parser Error.");
    return ret;
  }

  if (!document.IsObject()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document.HasMember("chart")) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"].IsObject()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"].HasMember("result")) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"].IsArray()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!(document["chart"]["result"].Size() > 0)) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0].IsObject()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0].HasMember("indicators")) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"].IsObject()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"].HasMember("quote")) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"]["quote"].IsArray()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!(document["chart"]["result"][0]["indicators"]["quote"].Size() > 0)) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"]["quote"][0].IsObject()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"]["quote"][0].HasMember(
          "close")) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  if (!document["chart"]["result"][0]["indicators"]["quote"][0]["close"]
           .IsArray()) {
    LOG_ERROR("Parsing fetch failed.");
    return ret;
  }

  const int size =
      document["chart"]["result"][0]["indicators"]["quote"][0]["close"].Size();
  for (int i = 1; i < size; i++) {
    ret.push_back(
        document["chart"]["result"][0]["indicators"]["quote"][0]["close"][i]
            .GetFloat());
  }

  return ret;
}
