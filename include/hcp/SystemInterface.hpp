#ifndef HCP_SYSTEM_INTERFACE_HPP
#define HCP_SYSTEM_INTERFACE_HPP

#include <cstdint>
#include <vector>

class HCPPacket
{
public:
    HCPPacket();
    HCPPacket(uint16_t command);

    void setCommand(uint16_t command);
    uint16_t getCommand() const;

    void putByte(uint8_t byte);
    void putShort(uint16_t word);
    void putInt(uint32_t dword);
    void putFloat(float fword);

    uint8_t getByte(size_t index) const;
    uint16_t getShort(size_t index) const;
    uint32_t getInt(size_t index) const;
    float getFloat(size_t index) const;

    uint8_t getByte() const;
    uint16_t getShort() const;
    uint32_t getInt() const;
    float getFloat() const;

    void resetGetPtr();
    bool isEnd() const;
    void setGetPtr(size_t index);
    size_t size() const;
    void end();
    bool isWriteable() const;
private:
    uint16_t m_command;
    std::vector<uint8_t> m_data;

    bool m_writeable;
    mutable size_t m_getPtr;

    friend class hcpsi;
};

class hcpsi
{
public:
    static void startOnPort(const char* port);
    static const char* getStatusStr();
    static void stop();
    static bool isAlive();
    static bool failed();

    static void send(const HCPPacket& packet);
private:
    static void interfaceThread(const char* port);
};

#endif // HCP_SYSTEM_INTERFACE_HPP