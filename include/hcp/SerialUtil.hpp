#ifndef HCP_SERIAL_UTIL_HPP
#define HCP_SERIAL_UTIL_HPP

#include <vector>
#include <string>

class hcpserial
{
public:
    static std::vector<std::string> getSerialPorts();
};

#endif // HCP_SERIAL_UTIL_HPP