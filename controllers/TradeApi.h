#pragma once

#include "Order.h"
#include <drogon/HttpController.h>
#include <functional>

using namespace drogon;

class TradeApi : public drogon::HttpController<TradeApi> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TradeApi::place_bid, "/buy", Post);
  ADD_METHOD_TO(TradeApi::place_ask, "/sell", Post);
  METHOD_LIST_END

  void place_bid(const HttpRequestPtr &req,
                 std::function<void(const HttpRequestPtr &)> &&callback);
  void place_ask(const HttpRequestPtr &req,
                 std::function<void(const HttpRequestPtr &)> &&callback);

private:
  book order_book;
};
