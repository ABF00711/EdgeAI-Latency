#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "udp_packet_generator.h"
#include "udp_packet_receiver.h"

int main() {
    // Create sending socket
    int sendSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendSock == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5005);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Create receiving socket
    int recvSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (recvSock == -1) {
        std::cerr << "Receiver socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(5005);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind receiving socket to address
    if (bind(recvSock, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(recvSock);  // Clean up socket
        return 1;
    }

    // Set socket to non-blocking mode
    int flags = fcntl(recvSock, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
        close(recvSock);
        return 1;
    }
    if (fcntl(recvSock, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Failed to set non-blocking mode: " << strerror(errno) << std::endl;
        close(recvSock);
        return 1;
    }

    // Start UDP packet generator and receiver threads
    std::thread generator(generate_data, sendSock, serverAddr);
    std::thread receiver(receive_data, recvSock, recvAddr);

    // Set receiver thread to real-time priority
    struct sched_param schedParam;
    schedParam.sched_priority = 99;  // High priority for real-time scheduling
    if (pthread_setschedparam(receiver.native_handle(), SCHED_FIFO, &schedParam) != 0) {
        std::cerr << "Failed to set thread priority: " << strerror(errno) << std::endl;
        close(recvSock);
        return 1;
    }

    // Wait for threads to finish
    generator.join();
    receiver.join();

    // Clean up
    close(sendSock);
    close(recvSock);
    return 0;
}
