#include <SocketHandler.hpp>
#include <matplotlibcpp.h>

namespace orderbook {

namespace plt = matplotlibcpp;

void SocketHandler::displayMetrics() {
  counter_++;
  time_data_.push_back(counter_);
  momentum_data_.push_back(buy_total_ - sell_total_);

  int window_size = 150;
  std::vector<double> moving_average_data(momentum_data_.size(), 0.0);
  for (size_t i = 0; i < momentum_data_.size(); i++) {
    double sum = 0.0;
    int count = 0;
    for (size_t j = i; j > 0 && count < window_size; j--) {
      sum += momentum_data_[j];
      count++;
    }
    moving_average_data[i] = sum / count;
  }

  plt::clf();
  std::map<std::string, std::string> keywords1;
  keywords1["label"] = "Momentum";
  plt::plot(time_data_, momentum_data_,
            keywords1);

  std::map<std::string, std::string> keywords2;
  keywords2["label"] = "Moving Average";
  keywords2["color"] = "red";
  plt::plot(time_data_, moving_average_data, keywords2);

  plt::title("Momentum with Moving Average");
  plt::xlabel("Time");
  plt::ylabel("Momentum");
  plt::legend();
  plt::pause(0.01);
};
} // namespace orderbook
