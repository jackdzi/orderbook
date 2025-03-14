#include "OrderBook.hpp"
#include "Helpers.hpp"
#include <thread>

namespace orderbook {

// auto newOrder = std::make_shared<Order>(orderId, side, price, quantity);
void OrderBook::addOrder(const std::shared_ptr<Order> &order) {
  lock_.lock();

  allOrders_[order->getOrderId()] = order;

  if (order->getSide() == Side::Buy) {
    addToMap(bids_, order);
  } else {
    addToMap(asks_, order);
  }

  lock_.unlock();
}

void OrderBook::cancelOrder(std::uint64_t orderId) {
  lock_.lock();

  auto location = allOrders_.find(orderId);
  if (location != allOrders_.end()) {
    auto order = location->second;

    if (order->getSide() == Side::Buy) {
      removeFromMap(bids_, order);
    } else {
      removeFromMap(asks_, order);
    }

    order->setStatus(OrderStatus::Cancelled);
    allOrders_.erase(location);
  }

  lock_.unlock();
}

std::vector<OrderUpdate> OrderBook::matchOrders() {
  lock_.lock();
  std::vector<OrderUpdate> updates;

  if (bids_.empty() || asks_.empty()) {
    lock_.unlock();
    return updates; // No possible match
  }

  // bids_.begin()->first is the highest bid
  // asks_.begin()->first is the lowest ask
  double bestBidPrice = bids_.begin()->first;
  double bestAskPrice = asks_.begin()->first;

  // TODO: Implement matching algorithm

  lock_.unlock();
  return updates;
}

void OrderBook::matchingEngineThread(std::atomic<bool> &running) {
  while (running) {
    auto updates = this->matchOrders();

    if (!updates.empty()) {
      for (const auto &update : updates) {
        // TODO: Process updates
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

} // namespace orderbook
