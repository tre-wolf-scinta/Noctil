/** Component: BleScanner (class)
 * Role: Hardware Abstraction Layer (HAL) for Bluetooth
 * Description: Manages a raw HCI (Host Controller Interface) socket to listen for 
 * Bluetooth Low Energy (BLE) advertisement packets directly from the Linux Kernel.
 * It provides the file descriptor to the EventLoop and processes incoming raw bytes.
**/

#pragma once

#include "noctil/utils/UniqueFD.hpp"
#include <vector>
#include <cstdint>

namespace noctil::hal {

  class BleScanner {
  public:
    BleScanner();
    ~BleScanner();

    // -- API --

    // Returns raw FD to be registered with epoll
    int getFd() const;

    // Callback function triggered by EventLoop when data is ready
    void handleInput();

  private:
    utils::UniqueFD hci_socket_;

    // Internal helper to open socket
    int openSocket();
  };  // closes class
}  // closes ns
