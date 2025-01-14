#include "../include/SocketHandler.hpp"
#include <cstdlib>

namespace orderbook {

SocketHandler::SocketHandler(const std::string& url, const std::string& api_key)
    : url_(url),
      api_key_(api_key),
      last_time_(std::chrono::steady_clock::now()) {}

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

ws_client SocketHandler::createClient() {
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
      std::cerr << "[FeedHandler] Connection failed" << std::endl;
    });

    client.set_close_handler([](connection_hdl hdl) {
      std::cerr << "[FeedHandler] Connection closed" << std::endl;
    });

    client.set_open_handler([&client](connection_hdl hdl) {
      std::cout << "[FeedHandler] Connection established" << std::endl;

      nlohmann::json subscription = {
          {"type", "subscribe"},
          {"product_ids", {"BTC-USD", "ETH-BTC"}},
          {"channels",
           {"level2",
            "heartbeat",
            {{"name", "ticker"}, {"product_ids", {"ETH-BTC"}}}}}};

      // TODO: Read api key and finish jwt token implementation
      auto token = jwt::create().set_type().sign(
          jwt::algorithm::es256{"put api key here"})

                       nlohmann::json subscriptionNew = {
          {"type", "subscribe"},
          {"product_ids", {"BTC-USD", "ETH-BTC"}},
          {"channels",
           {"level2",
            "heartbeat",
            {{"name", "ticker"},
             {"product_ids", {"ETH-BTC"}},
             {"jwt", token}}}}};

      std::string message = subscription.dump();
      websocketpp::lib::error_code ec;
      client.send(hdl, message, websocketpp::frame::opcode::text, ec);

      if (ec) {
        std::cerr << "[FeedHandler] Error sending subscription request: "
                  << ec.message() << std::endl;
      } else {
        std::cout << "[FeedHandler] Subscription request sent" << std::endl;
      }
    });

    websocketpp::lib::error_code ec;
    ws_client::connection_ptr conn = client.get_connection(url_, ec);
    if (ec) {
      std::cerr << "[FeedHandler] Could not create connection: " << ec.message()
                << std::endl;
      std::exit(EXIT_FAILURE);
    }

    client.connect(conn);

    while (running_.load()) {
      client.run_one();
    }
  } catch (const websocketpp::exception &e) {
    std::cerr << "[FeedHandler] WebSocket exception: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[FeedHandler] Standard exception: " << e.what() << std::endl;
  }
}

} // namespace orderbook

}
