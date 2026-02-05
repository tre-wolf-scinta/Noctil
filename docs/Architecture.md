# \# System Architecture \& Event Loop

## 

## \## Core Components



1. &nbsp; HCI Socket (User-Space Endpoint): Acts as the bridge between the hardware and the user-space application.



* Mechanism: The physical bluetooth antenna on the Raspberry Pi captures radio waves which are decoded into digital bits by the hardware controller. The Linux kernel driver buffers these bits and exposes them to the Noctil program via this socket file descriptor.



2. E poll Instance (Event Monitor): A scalable I/O event notification system.



* Mechanism: Instead of busy-waiting, the application uses epoll to sleep until the Kernel notifies that the HCI socket is readable (contains new data). This ensures minimal CPU usage and efficient power consumption.



3. &nbsp;Sound Interface (ALSA Subsystem): The output handler for the Noctil system.



* Mechanism: Manages the file descriptor connected to the ALSA (Advanced Linux Sound Architecture) system. When application logic calculates that the user is within target proximity, it writes synthesized speech data to this interface to trigger audio output.



\## System Workflow



\### Phase 1: Initialization \& Resource Acquisition



1. Startup: Main program initializes
2. Socket Creation: The program requests a raw HCI socket from the Kernel to communicate with the Bluetooth controller.
3. RAII Encapsulation: The socket's raw file descriptor is immediately wrapped in a UniqueFD object to guarantee exclusive ownership and resource cleanup.
4. Monitor Creation: The program instantiates an epoll object
5. Monitor Encapsulation: The epoll file descriptor is wrapped in a UniqueFD  object.
6. Registration: The program registers the HCI socket with the epoll instance (via epoll\_ctl), instructing the Kernel to wake the process whenever new Bluetooth data arrives on that socket.

   ### Phase 2: Main Event Loop
   
7. Wait State: The program calls epoll\_wait(), execution suspends and the process enters a low-power sleep state.
8. Signal Capture: External BLE beacon broadcasts a signal. The antenna on the Raspberry Pi device captures the radio waves.
9. Kernel Buffering: The device controller converts the signal into digital packets. The Linux Kernel receives an interrupt and pushes these packets into the HCI socket's Kernel buffer.
10. Wake-Up Event: The epoll instance detects that the HCI socket has transitioned into a readable state and wakes the main program.
11. Data Ingestion: The main program wakes up. It uses the raw file descriptor managed by UniqueFD to perform a read() system call, copying the data from the Kernel's buffer into the application's memory.
12. Logic Output: The program parses the RSSI (signal strength) to approximate distance. If the threshold is met, the program writes audio data to the sound interface.
13. Loop: The program returns to step 1 and suspends execution again.   
14. 
