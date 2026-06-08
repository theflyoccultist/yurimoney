#pragma once

#include "Order.h"
#include <drogon/HttpController.h>
#include <functional>

using namespace drogon;

class TradeApi : public drogon::HttpController<TradeApi> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TradeApi::get_books, "/book", Get);
  ADD_METHOD_TO(TradeApi::place_bid, "/buy", Post);
  ADD_METHOD_TO(TradeApi::place_ask, "/sell", Post);
  METHOD_LIST_END

  void get_books(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
  void place_bid(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
  void place_ask(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);

private:
  void bad_request(std::function<void(const HttpResponsePtr &)> &&callback);
  Json::Value to_json(const Order &order);
  book order_book;
};
