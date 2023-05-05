#ifndef HCP_SERIAL_UTIL_HPP
#define HCP_SERIAL_UTIL_HPP

#include "Logger.hpp"

#include <vector>
#include <string>

#define IMPL_FUNC // Marker for functions that are implemented in the platform-specific source file

class HCPSerial
{
public:
    struct Timeout
    {
        uint32_t perByteTimeout;
        uint32_t readTimeoutConstant;
        uint32_t readTimeoutMultiplier;
        uint32_t writeTimeoutConstant;
        uint32_t writeTimeoutMultiplier;

        Timeout(
        uint32_t perByteTimeout = 0,
        uint32_t readTimeoutConstant = 0,
        uint32_t readTimeoutMultiplier = 0,
        uint32_t writeTimeoutConstant = 0,
        uint32_t writeTimeoutMultiplier = 0);

        static Timeout fromTimeout(uint32_t timeout);
    };
    enum Parity
    {
        PARITY_NONE,
        PARITY_ODD,
        PARITY_EVEN
    };
    enum StopBits
    {
        STOPBITS_ONE = 1,
        STOPBITS_ONE_POINT_FIVE = 3,
        STOPBITS_TWO = 2
    };

    HCPSerial(const char* port = nullptr,
    uint32_t baudRate = 9600,
    Parity parity = Parity::PARITY_NONE,
    StopBits stopBits = StopBits::STOPBITS_ONE, 
    Timeout timeout = Timeout());

    HCPSerial(const HCPSerial&) = delete;
    HCPSerial& operator=(const HCPSerial&) = delete;

    ~HCPSerial();

    void IMPL_FUNC begin();
    void IMPL_FUNC close();

    bool isOpen() const;

    int available() const;
    void write(const uint8_t* data, size_t len);
    int read(uint8_t* data, size_t len);

    void IMPL_FUNC poll();
    // Returns true if the output buffer was flushed
    bool IMPL_FUNC flushOuput();

    const char* getPort() const;
    uint32_t getBaudRate() const;
    Parity getParity() const;
    StopBits getStopBits() const;
    Timeout getTimeout() const;

    void setBaudRate(uint32_t baudRate);
    void setParity(Parity parity);
    void setStopBits(StopBits stopBits);
    void setTimeout(Timeout timeout);

    static std::vector<std::string> IMPL_FUNC getSerialPorts();
private:
    static HCPLogger s_logger;

    char m_port[128];
    uint32_t m_baudRate;
    Parity m_parity;
    StopBits m_stopBits;
    Timeout m_timeout;

    std::vector<uint8_t> m_readBuffer;
    std::vector<uint8_t> m_writeBuffer;

    bool m_open;

    void* m_impl; // platform specific implementation struct
    void IMPL_FUNC impl_init();
    void IMPL_FUNC impl_setBaudRate(uint32_t baudRate);
    void IMPL_FUNC impl_setParity(Parity parity);
    void IMPL_FUNC impl_setStopBits(StopBits stopBits);
    void IMPL_FUNC impl_setTimeout(Timeout timeout);
    void IMPL_FUNC impl_destory();
};

#endif // HCP_SERIAL_UTIL_HPP