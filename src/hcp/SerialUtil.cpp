#include "hcp/SerialUtil.hpp"

static std::vector<std::string> i_getSerialPorts();

std::vector<std::string> hcpserial::getSerialPorts()
{
    return i_getSerialPorts();
}

#ifdef _WIN32 // Windows
#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <regstr.h>

static std::vector<std::string> i_getSerialPorts()
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

#elif __linux__ // Linux (Untested)
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

static std::vector<std::string> i_getSerialPorts()
{
    std::vector<std::string> ports;

    DIR* dir = opendir("/dev");
    if (!dir)
    {
        return ports;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type != DT_CHR)
            continue;

        std::string path = "/dev/";
        path += entry->d_name;

        int fd = open(path.c_str(), O_RDWR | O_NOCTTY);
        if (fd < 0)
            continue;

        struct serial_struct serinfo;
        if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
            close(fd);
            continue;

        close(fd);

        ports.push_back(path);
    }

    closedir(dir);

    return ports;
}

#elif __APPLE__ // Mac OS X (Untested)
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <IOKit/serial/ioss.h>

static std::vector<std::string> i_getSerialPorts()
{
    std::vector<std::string> ports;

    DIR* dir = opendir("/dev");
    if (!dir)
        return ports;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type != DT_CHR)
            continue;

        std::string path = "/dev/";
        path += entry->d_name;

        int fd = open(path.c_str(), O_RDWR | O_NOCTTY);
        if (fd < 0)
            continue;

        struct serial_struct serinfo;
        if (ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
            close(fd);
            continue;

        close(fd);

        ports.push_back(path);
    }

    closedir(dir);

    return ports;
}

#endif