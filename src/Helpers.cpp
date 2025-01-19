#include "Helpers.hpp"

template <typename MapType>
void addToMap(MapType &m, const std::shared_ptr<orderbook::Order> &order) {
  m[order->getPrice()].emplace(order->getOrderId(), order);
}

template <typename MapType>
void removeFromMap(MapType &m, const std::shared_ptr<orderbook::Order> &order) {
  double price = order->getPrice();

  auto priceIt = m.find(price);
  if (priceIt != m.end()) {
    priceIt->second.erase(order->getOrderId());
    if (priceIt->second.empty()) {
      m.erase(priceIt);
    }
  }
}
