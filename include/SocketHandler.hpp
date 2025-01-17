#ifndef ORDERBOOK_SOCKETHANDLER_HPP
#define ORDERBOOK_SOCKETHANDLER_HPP

#include <atomic>
#include <boost/asio/ssl.hpp>
#include <chrono>
#include <ctime>
//#include <iomanip>
//#include <iostream>
#include <jwt-cpp/jwt.h>
#include <map>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <thread>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <fstream>

namespace orderbook {

using json = nlohmann::json;
using websocketpp::connection_hdl;
typedef websocketpp::client<websocketpp::config::asio_tls_client> ws_client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

class SocketHandler {
public:
  SocketHandler(const std::string& url);
  ~SocketHandler();

  void start();
  void stop();
  void setTickers(const std::vector<std::string> tickers);

private:
  void run();
  void onMessage(const std::string &msg);
  void displayMetrics();

  std::string createJWT();

  std::string url_;
  std::string api_key_;
  std::thread thread_;
  std::atomic<bool> running_{false};

  struct Metrics {
    double last_price;
    double high_24h;
    double low_24h;
    double volume_24h;
    double best_bid;
    double best_ask;
    double spread;
    std::string last_update_time;
    int trade_count;
    double volume_weighted_price;
    double total_volume;
    double volitility;
  };

  std::vector<double> momentum_data_;
  std::vector<double> time_data_;
  double buy_total_;
  double sell_total_;
  int counter_ = 0;

  std::map<std::string, Metrics> metrics_;
  std::chrono::steady_clock::time_point last_print_time_;
};

} // namespace orderbook

#endif
