#pragma once

#include <atomic>
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
  book() : order_counter(0) {};

  Price highest_bid();
  Price lowest_bid();

  void add_bid(const Order &order);

  size_t queue_size_at_price(Price price);
  const std::vector<Order> &get_bids_at_price(Price price);

  void add_seller(const Order &order);

private:
  bool match(Price price);
  // buyers
  std::map<Price, std::vector<Order>, std::greater<Price>> bid_book;
  // sellers
  std::map<Price, std::vector<Order>> ask_book;

  std::atomic<uint64_t> order_counter;
};
