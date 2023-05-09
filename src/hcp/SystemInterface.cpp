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
    dst[0] = i_nibbleToHex((0xF0 & byte) >> 4);
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

// ----------- HCPPacket ------------ //

HCPPacket::HCPPacket()
    : HCPPacket(128)
{
    
}

HCPPacket::HCPPacket(uint16_t command) :
    m_command(command),
    m_getPtr(1),
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

HCPPacket& HCPPacket::putByte(uint8_t byte)
{
    uint8_t hex[2];
    i_hexNibblesByte(byte, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);

    return *this;
}

HCPPacket& HCPPacket::putShort(uint16_t word)
{
    uint8_t hex[4];
    i_hexNibblesWord(word, hex);

    m_data.push_back(hex[0]);
    m_data.push_back(hex[1]);
    m_data.push_back(hex[2]);
    m_data.push_back(hex[3]);

    return *this;
}

HCPPacket& HCPPacket::putInt(uint32_t dword)
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

    return *this;
}

HCPPacket& HCPPacket::putFloat(float fword)
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

    return *this;
}

uint8_t HCPPacket::getByte(size_t index) const
{
    return i_byteFromNibbles(&m_data[1 + index * 2]);
}

uint16_t HCPPacket::getShort(size_t index) const
{
    return i_wordFromNibbles(&m_data[1 + index * 4]);
}

uint32_t HCPPacket::getInt(size_t index) const
{
    return i_dwordFromNibbles(&m_data[1 + index * 8]);
}

float HCPPacket::getFloat(size_t index) const
{
    return i_fwordFromNibbles(&m_data[1 + index * 8]);
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
    m_getPtr = 1;
}

bool HCPPacket::isEnd() const
{
    return m_getPtr >= m_data.size() - 1;
}

void HCPPacket::setGetPtr(size_t index)
{
    m_getPtr = index;
}

size_t HCPPacket::size() const
{
    return m_data.size();
}

size_t HCPPacket::dataNumBytes() const
{
    if(!m_writeable) return (m_data.size() - 1) / 2;
    else return (m_data.size() - 2) / 2;
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

typedef void (*ReturnCallback)(HCPPacket& packet);
static HCPLogger i_logger("SystemInterface");

std::map<std::string, hcpsi::Variable*> hcpsi::i_variablesMap;
std::vector<hcpsi::Variable> hcpsi::i_variables;
static std::map<uint8_t, ReturnCallback> i_pendingReturns;
static uint8_t i_nextRetId = 0;

static std::unique_ptr<HCPSerial> i_serial;
static std::string i_statusStr = "Not started";

static bool i_isAlive = false;
static bool i_failed = false;

static std::queue<HCPPacket> i_recvQueue;
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
    i_thread = std::make_unique<std::thread>([port] { interfaceThread(port); });
}

const char* hcpsi::getStatusStr()
{
    return i_statusStr.c_str();
}

