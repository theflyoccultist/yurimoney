#pragma once

#include <atomic>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using Amount = double;
using Price = double;

struct Order {
  uint64_t order_id;
  std::string user_id;
  Amount amount;
  Price price;
};

class book {
public:
  book() : order_counter(0), seller_counter(0) {};

  Price highest_bid() {
    if (bid_book.empty())
      return 0.0;
    return bid_book.begin()->first;
  }

  Price lowest_bid() {
    if (bid_book.empty())
      return 0.0;
    return bid_book.rbegin()->first;
  }

  void add_bid(const Order &order) {
    uint64_t next_id = ++order_counter;

    Order internal_order{next_id, order.user_id, order.amount, order.price};
    bid_book[order.price].push_back(internal_order);

    if (match(order.price)) {
      std::cout << "a match was found!\n";
    }
  }

  size_t queue_size_at_price(Price price) {
    if (bid_book.find(price) == bid_book.end())
      return 0;
    return bid_book[price].size();
  }

  const std::vector<Order> &get_bids_at_price(Price price) {
    return bid_book[price];
  }

  void add_seller(const Order &order) {
    uint64_t next_id = ++seller_counter;

    Order internal_order{next_id, order.user_id, order.amount, order.price};
    ask_book[order.price].push_back(internal_order);
  }

  bool match(Price price) {
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

private:
  // buyers
  std::map<Price, std::vector<Order>, std::greater<Price>> bid_book;
  std::atomic<uint64_t> order_counter;
  // sellers
  std::map<Price, std::vector<Order>> ask_book;
  std::atomic<uint64_t> seller_counter;
};
