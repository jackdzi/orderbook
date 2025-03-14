#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include "Order.hpp"
#include "SpinLock.hpp"
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace orderbook {

class OrderBook {
public:
  OrderBook() = default;

  void addOrder(const std::shared_ptr<Order> &order);

  void cancelOrder(std::uint64_t orderId);

  std::vector<OrderUpdate> matchOrders();

  void matchingEngineThread(std::atomic<bool> &running);

private:
  // Price maps to another map of orderId -> Order
  // Bids sorted descending, Asks sorted ascending
  std::map<double, std::unordered_map<std::uint64_t, std::shared_ptr<Order>>,
           std::greater<double>>
      bids_;
  std::map<double, std::unordered_map<std::uint64_t, std::shared_ptr<Order>>>
      asks_;

  std::unordered_map<std::uint64_t, std::shared_ptr<Order>> allOrders_;

  SpinLock lock_;
};

} // namespace orderbook

#endif
