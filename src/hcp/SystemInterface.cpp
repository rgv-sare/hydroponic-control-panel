#include "hcp/SystemInterface.hpp"

#include "Logger.hpp"

#include <thread>
#include <memory>

union endian_teller
{
    uint32_t i;
    char c[4];
};

static endian_teller i_bint = {0x01020304};
static bool i_isSmallEndian = i_bint.c[0] == 4;

static inline uint8_t i_nibbleToHex(uint8_t nibble)
{
    // (0 - 9) = (0 - 9) and (10 - 15) = (A - F)
    return nibble < 10 ? 48 + nibble : 55 + nibble;
}

static inline uint8_t i_nibbleFromHex(uint8_t hex)
{
    return 65 <= hex ? hex - 55 : hex - 48;
}

static inline void i_hexNibblesByte(uint8_t byte, uint8_t* dst)
{
    dst[0] = i_nibbleToHex(0xF0 & byte);
    dst[1] = i_nibbleToHex(0x0F & byte);
}

static inline void i_hexNibblesWord(uint16_t word, uint8_t* dst)
{
    uint8_t* wordb = (uint8_t*) &word;

    if(i_isSmallEndian)
    {
        i_hexNibblesByte(wordb[0], dst);
        i_hexNibblesByte(wordb[1], dst + 2);
    }
    else
    {
        i_hexNibblesByte(wordb[1], dst);
        i_hexNibblesByte(wordb[0], dst + 2);
    }
}

static inline void i_hexNibblesDWord(uint32_t dword, uint8_t* dst)
{
    uint8_t* dwordb = (uint8_t*) &dword;

    if(i_isSmallEndian)
    {
        i_hexNibblesByte(dwordb[0], dst);
        i_hexNibblesByte(dwordb[1], dst + 2);
        i_hexNibblesByte(dwordb[2], dst + 4);
        i_hexNibblesByte(dwordb[3], dst + 6);
    }
    else
    {
        i_hexNibblesByte(dwordb[3], dst);
        i_hexNibblesByte(dwordb[2], dst + 2);
        i_hexNibblesByte(dwordb[1], dst + 4);
        i_hexNibblesByte(dwordb[0], dst + 6);
    }
}

static inline void i_hexNibblesFWord(float fword, uint8_t* dst)
{
    uint32_t dword = *(uint32_t*) &fword;
    i_hexNibblesDWord(dword, dst);
}

static inline uint8_t i_byteFromNibbles(const uint8_t* nibbles)
{
    uint8_t b0n1 = i_nibbleFromHex(nibbles[0]);
    uint8_t b0n0 = i_nibbleFromHex(nibbles[1]);
    return (b0n1 << 4) + b0n0;
}

static inline uint16_t i_wordFromNibbles(const uint8_t* nibbles)
{
    uint8_t b1n1 = i_nibbleFromHex(nibbles[0]);
    uint8_t b1n0 = i_nibbleFromHex(nibbles[1]);
    uint8_t b0n1 = i_nibbleFromHex(nibbles[2]);
    uint8_t b0n0 = i_nibbleFromHex(nibbles[3]);

    return (b1n1 << 12) + (b1n0 << 8) + (b0n1 << 4) + b0n0;
}

static inline uint32_t i_dwordFromNibbles(const uint8_t* nibbles)
{
    uint8_t b3n1 = i_nibbleFromHex(nibbles[0]);
    uint8_t b3n0 = i_nibbleFromHex(nibbles[1]);
    uint8_t b2n1 = i_nibbleFromHex(nibbles[2]);
    uint8_t b2n0 = i_nibbleFromHex(nibbles[3]);
    uint8_t b1n1 = i_nibbleFromHex(nibbles[4]);
    uint8_t b1n0 = i_nibbleFromHex(nibbles[5]);
    uint8_t b0n1 = i_nibbleFromHex(nibbles[6]);
    uint8_t b0n0 = i_nibbleFromHex(nibbles[7]);

    return (b3n1 << 28) + (b3n0 << 24) + (b2n1 << 20) + (b2n0 << 16) + (b1n1 << 12) + (b1n0 << 8) + (b0n1 << 4) + b0n0;
}

static inline float i_fwordFromNibbles(const uint8_t* nibbles)
{
    uint32_t dword = i_dwordFromNibbles(nibbles);

    return *(float*) &dword;
}

HCPPacket::HCPPacket()
    : HCPPacket(128)
{
    
}

HCPPacket::HCPPacket(uint16_t command) :
    m_command(command),
    m_getPtr(0),
    m_writeable(true)
{
    m_data.push_back((uint8_t) m_command);
}

void HCPPacket::setCommand(uint16_t command)
{
    m_command = command;
    m_data[0] = (uint8_t) m_command;
}

uint16_t HCPPacket::getCommand() const
{
    return m_command;
}

void HCPPacket::putByte(uint8_t byte)
{
    uint8_t hex[2];
    i_hexNibblesByte(byte, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);
}

