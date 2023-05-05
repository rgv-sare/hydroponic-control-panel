#ifdef _WIN32
#include "hcp/Serial.hpp"

#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <regstr.h>
#include <map>

#define READ_BUFFER_SIZE 4096

struct ImplWindows
{
    char port[128];
    HANDLE handle;

    uint8_t readBuffer[READ_BUFFER_SIZE];
    std::vector<uint8_t> writeBuffer;
};

static HCPLogger* i_logger = nullptr;

#define getImpl() (*((ImplWindows*) m_impl))

void IMPL_FUNC HCPSerial::begin()
{
    if(!m_port[0]) return;

    ImplWindows& impl = getImpl();

    impl.handle = CreateFileA(impl.port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (impl.handle == INVALID_HANDLE_VALUE)
    {
        s_logger.errorf("Failed to open serial port: %s", impl.port);
        return;
    }
    else
    {
        m_open = true;
        s_logger.infof("Opened serial port: %s", impl.port);
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    dcbSerialParams.BaudRate = m_baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = m_stopBits;
    dcbSerialParams.Parity = m_parity;

    if (!SetCommState(impl.handle, &dcbSerialParams))
    {
        s_logger.errorf("Failed to set serial port parameters: %s", impl.port);
        close();
        return;
    }

    impl_setTimeout(m_timeout);
}   

void IMPL_FUNC HCPSerial::close()
{
    ImplWindows& impl = getImpl();

    if (m_open)
    {
        CloseHandle(impl.handle);
        m_open = false;
    }
}

void IMPL_FUNC HCPSerial::poll()
{
    if(!m_open) return;

    ImplWindows& impl = getImpl();

    COMSTAT comStat;
    DWORD errorFlags;
    if(!ClearCommError(impl.handle, &errorFlags, &comStat))
    {
        s_logger.errorf("Failed to get comm status of port: %s", impl.port);
        close();
        return;
    }

    if(errorFlags & CE_BREAK)
    {
        s_logger.errorf("Serial port break detected: %s", impl.port);
        close();
        return;
    }

    if(comStat.cbInQue)
    {
        DWORD bytesRead;
        
        size_t maxBytesRead = min(comStat.cbInQue, READ_BUFFER_SIZE);
        ReadFile(impl.handle, impl.readBuffer, maxBytesRead, &bytesRead, NULL);

        DWORD error = GetLastError();
        if(error != ERROR_SUCCESS && error != ERROR_IO_PENDING)
        {
            s_logger.errorf("Failed to read from serial port: %s error %d", impl.port, error);
            close();
            return;
        }

        m_readBuffer.insert(m_readBuffer.end(), impl.readBuffer, impl.readBuffer + bytesRead);
        
    }

    flushOuput();
}

bool IMPL_FUNC HCPSerial::flushOuput()
{
    if(!m_open || m_writeBuffer.empty()) return false;

    ImplWindows& impl = getImpl();

    COMSTAT comStat;
    if(!ClearCommError(impl.handle, NULL, &comStat))
    {
        s_logger.errorf("Failed to get comm status of port: %s", impl.port);
        close();
        return false;
    }

    // Copy write buffer to impl write buffer
    impl.writeBuffer.resize(m_writeBuffer.size());
    memcpy(impl.writeBuffer.data(), m_writeBuffer.data(), m_writeBuffer.size());

    WriteFile(impl.handle, impl.writeBuffer.data(), impl.writeBuffer.size(), NULL, NULL);

    m_writeBuffer.clear();

    return true;
}

void IMPL_FUNC HCPSerial::impl_init()
{
    if(!i_logger) i_logger = &HCPSerial::s_logger;

    m_impl = new ImplWindows();
    ImplWindows& impl = getImpl();

    // Append '\\.\' prefix to COM port name
    strcpy_s(impl.port, sizeof(impl.port), "\\\\.\\");
    strcat_s(impl.port, sizeof(impl.port), m_port);

    impl.handle = INVALID_HANDLE_VALUE;
}

void IMPL_FUNC HCPSerial::impl_setBaudRate(uint32_t baudRate)
{
    if(!m_open) return;

    ImplWindows& impl = getImpl();

    DCB dcbSerialParams = { 0 };

    GetCommState(impl.handle, &dcbSerialParams);
    dcbSerialParams.BaudRate = baudRate;

    if (!SetCommState(impl.handle, &dcbSerialParams))
    {
        s_logger.errorf("Failed to set serial port baud rate: %s", impl.port);
        close();
    }
}

void IMPL_FUNC HCPSerial::impl_setParity(Parity parity)
{
    if(!m_open) return;

    ImplWindows& impl = getImpl();

    DCB dcbSerialParams = { 0 };

    GetCommState(impl.handle, &dcbSerialParams);
    dcbSerialParams.Parity = parity;

    if (!SetCommState(impl.handle, &dcbSerialParams))
    {
        s_logger.errorf("Failed to set serial port parity: %s", impl.port);
        close();
    }
}

void IMPL_FUNC HCPSerial::impl_setStopBits(StopBits stopBits)
{
    if(!m_open) return;

    ImplWindows& impl = getImpl();

    DCB dcbSerialParams = { 0 };

    GetCommState(impl.handle, &dcbSerialParams);
    dcbSerialParams.StopBits = stopBits;

    if (!SetCommState(impl.handle, &dcbSerialParams))
    {
        s_logger.errorf("Failed to set serial port stop bits: %s", impl.port);
        close();
    }
}

void IMPL_FUNC HCPSerial::impl_setTimeout(Timeout timeout)
{
    if(!m_open) return;

    ImplWindows& impl = getImpl();

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = timeout.perByteTimeout;
    timeouts.ReadTotalTimeoutConstant = timeout.readTimeoutConstant;
    timeouts.ReadTotalTimeoutMultiplier = timeout.readTimeoutMultiplier;
    timeouts.WriteTotalTimeoutConstant = timeout.writeTimeoutConstant;
    timeouts.WriteTotalTimeoutMultiplier = timeout.writeTimeoutMultiplier;

    if (!SetCommTimeouts(impl.handle, &timeouts))
    {
        s_logger.errorf("Failed to set serial port timeouts: %s", impl.port);
        close();
    }
}

void IMPL_FUNC HCPSerial::impl_destory()
{
    if(m_impl) delete (ImplWindows*) m_impl;
}

std::vector<std::string> IMPL_FUNC HCPSerial::getSerialPorts()
{
    std::vector<std::string> ports;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        return ports;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++)
    {
        DWORD dataType;
        DWORD dataSize = 0;
        SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, &dataType, 0, 0, &dataSize);
        if (dataSize == 0)
            continue;

        std::vector<char> data(dataSize);
        if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, &dataType, (PBYTE)data.data(), dataSize, &dataSize))
            continue;

        std::string friendlyName(data.begin(), data.end());
        if (friendlyName.find("COM") == std::string::npos)
            continue;

        std::string portName = friendlyName.substr(friendlyName.find("COM"));
        
        // Replace ')' with '\0'
        portName[portName.find(')')] = '\0';
        ports.push_back(portName);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return ports;
}

#endif // Windows