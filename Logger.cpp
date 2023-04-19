#include "Logger.hpp"

#include <stdarg.h>
#include <stdio.h>

HCPLogger::HCPLogger() :
    m_moduleName("Logger")
{
}

HCPLogger::HCPLogger(const char* moduleName) :
    m_moduleName(moduleName)
{
}

const std::string& HCPLogger::getModuleName() const
{
    return m_moduleName;
}

HCPLogger& HCPLogger::submodule(const char* submodule)
{
    m_submoduleName = submodule;

    return *this;
}

HCPLogger& HCPLogger::infof(const char* fmt, ...)
{
    char info[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(info, sizeof(info), fmt, args);
    va_end(args);

    if(!m_submoduleName.empty())
    {
        printf("[INFO][%s][%s]: %s\n", m_moduleName.c_str(), m_submoduleName.c_str(), info);
        m_submoduleName.clear();
    }
    else
    {
        printf("[INFO][%s]: %s\n", m_moduleName.c_str(), info);
    }

    return *this;
}

HCPLogger& HCPLogger::warnf(const char* fmt, ...)
{
    char info[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(info, sizeof(info), fmt, args);
    va_end(args);

    if(!m_submoduleName.empty())
    {
        printf("[WARNING][%s][%s]: %s\n", m_moduleName.c_str(), m_submoduleName.c_str(), info);
        m_submoduleName.clear();
    }
    else
    {
        printf("[WARNING][%s]: %s\n", m_moduleName.c_str(), info);
    }

    return *this;
}

HCPLogger& HCPLogger::errorf(const char* fmt, ...)
{
    char info[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(info, sizeof(info), fmt, args);
    va_end(args);

    if(!m_submoduleName.empty())
    {
        printf("[ERROR][%s][%s]: %s\n", m_moduleName.c_str(), m_submoduleName.c_str(), info);
        m_submoduleName.clear();
    }
    else
    {
        printf("[ERROR][%s]: %s\n", m_moduleName.c_str(), info);
    }

    return *this;
}

HCPLogger& HCPLogger::fatalf(const char* fmt, ...)
{
    char info[4096];

    va_list args;
    va_start(args, fmt);
    vsnprintf(info, sizeof(info), fmt, args);
    va_end(args);

    if(!m_submoduleName.empty())
    {
        printf("[FATAL][%s][%s]: %s\n", m_moduleName.c_str(), m_submoduleName.c_str(), info);
        m_submoduleName.clear();
    }
    else
    {
        printf("[FATAL][%s]: %s\n", m_moduleName.c_str(), info);
    }

    return *this;
}