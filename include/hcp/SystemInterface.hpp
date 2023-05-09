#ifndef HCP_SYSTEM_INTERFACE_HPP
#define HCP_SYSTEM_INTERFACE_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <string>

// List of commands to the Hydroponics System
#define s_noop 128
#define s_echo 129
#define s_ret 130
#define s_getbyte 131
#define s_getword 132
#define s_getdword 133
#define s_getstring 134
#define s_getvars 135

// List of commands from the Hydroponics System
#define r_noop 128
#define r_logstr 129
#define r_ret 130

class HCPPacket
{
public:
    HCPPacket();
    HCPPacket(uint16_t command);

    void setCommand(uint16_t command);
    uint16_t getCommand() const;

    HCPPacket& putByte(uint8_t byte);
    HCPPacket& putShort(uint16_t word);
    HCPPacket& putInt(uint32_t dword);
    HCPPacket& putFloat(float fword);

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
    size_t dataNumBytes() const;
    void end();
    bool isWriteable() const;
private:
    uint16_t m_command;
    std::vector<uint8_t> m_data;

    bool m_writeable;
    mutable size_t m_getPtr;

    friend class hcpsi;
};

#define HCP_COMS // Marker for commands to the Hydroponics System
#define HCP_COMR // Marker for commands from the Hydroponics System

class hcpsi
{
public:
    enum Type
    {
        BYTE,
        SHORT,
        INT,
        FLOAT,
        STRING
    };

    static void startOnPort(const char* port);
    static const char* getStatusStr();
    static void stop();
    static bool isAlive();
    static bool failed();

    template<typename T>
    static const T getVariable(const char* name)
    {
        auto it = i_variablesMap.find(name);
        if (it == i_variablesMap.end())
        {
            if(typeid(T) == typeid(char*)) return (T)"";
            return T();
        }

        Variable* var = it->second;

        if(typeid(T) == typeid(char*)) return (T)var->data;
        return *(T*)var->data;
    }

    static Type getVariableType(const char* name);

    // Manual interface
    struct Variable
    {
        hcpsi::Type type;
        uint8_t data[256];
    };

    static std::map<std::string, Variable*> i_variablesMap;
    static std::vector<Variable> i_variables;

    static void send(const HCPPacket& packet);
private:
    static void processCommand(const HCPPacket& packet);
    
    // Commands to the Hydroponics System
    static HCP_COMS void com_noop();
    static HCP_COMS void com_echo(const char* str);
    //static HCP_COMS void com_ret(size_t dataSize, const uint8_t* data);
    static HCP_COMS void com_getbyte(uint16_t varID);
    static HCP_COMS void com_getword(uint16_t varID);
    static HCP_COMS void com_getdword(uint16_t varID);
    static HCP_COMS void com_getstring(uint16_t varID);
    static HCP_COMS void com_getvars();

    // Commands from the Hydroponics System (to process)
    static HCP_COMR void comr_logstr(const HCPPacket& packet);
    static HCP_COMR void comr_ret(const HCPPacket& packet);

    static void interfaceThread(const char* port);
};

#endif // HCP_SYSTEM_INTERFACE_HPP