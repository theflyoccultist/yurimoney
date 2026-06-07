#include "TradeApi.h"
#include <memory>

void TradeApi::place_bid(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  LOG_DEBUG << "Place Bid";

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    LOG_ERROR << "Bad request";
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k400BadRequest);
    callback(resp);
    return;
  }

  Json::Value json = *jsonPtr;
  Order new_bid{
      0,
      json["user_id"].asString(),
      json["amount"].asDouble(),
      json["price"].asDouble(),
  };

  order_book.add_bid(new_bid);

  Json::Value ret{};
  ret = toJson(new_bid);
  auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
  resp->setStatusCode(HttpStatusCode::k201Created);
  callback(resp);
}

void TradeApi::place_ask(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  LOG_DEBUG << "Place ask";
}

Json::Value TradeApi::toJson(const Order &order) {
  Json::Value ret{};
  ret["order_id"] = order.order_id;
  ret["user_id"] = order.user_id;
  ret["amount"] = order.amount;
  ret["price"] = order.price;
  return ret;
}
