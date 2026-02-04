# Edge AI Latency & Signal Processing

## Description

This project processes high-frequency sensor data to calculate the **Estimated Time of Impact (ETI)**. It receives UDP packets, computes ETI, and triggers a response if ETI is less than 10ms. The system is optimized for low-latency processing.

## Features

- Receives high-frequency sensor data via UDP packets.
- Calculates ETI (time to impact) based on velocity and distance.
- Logs **latencies** and **jitter** to a CSV file.
- Trigger response if ETI < 10ms.

## Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/ABF00711/EdgeAI-Latency.git
    ```

2. Open the project in **Visual Studio** (or any C++ IDE).

3. Build and run the project.

## Usage

1. Run the program.
2. The program will:
   - Process incoming UDP packets.
   - Calculate **ETI**.
   - Log **latency** and **jitter** to `latency_log.csv`.