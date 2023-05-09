#include "hcp/SystemInterface.hpp"
#include "hcp/Serial.hpp"

#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    std::string port;
    if(argc != 2)
    {
        std::cout << "List of available serial ports:\n";
        std::vector<std::string> ports = HCPSerial::getSerialPorts();

        for (auto& port : ports)
            std::cout << "\t" << port << '\n';

        std::cout << "Enter the port you want to use: ";
        std::cin >> port;
    }
    else
        port = argv[1];
    port = "COM3";

    std::cout << "Starting on port " << port << '\n';
    hcpsi::startOnPort(port.c_str());

    std::cout << "Waiting for connection...\n";
    while (!hcpsi::isAlive() && !hcpsi::failed())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Stopping...\n";
    hcpsi::stop();

    return 0;
}