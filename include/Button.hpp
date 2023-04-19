#ifndef HCPBUTTON_HPP
#define HCPBUTTON_HPP

#include "Widget.hpp"

class HCPButton : public HCPWidget
{
public:
    HCPButton();
    HCPButton(const char* text);
protected:
    virtual void doDraw();
private:
};

#endif // HCPBUTTON_HPP