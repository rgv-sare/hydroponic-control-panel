#ifndef HCP_SCREEN_HPP
#define HCP_SCREEN_HPP

#include <string>

class HCPScreen
{
public:
    enum Type
    {
        NONE,
        MAIN_MENU
    };

    HCPScreen(Type type, const char* title)
        : m_type(type), m_title(title)
    {

    }

    virtual void setup() = 0;
    virtual void draw() = 0;
    virtual void close() = 0;

    const char* getTitle() const { return m_title.c_str(); }
    Type getType() const { return m_type; }
protected:
    Type m_type;
    std::string m_title;
};

#endif // HCP_SCREEN_HPP