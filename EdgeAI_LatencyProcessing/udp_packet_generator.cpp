#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "udp_packet_generator.h"

void generate_data(int sock, sockaddr_in serverAddr) {
    while (true) {
        SensorData data;
        data.object_id = rand() % 100;
        data.velocity = static_cast<float>(rand() % 1000);
        data.distance = static_cast<float>(rand() % 10000);
        data.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());

        int sent = sendto(sock, (char*)&data, sizeof(data), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (sent == -1) {
            std::cerr << "Failed to send packet: " << strerror(errno) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }
}
