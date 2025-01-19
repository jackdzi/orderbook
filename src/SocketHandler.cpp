#include <SocketHandler.hpp>
#include <chrono>
#include <cstdlib>

namespace orderbook {

SocketHandler::SocketHandler(const std::string &url)
    : url_(url),
      last_print_time_(std::chrono::steady_clock::now()) {}

SocketHandler::~SocketHandler() { stop(); }

void SocketHandler::start() {
  running_.store(true);
  thread_ = std::thread(&SocketHandler::run, this);
}

void SocketHandler::stop() {
  running_.store(false);
  if (thread_.joinable()) {
    thread_.join();
  }
}

static context_ptr onTlsInit(websocketpp::connection_hdl) {
  context_ptr ctx = std::make_shared<boost::asio::ssl::context>(
      boost::asio::ssl::context::sslv23);
  try {
    ctx->set_options(boost::asio::ssl::context::default_workarounds |
                     boost::asio::ssl::context::no_sslv2 |
                     boost::asio::ssl::context::no_sslv3 |
                     boost::asio::ssl::context::single_dh_use);
  } catch (std::exception &e) {
    std::cerr << "Error in TLS context setup: " << e.what() << std::endl;
  }
  return ctx;
}

std::pair<std::string, std::string> generate_signature() {
  auto now = std::chrono::system_clock::now();
  auto timestamp = std::to_string(
      std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
          .count());

  std::string api_key;
  std::string home = std::getenv("HOME");
  std::ifstream key_file(home + "/Documents/keys/cdp_api_key.json");
  nlohmann::json key;
  if (key_file.is_open()) {
    key_file >> key;
    api_key = key["privateKey"].get<std::string>();
  }
  key_file.close();

  auto token = jwt::create()
                   .set_type("JWT")
                   .set_issuer("cdp")
                   .set_issued_at(now)
                   .set_expires_at(now + std::chrono::minutes{2})
                   .set_subject("069a39ab-affe-45bc-96bb-a1a90ab23389")
                   .sign(jwt::algorithm::es256("069a39ab-affe-45bc-96bb-a1a90ab23389", api_key));

  return {token, timestamp};
}

void SocketHandler::run() {
  ws_client client;
  try {
    client.init_asio();
    client.set_tls_init_handler(onTlsInit);
    client.clear_access_channels(websocketpp::log::alevel::all);
    client.set_error_channels(websocketpp::log::elevel::all);

    client.set_message_handler(
        [this](connection_hdl hdl, ws_client::message_ptr msg) {
          onMessage(msg->get_payload());
        });

    client.set_fail_handler([](connection_hdl hdl) {
      std::cerr << "Connection failed" << std::endl;
    });

    client.set_close_handler([](connection_hdl hdl) {
      std::cerr << "Connection closed" << std::endl;
    });

    client.set_open_handler([&client, this](connection_hdl hdl) {
      std::cout << "Connection established" << std::endl;

      auto [token, timestamp] = generate_signature();

      nlohmann::json subscription = {
          {"type", "subscribe"},
          {"product_ids", {"BTC-USD"}},
          {"channels", {"level2_batch"}},
          {"signature", token},
          {"api_key", "069a39ab-affe-45bc-96bb-a1a90ab23389"},
          {"passphrase", ""},
          {"timestamp", timestamp}};
      std::string message = subscription.dump();
      websocketpp::lib::error_code ec;
      client.send(hdl, message, websocketpp::frame::opcode::text, ec);

      if (ec) {
        std::cerr << "Error sending subscription request: " << ec.message()
                  << std::endl;
      } else {
        std::cout << "Subscription request sent" << std::endl;
      }
    });

    websocketpp::lib::error_code ec;
    ws_client::connection_ptr conn = client.get_connection(url_, ec);
    if (ec) {
      std::cerr << "Could not create connection: " << ec.message() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    client.connect(conn);
    while (running_.load()) {
      client.run_one();
    }
  } catch (const websocketpp::exception &e) {
    std::cerr << "WebSocket exception: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Standard exception: " << e.what() << std::endl;
  }
}

void SocketHandler::onMessage(const std::string &msg) {

  if (!running_.load()) {
    return;
  }

  try {
    auto j = json::parse(msg);

    std::cout << j << std::endl;
    if (j.contains("changes")) {
        for (const auto& change : j["changes"]) {
            if (change.size() > 2) {
                if (change[0] == "buy") {
                    buy_total_ += std::stod(static_cast<std::string>(change[2]));
                } else if (change[0] == "sell") {
                    sell_total_ += std::stod(static_cast<std::string>(change[2]));
                }
            }
        }
    }
    displayMetrics();
    if (j.contains("type") && j["type"] == "ticker") {
      std::string product_id = j["product_id"];
      auto &metric = metrics_[product_id];
      std::cout << "Called!" << std::endl;
      metric.last_price = std::stod(j["price"].get<std::string>());
      metric.high_24h = std::stod(j["high_24h"].get<std::string>());
      metric.low_24h = std::stod(j["low_24h"].get<std::string>());
      metric.volume_24h = std::stod(j["volume_24h"].get<std::string>());
      metric.best_bid = std::stod(j["best_bid"].get<std::string>());
      metric.best_ask = std::stod(j["best_ask"].get<std::string>());
      metric.spread = metric.best_ask - metric.best_bid;
      metric.last_update_time = j["time"];
      metric.trade_count++;

      // Update VWAP calculations
      double last_size = std::stod(j["last_size"].get<std::string>());
      metric.volume_weighted_price += metric.last_price * last_size;
      metric.total_volume += last_size;

      // Print updated metrics
    }

    // Don't need to process heartbeat messages for now

  } catch (const std::exception &ex) {
    std::cerr << "onMessage parse error: " << ex.what() << "\n";
  }
}


} // namespace orderbook
//
//
