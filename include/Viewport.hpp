#ifndef HCPVIEWPORT_HPP
#define HCPVIEWPORT_HPP

#include "Widget.hpp"

class HCPViewport : public HCPWidget
{
public:
    HCPViewport();

    void start(bool clipping);
    void end();
protected:
    virtual void doDraw();
private:
    bool m_wasClipping;
};

#endif // HCPVIEWPORT_HPP