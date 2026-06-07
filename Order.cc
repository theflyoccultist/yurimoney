#include "Order.h"
#include <iostream>

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

  if (match(order.price)) {
    std::cout << "a match was found!\n";
  }
}

size_t book::queue_size_at_price(Price price) {
  if (bid_book.find(price) == bid_book.end())
    return 0;
  return bid_book[price].size();
}

const std::vector<Order> &book::get_bids_at_price(Price price) {
  return bid_book[price];
}

void book::add_seller(const Order &order) {
  uint64_t next_id = ++order_counter;

  Order internal_order{next_id, order.user_id, order.amount, order.price};
  ask_book[order.price].push_back(internal_order);
}

bool book::match(Price price) {
  if (bid_book.empty() || ask_book.empty())
    return false;

  for (auto &[bid_price, _] : bid_book) {
    for (auto &[ask_price, _] : ask_book) {
      if (bid_price == ask_price) {
        bid_book.erase(bid_price);
        ask_book.erase(ask_price);
        return true;
      }
    }
  }
  return false;
}
