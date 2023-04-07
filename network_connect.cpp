#include <chrono>
#include <vector>
#include <netinet/ip_icmp.h> // for ping functions
#include <netdb.h>           // for getaddrinfo function
#include <iostream>          // for console output
#include <string.h>          // for memset function
#include <unistd.h>          // for getpid function
#include <cstdio>            // for std::endl
#include <arpa/inet.h>       // for inet_ntop function


std::pair<int, double> tcpTest(const char** hosts, int numHosts) {
    std::vector<double> latencies;
    int successfulConnections = 0;
    double latencySum = 0.0;

    for (int i = 0; i < numHosts; i++) {
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        int getaddrinfoResult = getaddrinfo(hosts[i], "443", &hints, &res);
        if (getaddrinfoResult != 0) {
            std::cout << "getaddrinfo failed for host " << hosts[i] << ": " << gai_strerror(getaddrinfoResult) << std::endl;
            continue;
        }

        int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) {
            std::cout << "socket creation failed for host " << hosts[i] << ": " << strerror(errno) << std::endl;
            continue;
        }

        auto start = std::chrono::high_resolution_clock::now();
        int connectResult = connect(sock, res->ai_addr, res->ai_addrlen);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsedTime = end - start;

        if (connectResult == 0) {
            successfulConnections++;
            double elapsedTimeMs = elapsedTime.count();
            latencySum += elapsedTimeMs;
            latencies.push_back(elapsedTimeMs);
            std::cout << "TCP connection to " << hosts[i] << " on port 443 was successful. Time taken: " << elapsedTimeMs << " ms" << std::endl;
        } else {
            std::cout << "TCP connection to " << hosts[i] << " on port 443 failed: " << strerror(errno) << std::endl;
        }

        close(sock);
    }

    double avgLatency = 0.0;
    if (!latencies.empty()) {
        double sum = 0;
        for (const auto& latency : latencies) {
            sum += latency;
        }
        avgLatency = sum / latencies.size();
        std::cout << "Average latency: " << avgLatency << " ms" << std::endl;
    }

    std::cout << "Successful connections: " << successfulConnections << " out of " << numHosts << std::endl;

    return std::make_pair(successfulConnections, latencySum);
}


void evaluateConnectionQuality(int numHosts, int successfulConnections, double avgLatency) {
    double successRate = (double)successfulConnections / numHosts * 100;

    if (successRate < 50) {
        std::cout << "Network connection quality: Terrible" << std::endl;
    } else if (successRate < 75) {
        std::cout << "Network connection quality: Bad" << std::endl;
    } else if (avgLatency > 200) {
        std::cout << "Network connection quality: OK" << std::endl;
    } else if (avgLatency > 100) {
        std::cout << "Network connection quality: Good" << std::endl;
    } else {
        std::cout << "Network connection quality: Great" << std::endl;
    }
}


int main() {
    const char* hosts[] = { "8.8.8.8", "8.8.4.4", "1.1.1.1", "208.67.222.222", "208.67.220.220" };
    int numHosts = sizeof(hosts) / sizeof(hosts[0]);

    auto testResults = tcpTest(hosts, numHosts);
    int successfulConnections = testResults.first;
    double latencySum = testResults.second;

    // Calculate the average latency
    double avgLatency = successfulConnections > 0 ? latencySum / successfulConnections : 0.0;

    evaluateConnectionQuality(numHosts, successfulConnections, avgLatency);

    return 0;
}

