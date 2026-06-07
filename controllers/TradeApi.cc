#include "TradeApi.h"
#include <memory>
#include <string>

void TradeApi::place_bid(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  LOG_DEBUG << "Place Bid";

  auto jsonPtr = req->getJsonObject();
  if (!jsonPtr) {
    bad_request(std::move(callback));
    return;
  }

  Json::Value json = *jsonPtr;

  // Json validation
  if (!json.isMember("user_id") || !json.isMember("amount") ||
      !json.isMember("price")) {
    bad_request(std::move(callback));
    return;
  }

  std::string user = json["user_id"].asString();
  Amount amount = json["amount"].asDouble();
  Price price = json["price"].asDouble();

  if (amount <= 0.0 || price <= 0.0 || user.empty()) {
    bad_request(std::move(callback));
    return;
  }

  // temp order state (ID doesn't matter yet)
  Order raw_bid{0, user, amount, price};

  // Commit to engine
  Order finalized_bid = order_book.add_bid(raw_bid);

  // Return the actual order data
  Json::Value ret = to_json(finalized_bid);
  auto resp = HttpResponse::newHttpJsonResponse(ret);
  resp->setStatusCode(HttpStatusCode::k201Created);
  callback(resp);
}

void TradeApi::place_ask(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  LOG_DEBUG << "Place ask";
}

void TradeApi::bad_request(
    std::function<void(const HttpResponsePtr &)> &&callback) {
  LOG_ERROR << "Bad request";
  auto resp = HttpResponse::newHttpResponse();
  resp->setStatusCode(HttpStatusCode::k400BadRequest);
  callback(resp);
}

Json::Value TradeApi::to_json(const Order &order) {
  Json::Value ret{};
  ret["order_id"] = order.order_id;
  ret["user_id"] = order.user_id;
  ret["amount"] = order.amount;
  ret["price"] = order.price;
  return ret;
}
