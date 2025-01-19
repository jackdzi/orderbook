#include <SocketHandler.hpp>
#include <matplotlibcpp.h>

namespace orderbook {

namespace plt = matplotlibcpp;

void SocketHandler::displayMetrics() {
  counter_++;
  time_data_.push_back(counter_);
  momentum_data_.push_back(buy_total_ - sell_total_);

  int window_size = 12000;
  int left = std::max(0, static_cast<int>(momentum_data_.size()) - window_size);
  double sum = 0;
  int count = 0;
  for (int i = left; i < static_cast<int>(momentum_data_.size()); i++) {
    sum += momentum_data_[i];
    count++;
  }

  if (!momentum_avg_data_.empty()) {
    momentum_avg_data_.push_back(sum / count);
  } else {
    momentum_avg_data_.push_back(0);
  }

  plt::clf();
  std::map<std::string, std::string> keywords1;
  keywords1["label"] = "Momentum";
  plt::plot(time_data_, momentum_data_, keywords1);

  std::map<std::string, std::string> keywords2;
  keywords2["label"] = "5 Minute Moving Average";
  keywords2["color"] = "red";
  plt::plot(time_data_, momentum_avg_data_, keywords2);

  plt::title("Momentum with Moving Average");
  plt::xlabel("Time (0.05s)");
  plt::ylabel("Momentum");
  plt::legend();
  plt::pause(0.01);
};
} // namespace orderbook
