#include <winsock2.h> // for socket programming
#include <iphlpapi.h> // for ping functions
#include <iostream>   // for console output

int ping(const char** hosts, int numHosts) {
    int numConnected = 0;

    for (int i = 0; i < numHosts; i++) {
        in_addr destAddr;
        if (inet_pton(AF_INET, hosts[i], &destAddr) != 1) {
            std::cout << "invalid destination IP address!" << std::endl;
            continue;
        }

        sockaddr_in destSockAddr;
        destSockAddr.sin_family = AF_INET;
        destSockAddr.sin_addr = destAddr;
        destSockAddr.sin_port = 0;

        const int packetSize = 32; // size of the packet payload
        char packet[packetSize];
        memset(packet, 0, packetSize);
        ICMP_ECHO_REPLY* reply = (ICMP_ECHO_REPLY*)packet;
        reply->Data = "network connectivity checker by ChatGPT"; // custom payload data

        if (sendto(sock, packet, packetSize, 0, (sockaddr*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR) {
            std::cout << "sendto failed!" << std::endl;
            continue;
        }

        char recvBuffer[packetSize + sizeof(sockaddr_in) + sizeof(DWORD)]; // buffer for receiving reply
        sockaddr_in fromSockAddr;
        int fromSockAddrLen = sizeof(fromSockAddr);
        DWORD numBytesRecv = recvfrom(sock, recvBuffer, sizeof(recvBuffer), 0, (sockaddr*)&fromSockAddr, &fromSockAddrLen);
        if (numBytesRecv == SOCKET_ERROR) {
            std::cout << "recvfrom failed!" << std::endl;
            continue;
        }

        ICMP_ECHO_REPLY* recvReply = (ICMP_ECHO_REPLY*)recvBuffer;
        if (recvReply->Status == IP_SUCCESS) {
            numConnected++;
        }
    }

    return numConnected;
}


const char* hosts[] = { "8.8.8.8", "8.8.4.4", "1.1.1.1" };
int numHosts = sizeof(hosts) / sizeof(hosts[0]);
int numConnected = ping(hosts, numHosts);
std::cout << "Number of connected hosts: " << numConnected << std::endl;
