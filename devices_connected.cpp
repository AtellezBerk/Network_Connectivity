#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    std::string arpOutput = exec("arp -a");
    std::istringstream iss(arpOutput);
    std::string line;
    int deviceCount = 0;

    while (std::getline(iss, line)) {
        deviceCount++;
        std::cout << "Device " << deviceCount << ": " << line << std::endl;
    }

    std::cout << "Total devices connected: " << deviceCount << std::endl;
    return 0;
}
