/** Component: EventLoop (class)
 * Role: Core event orchestration engine
 * Description: EventLoop encapsulates the Linux epoll mechanism to efficiently monitor multiple file descriptors for I/O events and 
 * manages the main execution cycle of the application.
 * By registering callbacks to specific file descriptors, it allows the system to sleep until data is ready and then wake up to process events immediately.
**/

#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include "noctil/utils/UniqueFD.hpp"


namespace noctil::core {

  using EventCallback = std::function<void()>;    // Standardize type for event handlers

  class EventLoop {
  public:
    EventLoop();
    ~EventLoop();

  // - API -
    void run();
    void stop();
    void addFD(int fd, EventCallback cb);    // Registers sensor (FD) and a function to run when it has data

  private:
    bool running_ {false};
    utils::UniqueFD epoll_fd_;
    std::unordered_map<int, EventCallback> callbacks_;
  };  // close class
} // close namespace
