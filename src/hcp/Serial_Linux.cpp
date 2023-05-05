#ifdef __linux__
#include "hcp/Serial.hpp"

struct ImplLinux
{
    int m_handle;
};

#define implGet() (*((ImplLinux*) m_impl))

void HCPSerial::begin()
{

}

void HCPSerial::close()
{

}

int HCPSerial::available() const
{
    return 0;
}

void HCPSerial::write(const uint8_t* data, size_t len)
{

}

int HCPSerial::read(uint8_t* data, size_t len)
{
    return 0;
}

void HCPSerial::flush()
{

}

void HCPSerial::flushInput()
{

}

void HCPSerial::flushOuput()
{

}

void HCPSerial::impl_init()
{
    m_impl = new ImplLinux();
}

void HCPSerial::impl_setBaudRate(uint32_t baudRate)
{

}

void HCPSerial::impl_setParity(Parity parity)
{

}

void HCPSerial::impl_setStopBits(StopBits stopBits)
{

}

void HCPSerial::impl_setTimeout(Timeout timeout)
{

}

void HCPSerial::impl_destory()
{
    delete (ImplLinux*) m_impl;
}

std::vector<std::string> HCPSerial::getSerialPorts()
{
    std::vector<std::string> ports;

    return ports;
}

#endif // Linux