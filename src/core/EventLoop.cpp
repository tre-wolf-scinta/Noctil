#include "noctil/core/EventLoop.hpp"
#include <sys/epoll.h>
#include <stdexcept>
#include <vector>
#include <cerrno>

namespace noctil::core {

  EventLoop::EventLoop() {
    int raw_fd = epoll_create1(EPOLL_CLOEXEC);  // instantiates new epoll instance and handles closing if new process spawns

    if (raw_fd < 0) {
      throw std::runtime_error("Failed to create epoll instance");
    } // if

    epoll_fd_.reset(raw_fd);  // moves ownership to RAII wrapper (UniqueFD)
  } // closes constructor

  EventLoop::~EventLoop() { stop(); }  

  void EventLoop::run() {
    running_ = true;
    const int max_events {16};

    std::vector<struct epoll_event> events(max_events);

    while (running_) {
      int nfds = epoll_wait(epoll_fd_.get(), events.data(), max_events, -1);

      // epoll_wait() returns -1 when error occurs  
      if (nfds < 0) {
        if (errno == EINTR) continue;    // Error is signal interrupt, try again
        throw std::runtime_error("epoll_wait() failed");
      } // if 

      // Iterate only through events that actually happen (nfds)
      for (int i = 0; i < nfds; ++i) {
      // Fetch file descriptor that triggered event
      int fd = events[i].data.fd;

      // Look up the function associated with the file descriptor (callbacks_)
      if (callbacks_.find(fd) != callbacks_.end()) {
        // call function registered with the fd
        callbacks_[fd]();
      } // if
      } // for
    }  // while
  }  // closes run()

  void EventLoop::stop() { running_ = false; }

  void EventLoop::addFD(int fd, EventCallback cb) {
    struct epoll_event event;  // configuration struct

    event.events = EPOLLIN;  // sets flag to wake up on read when data from sensor is available
    event.data.fd = fd;  // Tells us which sensor triggered the event

    if (epoll_ctl(epoll_fd_.get(), EPOLL_CTL_ADD, fd, &event) < 0) {
      throw std::runtime_error("Failed to add file descriptor to epoll instance");
    }  

    callbacks_[fd] = cb;  
  }  // closes addFD()

} // closes ns