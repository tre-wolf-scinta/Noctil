#include <iostream>
#include <format>
#include "noctil/core/EventLoop.hpp"

int main() {

  try {
    noctil::core::EventLoop loop;

    std::cout << std::format("Noctil engine initialized. epoll_create1() successful.\n");
  } catch (const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
  }
return 0;
}