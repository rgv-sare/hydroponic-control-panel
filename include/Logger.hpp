#ifndef HCP_LOGGER_HPP
#define HCP_LOGGER_HPP

#include <string>

class HCPLogger
{
public:
    HCPLogger();
    HCPLogger(const char* moduleName);

    const std::string& getModuleName() const;

    HCPLogger& submodule(const char* submodule);

    HCPLogger& infof(const char* fmt, ...);
    HCPLogger& warnf(const char* fmt, ...);
    HCPLogger& errorf(const char* fmt, ...);
    HCPLogger& fatalf(const char* fmt, ...);
private:
    std::string m_moduleName;
    std::string m_submoduleName;
};

#endif // HCP_LOGGER_HPP