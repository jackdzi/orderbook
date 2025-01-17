#include <SocketHandler.hpp>
#include <cstdlib>

int main(void) {
  const std::string url = "wss://ws-feed.exchange.coinbase.com";

   orderbook::SocketHandler socketHandler(url);
   socketHandler.start();

   std::cout << "Connecting to " << url << " ...\n";
   std::cout << "Press Enter to stop.\n";
   std::cin.get();

   socketHandler.stop();
   std::cout << "Exiting.\n";

  return 0;
}
