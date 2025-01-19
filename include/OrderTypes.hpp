#ifndef ORDERBOOK_ORDERTYPES_HPP
#define ORDERBOOK_ORDERTYPES_HPP

#include <cstdint>

namespace orderbook {

enum class Side { Buy, Sell };

enum class OrderStatus { New, PartiallyFilled, Filled, Cancelled };

struct OrderUpdate {
  std::uint64_t orderId;
  Side side;
  double price;
  std::uint64_t quantity;
  OrderStatus status;
};

} // namespace orderbook

#endif
