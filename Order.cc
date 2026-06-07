#include "Order.h"
#include <algorithm>

Price book::highest_bid() {
  if (bid_book.empty())
    return 0.0;
  return bid_book.begin()->first;
}

Price book::lowest_bid() {
  if (bid_book.empty())
    return 0.0;
  return bid_book.rbegin()->first;
}

void book::add_bid(const Order &order) {
  uint64_t next_id = ++order_counter;

  Order internal_order{next_id, order.user_id, order.amount, order.price};
  bid_book[order.price].push_back(internal_order);

  match();
}

size_t book::bid_queue_size_at_price(Price price) {
  if (bid_book.find(price) == bid_book.end())
    return 0;
  if (bid_book[price].empty())
    return 0;
  return bid_book[price].size();
}

const std::vector<Order> &book::get_bids_at_price(Price price) {
  return bid_book[price];
}

Price book::lowest_sell() {
  if (ask_book.empty())
    return 0.0;
  return ask_book.begin()->first;
}

void book::add_seller(const Order &order) {
  uint64_t next_id = ++order_counter;

  Order internal_order{next_id, order.user_id, order.amount, order.price};
  ask_book[order.price].push_back(internal_order);
}

size_t book::ask_queue_size_at_price(Price price) {
  if (ask_book.find(price) == ask_book.end())
    return 0;
  return ask_book[price].size();
}

Amount book::get_top_ask_amount(Price price) {
  if (ask_book.empty())
    return 0.0;

  return ask_book[price].front().amount;
}

void book::match() {
  if (bid_book.empty() || ask_book.empty())
    return;

  auto best_bid_it = bid_book.begin(); // Highest buying price
  auto best_ask_it = ask_book.begin(); // Lowest selling price

  Price buyer_price = best_bid_it->first;
  Price seller_price = best_ask_it->first;

  // Does the buyer's max budged cover the seller's asking price?
  if (buyer_price >= seller_price) {
    auto &buy_queue = best_bid_it->second;
    auto &sell_queue = best_ask_it->second;

    if (buy_queue.empty() || sell_queue.empty())
      return;

    Order &buy_order = buy_queue.front();
    Order &sell_order = sell_queue.front();
    Amount trade_amount = std::min(buy_order.amount, sell_order.amount);

    buy_order.amount -= trade_amount;
    sell_order.amount -= trade_amount;

    // Clean up empty orders from the vectors
    if (buy_order.amount == 0)
      buy_queue.erase(buy_queue.begin());
    if (sell_order.amount == 0)
      sell_queue.erase(sell_queue.begin());

    // Clean up empty price shelves from the map
    if (buy_queue.empty())
      bid_book.erase(best_bid_it);
    if (sell_queue.empty())
      ask_book.erase(best_ask_it);

    match();
  }
}
