#pragma once

#include <unistd.h>  // for close()
#include <utility>  // for std::exchange

namespace noctil::utils {

  class UniqueFD {
  private:
    int fd_ {-1};

  public:
    explicit UniqueFD(int fd = -1) : fd_(fd) {}
    ~UniqueFD() {
        if (isValid()) {
          close(fd_);
        }  
    } 

    // Copy semantics deleted as resource ownership cannot be shared
    UniqueFD(const UniqueFD&) = delete;	// copy constructor
    UniqueFD& operator=(const UniqueFD&) = delete;	// copy assignment operator

    // Move semantics allowed as ownership may be transferred
    UniqueFD(UniqueFD&& other) noexcept : fd_(std::exchange(other.fd_, -1))  {}
    UniqueFD& operator=(UniqueFD&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    // API
    int get() const { return fd_; }

    bool isValid() const { return fd_ >= 0; }

    explicit operator bool() const { return isValid(); }

    void reset(int new_fd = -1) {
        if (isValid()) {
            close(fd_);
        }
        fd_ = new_fd;
    }

    int release() {
        return std::exchange(fd_, -1);
    }
  };  
} 