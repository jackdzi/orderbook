#ifndef ORDERBOOK_ORDER_HPP
#define ORDERBOOK_ORDER_HPP

#include "OrderTypes.hpp"

namespace orderbook {

class Order {
public:
  Order(std::uint64_t orderId, Side side, double price, std::uint64_t quantity)
      : orderId_(orderId), side_(side), price_(price), quantity_(quantity),
        status_(OrderStatus::New) {}

  std::uint64_t getOrderId() const { return orderId_; }
  Side getSide() const { return side_; }
  double getPrice() const { return price_; }
  std::uint64_t getQuantity() const { return quantity_; }
  OrderStatus getStatus() const { return status_; }

  void setQuantity(std::uint64_t newQty) { quantity_ = newQty; }
  void setStatus(OrderStatus status) { status_ = status; }

private:
  std::uint64_t orderId_;
  Side side_;
  double price_;
  std::uint64_t quantity_;
  OrderStatus status_;
};

} // namespace orderbook

#endif
