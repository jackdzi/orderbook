#ifndef ORDERBOOKHELPERS_HPP
#define ORDERBOOKHELPERS_HPP

#include "Order.hpp"
#include <memory>

template <typename MapType>
void addToMap(MapType &m, const std::shared_ptr<orderbook::Order> &order);

template <typename MapType>
void removeFromMap(MapType &m, const std::shared_ptr<orderbook::Order> &order);

#endif