void hcpsi::stop()
{
    i_isAlive = false;

    if(i_thread.get()) i_thread->join();
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

hcpsi::Type hcpsi::getVariableType(const char* name)
{
    auto it = i_variablesMap.find(name);
    if(it != i_variablesMap.end())
    {
        return it->second->type;
    }
    else
    {
        i_logger.warnf("Variable %s does not exist", name);
        return Type::BYTE;
    }
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

void hcpsi::processCommand(const HCPPacket& packet)
{
    uint16_t command = packet.getCommand();

    switch (command)
    {
    case r_noop:
        break;
    case r_logstr:
        break;
    case r_ret:
        comr_ret(packet);
        break;
    }
}

HCP_COMS void hcpsi::com_noop()
{
    HCPPacket packet(s_noop);
    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_echo(const char* str)
{
    HCPPacket packet(s_echo);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);

    while(*str != 0)
    {
        packet.putByte(*str);
        str++;
    }

    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_getbyte(uint16_t varID)
{
    HCPPacket packet(s_getbyte);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);
    packet.putShort(varID);

    auto onReturn = [] (HCPPacket& packet)
    {
        uint16_t varID = packet.getShort();
        uint8_t value = packet.getByte();

        uint8_t* data = i_variables[varID].data;
        *data = value;
    };

    i_pendingReturns[nextRetId] = onReturn;

    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_getword(uint16_t varID)
{
    HCPPacket packet(s_getword);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);
    packet.putShort(varID);

    auto onReturn = [] (HCPPacket& packet)
    {
        uint16_t varID = packet.getShort();
        uint16_t value = packet.getShort();

        uint8_t* data = i_variables[varID].data;
        *(uint16_t*) data = value;
    };

    i_pendingReturns[nextRetId] = onReturn;

    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_getdword(uint16_t varID)
{
    HCPPacket packet(s_getdword);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);
    packet.putShort(varID);

    auto onReturn = [] (HCPPacket& packet)
    {
        uint16_t varID = packet.getShort();
        uint32_t value = packet.getInt();

        uint8_t* data = i_variables[varID].data;
        *(uint32_t*) data = value;
    };

    i_pendingReturns[nextRetId] = onReturn;

    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_getstring(uint16_t varID)
{
    HCPPacket packet(s_getstring);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);
    packet.putShort(varID);

    auto onReturn = [] (HCPPacket& packet)
    {
        uint16_t varID = packet.getShort();

        uint8_t* data = i_variables[varID].data;
        uint8_t* ptr = data;

        while(!packet.isEnd())
        {
            *ptr = packet.getByte();
            if(*ptr == 0) break;
            ptr++;
        }
    };

    i_pendingReturns[nextRetId] = onReturn;

    packet.end();

    send(packet);
}

HCP_COMS void hcpsi::com_getvars()
{
    HCPPacket packet(s_getvars);

    uint8_t nextRetId = i_nextRetId;
    i_nextRetId++;

    packet.putByte(nextRetId);

    auto onReturn = [] (HCPPacket& packet)
    {
        i_variables.clear();
        i_variablesMap.clear();

        while(!packet.isEnd())
        {
            Variable var;
            memset(var.data, 0, sizeof(var.data));
            var.type = (Type) packet.getByte();
            
            // Get name
            std::string name;
            while(!packet.isEnd())
            {
                char c = packet.getByte();
                if(c == 0) break;
                name.push_back(c);
            }

            i_variables.push_back(var);
            i_variablesMap[name] = &i_variables.back();
        }
    };

    i_pendingReturns[nextRetId] = onReturn;

    packet.end();

    send(packet);
}

HCP_COMR void hcpsi::comr_ret(const HCPPacket& packet)
{
    uint8_t retId = packet.getByte();
    auto it = i_pendingReturns.find(retId);
    if(it != i_pendingReturns.end())
    {
        it->second(const_cast<HCPPacket&>(packet));
        i_pendingReturns.erase(it);
    }
    else i_logger.warnf("Received return with invalid ID %d", retId);
}

void hcpsi::interfaceThread(const char* port)
{
    i_statusStr = "Starting on port " + std::string(port);

    i_serial = std::make_unique<HCPSerial>(port, 115200);
    i_serial->setTimeout(HCPSerial::Timeout::fromTimeout(5000));
    i_serial->begin();

    if(i_serial->isOpen()) 
    {
        i_statusStr = "Initializing system on port " + std::string(port);
        com_getvars();
        i_isAlive = true;
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
        if(!i_serial->isOpen())
        {
            i_logger.errorf("Serial port closed");
            i_isAlive = false;
            break;
        }

        if(!i_sendQueue.empty())
        {
            HCPPacket packet = i_sendQueue.front();
            i_sendQueue.pop();

            i_serial->write(packet.m_data.data(), packet.m_data.size());
        }

        // Read serial data to form packets
        if(i_serial->available())
        {
            uint8_t readBuffer[1024];
            size_t readSize = i_serial->read(readBuffer, 1024);

            for(size_t i = 0; i < readSize; i++)
            {
                uint8_t byte = readBuffer[i];

                // New packet
                if(127 < byte)
                {
                    // End last packet if not ended
                    if(!i_recvQueue.empty() && i_recvQueue.back().isWriteable())
                        i_recvQueue.back().end();

                    HCPPacket& newPacket = i_recvQueue.emplace(byte);

                    while(i++ < readSize)
                    {
                        byte = readBuffer[i];

                        // End of packet
                        if(byte == 128)
                        {
                            newPacket.end();
                            break;
                        }
                        else newPacket.m_data.push_back(byte);
                    }
                }
                // Data byte
                else if(!i_recvQueue.empty())
                {
                    HCPPacket& packet = i_recvQueue.back();
                        
                    // End of packet
                    if(byte == 128)
                    {
                        packet.end();
                        break;
                    }
                    else packet.m_data.push_back(byte);
                }
            }
        }

        // Process received packets
        for(size_t i = 0; i < i_recvQueue.size(); i++)
        {
            HCPPacket& packet = i_recvQueue.front();

            if(packet.isEnd())
            {
                i_recvQueue.pop();

                processCommand(packet);
            }
        }

        i_serial->poll();
        // Thread sleep 1/120th of a second
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    i_serial.reset();
    i_statusStr = "Stopped";
    i_isAlive = false;
}