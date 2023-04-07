#include <sys/socket.h>      // for socket programming
#include <netinet/ip_icmp.h> // for ping functions
#include <netdb.h>           // for getaddrinfo function
#include <iostream>          // for console output
#include <string.h>          // for memset function
#include <unistd.h>          // for getpid function
#include <cstdio>            // for std::endl


int ping(const char** hosts, int numHosts) {
    int numConnected = 0;

    for (int i = 0; i < numHosts; i++) {
        struct addrinfo hints, * res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_ICMP;

        if (getaddrinfo(hosts[i], nullptr, &hints, &res) != 0) {
            std::cout << "getaddrinfo failed!" << std::endl;
            continue;
        }

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        if (sock < 0) {
            std::cout << "socket creation failed!" << std::endl;
            continue;
        }

        const int packetSize = 32; // size of the packet payload
        char packet[packetSize];
        memset(packet, 0, packetSize);
        struct icmp* icmpHeader = (struct icmp*)packet;
        icmpHeader->icmp_type = ICMP_ECHO;
        icmpHeader->icmp_code = 0;
        icmpHeader->icmp_id = htons(getpid());
        icmpHeader->icmp_seq = htons(i);
        char* payload = packet + sizeof(struct icmp);
        memset(payload, 'A', packetSize - sizeof(struct icmp));

        struct sockaddr_in destSockAddr;
        memset(&destSockAddr, 0, sizeof(destSockAddr));
        destSockAddr.sin_family = AF_INET;
        destSockAddr.sin_addr.s_addr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;

        if (sendto(sock, packet, packetSize, 0, (struct sockaddr*)&destSockAddr, sizeof(destSockAddr)) == -1) {
            std::cout << "sendto failed!" << std::endl;
            close(sock);
            continue;
        }

        char recvBuffer[packetSize + sizeof(struct ip) + sizeof(struct icmp)]; // buffer for receiving reply
        struct sockaddr_in fromSockAddr;
        socklen_t fromSockAddrLen = sizeof(fromSockAddr);
        ssize_t numBytesRecv = recvfrom(sock, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr*)&fromSockAddr, &fromSockAddrLen);
        if (numBytesRecv == -1) {
            std::cout << "recvfrom failed!" << std::endl;
            close(sock);
            continue;
        }

        struct ip* ipHeader = (struct ip*)recvBuffer;
        struct icmp* icmpReply = (struct icmp*)(recvBuffer + (ipHeader->ip_hl << 2));
        if (icmpReply->icmp_type == ICMP_ECHOREPLY) {
            numConnected++;
        }

        close(sock);
    }

    return numConnected;
}


int main() {
    const char* hosts[] = { "8.8.8.8", "8.8.4.4", "1.1.1.1" };
    int numHosts = sizeof(hosts) / sizeof(hosts[0]);
    int numConnected = ping(hosts, numHosts);
    std::cout << "Number of connected hosts: " << numConnected << std::endl;

    // Estimate the number of users based on the number of connected hosts
    // You can adjust this estimate based on your knowledge of the network topology and usage patterns
    int numUsers = (numConnected > 0) ? (numConnected + 1) : 0;
    std::cout << "Number of users: " << numUsers << std::endl;

    return 0;
}
