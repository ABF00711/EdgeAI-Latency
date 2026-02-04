#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <atomic>
#include "udp_packet_generator.h"
#include "udp_packet_receiver.h"

int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    SOCKET sendSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sendSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5005);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    SOCKET recvSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (recvSock == INVALID_SOCKET) {
        std::cerr << "Receiver socket creation failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(5005);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(recvSock, (sockaddr*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }

    u_long mode = 1;
    if (ioctlsocket(recvSock, FIONBIO, &mode) == SOCKET_ERROR) {
        std::cerr << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        return 1;
    }

    std::thread generator(generate_data, sendSock, serverAddr);
    std::thread receiver(receive_data, recvSock, recvAddr);

    generator.join();
    receiver.join();

    closesocket(sendSock);
    closesocket(recvSock); 
    WSACleanup();           

    return 0;
}
