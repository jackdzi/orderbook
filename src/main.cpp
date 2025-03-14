#include "OrderBook.hpp"
#include <SocketHandler.hpp>
#include <cmath>
#include <cstdlib>
#include <thread>

int main(void) {
  const std::string url = "wss://ws-feed.exchange.coinbase.com";

  orderbook::OrderBook orderbook;
  std::atomic<bool> running(true);
  orderbook::SocketHandler socketHandler(url);

  std::thread connection([&]() {
    socketHandler.start();
    std::cout << "Connecting to " << url << " ...\n";
    while (running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  });

  std::thread matchingThread(
      [&]() { orderbook.matchingEngineThread(running); });

  std::cout << "Press enter to stop" << std::endl;
  std::string input;
  std::getline(std::cin, input);

  socketHandler.stop();

  connection.join();
  matchingThread.join();
  return 0;
}