void HCPPacket::putShort(uint16_t word)
{
    uint8_t hex[4];
    i_hexNibblesWord(word, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);
    m_data.push_back(hex[2]);
    m_data.push_back(hex[3]);
}

void HCPPacket::putInt(uint32_t dword)
{
    uint8_t hex[8];
    i_hexNibblesDWord(dword, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);
    m_data.push_back(hex[2]);
    m_data.push_back(hex[3]);
    m_data.push_back(hex[4]);
    m_data.push_back(hex[5]);
    m_data.push_back(hex[6]);
    m_data.push_back(hex[7]);
}

void HCPPacket::putFloat(float fword)
{
    uint8_t hex[8];
    i_hexNibblesFWord(fword, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);
    m_data.push_back(hex[2]);
    m_data.push_back(hex[3]);
    m_data.push_back(hex[4]);
    m_data.push_back(hex[5]);
    m_data.push_back(hex[6]);
    m_data.push_back(hex[7]);
}

uint8_t HCPPacket::getByte(size_t index) const
{
    return i_byteFromNibbles(&m_data[index * 2]);
}

uint16_t HCPPacket::getShort(size_t index) const
{
    return i_wordFromNibbles(&m_data[index * 4]);
}

uint32_t HCPPacket::getInt(size_t index) const
{
    return i_dwordFromNibbles(&m_data[index * 8]);
}

float HCPPacket::getFloat(size_t index) const
{
    return i_fwordFromNibbles(&m_data[index * 8]);
}

uint8_t HCPPacket::getByte() const
{
    uint8_t byte = getByte(m_getPtr);
    m_getPtr += 2;
    return byte;
}

uint16_t HCPPacket::getShort() const
{
    uint16_t word = getShort(m_getPtr);
    m_getPtr += 4;
    return word;
}

uint32_t HCPPacket::getInt() const
{
    uint32_t dword = getInt(m_getPtr);
    m_getPtr += 8;
    return dword;
}

float HCPPacket::getFloat() const
{
    float fword = getFloat(m_getPtr);
    m_getPtr += 8;
    return fword;
}

void HCPPacket::resetGetPtr()
{
    m_getPtr = 0;
}

bool HCPPacket::isEnd() const
{
    return m_getPtr >= m_data.size();
}

void HCPPacket::setGetPtr(size_t index)
{
    m_getPtr = index;
}

size_t HCPPacket::size() const
{
    return m_data.size();
}

void HCPPacket::end()
{
    m_writeable = false;
    m_data.push_back(128);
}

bool HCPPacket::isWriteable() const
{
    return m_writeable;
}

// ----------- hcpsi ------------ //

#include "hcp/Serial.hpp"

#include <queue>

static HCPLogger i_logger("SystemInterface");

static std::unique_ptr<HCPSerial> i_serial;
static std::string i_statusStr = "Not started";

static bool i_isAlive = false;
static bool i_failed = false;

static std::queue<HCPPacket> i_sendQueue;
static std::unique_ptr<std::thread> i_thread;

void hcpsi::startOnPort(const char* port)
{
    if(i_isAlive)
    {
        i_logger.warnf("Already started");
        return;
    }

    i_failed = false;
    i_thread = std::make_unique<std::thread>(hcpsi::interfaceThread, port);
}

const char* hcpsi::getStatusStr()
{
    return i_statusStr.c_str();
}

void hcpsi::stop()
{
    if(!i_isAlive)
    {
        i_logger.warnf("System interface not started");
        return;
    }

    i_isAlive = false;
    i_thread->join();
    i_thread.reset();
}

bool hcpsi::isAlive()
{
    return i_isAlive;
}

bool hcpsi::failed()
{
    return i_failed;
}

void hcpsi::send(const HCPPacket& packet)
{
    if(!i_isAlive)
    {
        i_logger.warnf("System interface not started");
        return;
    }

    i_sendQueue.push(packet);

    if(i_sendQueue.back().isWriteable())
    {
        i_sendQueue.back().end();
        i_logger.warnf("Packet must be sent with end()");
    }
}

void hcpsi::interfaceThread(const char* port)
{
    i_statusStr = "Starting on port " + std::string(port);

    i_serial = std::make_unique<HCPSerial>(port);

    if(i_serial->isOpen()) 
    {
        i_statusStr = "Initializing system on port " + std::string(port);
    }
    else
    {
        i_statusStr = "Failed to start on port " + std::string(port);
        i_logger.errorf("Failed to start on port %s", port);
        i_isAlive = false;
        i_failed = true;
        return;
    }

    while(i_isAlive)
    {
        if(!i_sendQueue.empty())
        {
            HCPPacket packet = i_sendQueue.front();
            i_sendQueue.pop();

            i_logger.infof("Sending packet with command %d", packet.getCommand());

            i_serial->write(packet.m_data.data(), packet.m_data.size());
        }


        i_serial->poll();
        // Thread sleep 1/120th of a second
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    i_serial.reset();
    i_statusStr = "Stopped";
    i_isAlive = false;
}