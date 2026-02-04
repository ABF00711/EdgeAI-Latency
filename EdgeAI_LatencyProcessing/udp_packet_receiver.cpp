#include <winsock2.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <mutex>

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

void receive_data(SOCKET recvSock, sockaddr_in recvAddr) {

	std::ofstream logFile("latency_log.csv", std::ios::app);
    while (true) {
        SensorData data;
        sockaddr_in senderAddr;
        int senderAddrSize = sizeof(senderAddr);


		auto start_time = std::chrono::high_resolution_clock::now();
        int bytesReceived = recvfrom(recvSock, (char*)&data, sizeof(data), 0, (sockaddr*)&senderAddr, &senderAddrSize);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Failed to receive packet: " << WSAGetLastError() << std::endl;
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

                logFile << "Jitter (Standard Deviation): " << jitter << " microseconds " << "\n";
                logBuffer.push_back("Jitter (Standard Deviation): " + std::to_string(jitter) + " microseconds ");

                latencies.clear();
            }
        }

        if (logBuffer.size() >= 100) {
            std::lock_guard<std::mutex> lock(logFileMutex);
            std::ofstream logFile("latency_log.csv", std::ios::app);
            for (const auto& logEntry : logBuffer) {
                logFile << logEntry;
            }
            logBuffer.clear();
        }
    }

    logFile.close();
}