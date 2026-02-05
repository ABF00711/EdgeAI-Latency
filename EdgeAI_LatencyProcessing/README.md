# Edge AI Latency & Signal Processing

## Description

This project processes high-frequency sensor data to calculate the **Estimated Time of Impact (ETI)**. It receives UDP packets, computes ETI, and triggers a response if ETI is less than 10ms. The system is optimized for **low-latency processing** and works in **real-time** with real-time thread prioritization. This version of the project is built for **Linux environments** and uses **POSIX sockets** for network communication.

## Features

- Receives **high-frequency sensor data** via UDP packets using **POSIX sockets**.
- Calculates **ETI (Estimated Time of Impact)** based on velocity and distance.
- Optimized for **real-time processing** with **real-time thread priority** using `pthread_setschedparam`.
- Logs **latencies** and **jitter** (standard deviation) to a **CSV file** for analysis.
- **Real-time thread scheduling** to ensure the receiver thread has high priority in the OS.

## Requirements

### Hardware:
- **NVIDIA Jetson** (or any Linux system with a **CUDA-enabled GPU** for advanced AI tasks).
- Any system supporting **Linux** and **POSIX-compliant threading** (Ubuntu, Raspberry Pi OS, etc.).

### Software:
- **C++ Compiler** (GCC, g++)
- **Linux-based OS** (e.g., Ubuntu, NVIDIA Jetson environment).
- **pthread library** (for real-time thread management).
- **Sockets library** (POSIX socket support for UDP communication).

## Installation Instructions

1. **Clone the repository**:
    ```bash
    git clone https://github.com/ABF00711/EdgeAI-Latency.git
    ```

2. **Navigate to the project directory**:
    ```bash
    cd EdgeAI-Latency
    ```

3. **Install dependencies**:
    - Ensure you have a **C++ compiler** (e.g., GCC) installed:
      ```bash
      sudo apt-get update
      sudo apt-get install g++
      ```

4. **Compile the project**:
    - Use `g++` to compile the project:
      ```bash
      g++ -o EdgeAI_LatencyProcessing EdgeAI_LatencyProcessing.cpp udp_packet_generator.cpp udp_packet_receiver.cpp -pthread
      ```

5. **Run the project**:
    - Run the executable:
      ```bash
      ./EdgeAI_LatencyProcessing
      ```

6. **Verify Real-Time Priority**:
    - Ensure that your system allows the process to set real-time priority. On **Linux**, the process must have the correct **permissions** to set real-time priorities:
      ```bash
      sudo setcap cap_sys_nice=eip ./EdgeAI_LatencyProcessing
      ```

## Usage

1. **Start the program**:
    - The program will:
        - Continuously generate UDP packets with sensor data (velocity, distance).
        - Receive these packets, calculate **ETI**, and log **latencies** and **jitter** in a CSV file.

2. **Example Output**:
    ```plaintext
    Received data - ID: 41, Velocity: 467 m/s, Distance: 6334 meters ETI = 13563.2 ms, Latency = 344 microseconds
    RESPONSE_TRIGGER: ETI < 10ms
    ```

3. The program logs data to `latency_log.csv` and outputs **latency** and **jitter** to the console.

4. **Real-Time Thread Priority**:
    - The receiver thread is given **real-time priority** using **`pthread_setschedparam`**, which ensures high-priority handling of incoming UDP packets for **low-latency processing**.
