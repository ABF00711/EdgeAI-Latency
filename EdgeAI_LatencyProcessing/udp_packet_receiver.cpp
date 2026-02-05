#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <mutex>
#include <unistd.h> // For close()
#include <cstring>  // For strerror()

std::mutex logFileMutex;
std::vector<long long> latencies;
static int packetCount = 0;
static std::vector<std::string> logBuffer;

struct SensorData {
    int object_id;
    float velocity;
    float distance;
    uint64_t timestamp;
};

void receive_data(int recvSock, sockaddr_in recvAddr) {
    while (true) {
        if (recvSock == -1) {
            std::cerr << "Socket is invalid or closed!" << std::endl;
            continue;
        }

        SensorData data;
        sockaddr_in senderAddr;
        socklen_t senderAddrSize = sizeof(senderAddr);

        auto start_time = std::chrono::high_resolution_clock::now();
        int bytesReceived = recvfrom(recvSock, (char*)&data, sizeof(data), 0, (struct sockaddr*)&senderAddr, &senderAddrSize);

        if (bytesReceived == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;  // If no data is available, continue
            }
            std::cerr << "Failed to receive packet: " << strerror(errno) << std::endl;
            continue;
        }

        if (data.velocity > 350) {
            float etime = data.distance / data.velocity;
            float etime_ms = etime * 1000;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
            latencies.push_back(latency);

            std::cout << "Received data - ID: " << data.object_id << ", Velocity: "
                << data.velocity << " m/s, Distance: " << data.distance << " meters\n";
            std::cout << "ETI = " << etime_ms << " ms, Latency = " << latency << " microseconds\n";

            logBuffer.push_back("Received data - ID: " + std::to_string(data.object_id) + ", Velocity: "
                + std::to_string(data.velocity) + " m/s, Distance: " + std::to_string(data.distance)
                + " meters, ETI = " + std::to_string(etime_ms) + " ms, Latency = " + std::to_string(latency) + " microseconds\n");

            if (etime_ms < 10) {
                std::cout << "RESPONSE_TRIGGER: ETI < 10ms\n";
            }

            if (latencies.size() >= 100) {
                double mean = 0;
                for (auto lat : latencies) {
                    mean += lat;
                }
                mean /= latencies.size();

                double variance = 0;
                for (auto lat : latencies) {
                    variance += std::pow(lat - mean, 2);
                }
                variance /= latencies.size();

                double jitter = std::sqrt(variance);
                std::cout << "Jitter (Standard Deviation): " << jitter << " microseconds\n";

                logBuffer.push_back("Jitter (Standard Deviation): " + std::to_string(jitter) + " microseconds ");

                latencies.clear();
            }
        }
    }

    std::ofstream logFile("latency_log.csv", std::ios::app);
    for (const auto& logEntry : logBuffer) {
        logFile << logEntry;
    }

    logBuffer.clear();
    logFile.close();
}
