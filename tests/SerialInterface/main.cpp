#include "hcp/SystemInterface.hpp"
#include "hcp/Serial.hpp"

#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    glfwInit();

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

    std::cout << "Starting on port " << port << '\n';
    hcpsi::startOnPort(port.c_str());

    std::cout << "Waiting for connection...\n";
    while (!hcpsi::isAlive() && !hcpsi::failed())
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Getting variables...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    std::vector<std::string> vars = hcpsi::getVariables();

    std::cout << "Variables:\n";

    for (auto& var : vars)
    {
        hcpsi::Var& v = hcpsi::getVariable(var.c_str());
        std::cout << var << ": \n";
        std::cout << "\tType: " << v.type << '\n';

        switch(v.type)
        {
        case hcpsi::Type::BYTE:
            std::cout << "\tValue: " << (int)v.byte << '\n';
            break;
        case hcpsi::Type::SHORT:
            std::cout << "\tValue: " << v.word << '\n';
            break;
        case hcpsi::Type::INT:
            std::cout << "\tValue: " << v.dword << '\n';
            break;
        case hcpsi::Type::FLOAT:
            std::cout << "\tValue: " << v.fword << '\n';
            break;
        case hcpsi::Type::STRING:
            std::cout << "\tValue: " << v.data << '\n';
            break;
        }
    }

    std::cout << "Stopping...\n";
    hcpsi::stop();

    return 0;
}