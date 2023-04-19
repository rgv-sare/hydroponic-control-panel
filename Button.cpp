#include "Button.hpp"

#include "UIRender.hpp"

HCPButton::HCPButton()
{
    width = 180;
    height = 30;

    m_type = BUTTON;
}

HCPButton::HCPButton(const char* text)
{
    setText(text);

    width = 180;
    height = 30;

    m_type = BUTTON;
}

void HCPButton::doDraw()
{
    int textColor = !isEnabled() ? 0xFFA0A0A0 : isHovered() ? 0xFFFFFFBA :0xFFE0E0E0;
    int backgroundColor = isHovered() ? 0x664C4C4C : 0x66000000;
    HCPUIRender::genQuad(x, y, x + width, y + height, backgroundColor);
    HCPUIRender::genString(HCPAlignment::CENTER, getText(), x + width / 2, y + height / 2, 15.0f, textColor);
}