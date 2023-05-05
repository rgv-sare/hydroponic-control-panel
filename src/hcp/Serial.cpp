#include "hcp/Serial.hpp"

HCPLogger HCPSerial::s_logger("Serial");

HCPSerial::Timeout::Timeout(
    uint32_t perByteTimeout_, 
    uint32_t readTimeoutConstant_,  
    uint32_t readTimeoutMultiplier_,
    uint32_t writeTimeoutConstant_,
    uint32_t writeTimeoutMultiplier_) :
    perByteTimeout(perByteTimeout_),
    readTimeoutConstant(readTimeoutConstant_),
    readTimeoutMultiplier(readTimeoutMultiplier_),
    writeTimeoutConstant(writeTimeoutConstant_),
    writeTimeoutMultiplier(writeTimeoutMultiplier_)
{
}

HCPSerial::Timeout HCPSerial::Timeout::fromTimeout(uint32_t timeout_)
{
    return Timeout(0xFFFFFFF, timeout_, 0, timeout_, 0);
}

HCPSerial::HCPSerial(const char* port_, uint32_t baudRate_, Parity parity_, StopBits stopBits_, Timeout timeout_) :
    m_baudRate(baudRate_),
    m_parity(parity_),
    m_stopBits(stopBits_),
    m_timeout(timeout_),
    m_impl(nullptr),
    m_open(false)
{
    if(port_) strcpy_s(m_port, sizeof(m_port), port_);
    else m_port[0] = '\0';
    impl_init();
}

HCPSerial::~HCPSerial()
{
    close();
    impl_destory();
}

bool HCPSerial::isOpen() const
{
    return m_open;
}

int HCPSerial::available() const
{
    return (int) m_readBuffer.size();
}

void HCPSerial::write(const uint8_t* data, size_t len)
{
    // Append data to write buffer
    m_writeBuffer.insert(m_writeBuffer.end(), data, data + len);
}

int HCPSerial::read(uint8_t* data, size_t len)
{
    // Read data from read buffer
    size_t readLen = std::min(len, m_readBuffer.size());
    memcpy(data, m_readBuffer.data(), readLen);
    m_readBuffer.erase(m_readBuffer.begin(), m_readBuffer.begin() + readLen);
    return (int) readLen;
}

const char* HCPSerial::getPort() const
{
    return m_port;
}

uint32_t HCPSerial::getBaudRate() const
{
    return m_baudRate;
}

HCPSerial::Parity HCPSerial::getParity() const
{
    return m_parity;
}

HCPSerial::StopBits HCPSerial::getStopBits() const
{
    return m_stopBits;
}

HCPSerial::Timeout HCPSerial::getTimeout() const
{
    return m_timeout;
}

void HCPSerial::setBaudRate(uint32_t baudRate)
{
    m_baudRate = baudRate;
    impl_setBaudRate(baudRate);
}

void HCPSerial::setParity(Parity parity)
{
    m_parity = parity;
    impl_setParity(parity);
}

void HCPSerial::setStopBits(StopBits stopBits)
{
    m_stopBits = stopBits;
    impl_setStopBits(stopBits);
}

void HCPSerial::setTimeout(Timeout timeout)
{
    m_timeout = timeout;
    impl_setTimeout(timeout);
}