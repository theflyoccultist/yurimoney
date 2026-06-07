#define DROGON_TEST_MAIN

#include "../Order.h"
#include <drogon/drogon.h>
#include <drogon/drogon_test.h>
#include <vector>

DROGON_TEST(Price_Priority) {
  book b;
  b.add_bid({
      .user_id = "yuri",
      .amount = 40,
      .price = 90.0,
  });

  b.add_bid({
      .user_id = "pwatpwat",
      .amount = 60,
      .price = 105.0,
  });

  b.add_bid({
      .user_id = "john",
      .amount = 70,
      .price = 95.0,
  });

  b.add_bid({
      .user_id = "linda",
      .amount = 150,
      .price = 100.0,
  });

  b.add_bid({
      .user_id = "sally",
      .amount = 170,
      .price = 110.0,
  });

  CHECK(b.highest_bid() == 110.0);
  CHECK(b.lowest_bid() == 90.0);
}

DROGON_TEST(Time_Priority) {
  book b;
  b.add_bid({.user_id = "pwat", .amount = 50, .price = 100.0});
  b.add_bid({.user_id = "pwatpwot", .amount = 880, .price = 100.0});
  b.add_bid({.user_id = "pweep", .amount = 500, .price = 100.0});
  auto bids = b.get_bids_at_price(100.0);

  std::vector<uint64_t> expected = {1, 2, 3};

  for (size_t i = 0; i < bids.size(); ++i) {
    CHECK(bids[i].order_id == expected[i]);
  }
}

DROGON_TEST(Perfect_match) {
  book b;
  for (int i = 0; i < 10; ++i) {
    b.add_seller({.user_id = "10_sellers", .amount = 69, .price = 420.0});
    b.add_bid({.user_id = "10_buyers", .amount = 69, .price = 420.0});
  }

  CHECK(b.bid_queue_size_at_price(420.0) == 0);
}

DROGON_TEST(One_big_order_many_small_orders) {
  book b;
  b.add_seller({.user_id = "pwat1", .amount = 10.0, .price = 100.0});
  b.add_seller({.user_id = "pwat2", .amount = 10.0, .price = 100.0});
  b.add_seller({.user_id = "pwat3", .amount = 10.0, .price = 100.0});
  b.add_bid({.user_id = "pwatbuy", .amount = 25.0, .price = 100.0});

  // The buy queue should be completely empty because it was fully filled
  CHECK(b.bid_queue_size_at_price(100.0) == 0);

  // The sell queue should have exactly ONE order left inside it (the partially
  // filled third one)
  CHECK(b.ask_queue_size_at_price(100.0) == 1);
  // That remaining order must have exactly 5.0 units left
  CHECK(b.get_top_ask_amount(100) == 5.0);
}

DROGON_TEST(Price_Overlap) {
  book b;
  b.add_seller({.user_id = "desperate", .amount = 10.0, .price = 95.0});
  b.add_bid({.user_id = "pwato", .amount = 10.0, .price = 100.0});

  CHECK(b.bid_queue_size_at_price(100.0) == 0);
  CHECK(b.ask_queue_size_at_price(100.0) == 0);
}

DROGON_TEST(Sweep_Book) {
  book b;
  b.add_seller({.user_id = "sell1", .amount = 5.0, .price = 101.0});
  b.add_seller({.user_id = "sell2", .amount = 5.0, .price = 102.0});
  b.add_bid({.user_id = "buy1", .amount = 10.0, .price = 105.0});

  CHECK(b.ask_queue_size_at_price(101.0) == 0);
  CHECK(b.ask_queue_size_at_price(102.0) == 0);
  CHECK(b.bid_queue_size_at_price(105.0) == 0);
}

DROGON_TEST(No_Match) {
  book b;
  b.add_seller({.user_id = "sell1", .amount = 10.0, .price = 110.0});
  b.add_bid({.user_id = "buy1", .amount = 10.0, .price = 90.0});

  CHECK(b.bid_queue_size_at_price(90.0) == 1);
  CHECK(b.ask_queue_size_at_price(110.0) == 1);
}

int main(int argc, char **argv) {
  using namespace drogon;

  std::promise<void> p1;
  std::future<void> f1 = p1.get_future();

  // Start the main loop on another thread
  std::thread thr([&]() {
    // Queues the promise to be fulfilled after starting the loop
    app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
    app().run();
  });

  // The future is only satisfied after the event loop started
  f1.get();
  int status = test::run(argc, argv);

  // Ask the event loop to shutdown and wait
  app().getLoop()->queueInLoop([]() { app().quit(); });
  thr.join();
  return status;
}
