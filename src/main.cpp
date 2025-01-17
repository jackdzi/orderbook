#include <SocketHandler.hpp>
#include <cstdlib>
#include <fstream>

int main(void) {
  const std::string url = "wss://ws-feed.exchange.coinbase.com";

  std::string api_key;
  std::ifstream key_file("~/Documents/keys/cdp_api_key.json");
  nlohmann::json key;
  if (key_file.is_open()) {
    key_file >> key;
    api_key = key["privateKey"].get<std::string>();
  }
  key_file.close();
  std::cout << api_key << std::endl;

  orderbook::SocketHandler socketHandler(url, api_key);
  socketHandler.start();

  std::cout << "Connecting to " << url << " ...\n";
  std::cout << "Press Enter to stop.\n";
  std::cin.get();

  socketHandler.stop();
  std::cout << "Exiting.\n";

  return 0;
}
