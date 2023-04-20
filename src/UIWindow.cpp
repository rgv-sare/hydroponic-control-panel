#include "UIWindow.hpp"

#include "UIRender.hpp"

std::list<std::shared_ptr<HCPUIWindow>> HCPUIWindow::s_windows;

HCPUIWindow::CloseButton::CloseButton() :
    HCPButton("X")
{
}

void HCPUIWindow::CloseButton::doDraw()
{
    int color = isHovered() && isEnabled() ? -439615488 : -864092160;  
    hcpui::genQuad(x, y, x + width, y + height, color);
}

void HCPUIWindow::drawWindows()
{
    for(auto i = s_windows.begin(); i != s_windows.end();)
    {
        auto window = *i;
        if (window->shouldClose())
        {
            HCPWidget::setCurrentZLevel(HCPWidget::getCurrentZLevel() - 1);
            i = s_windows.erase(i);
        }
        else
        {
            window->draw();
            ++i;
        }
    }
}

void HCPUIWindow::closeWindows()
{
    for (auto window : s_windows)
    {
        window->setShouldClose(true);
    }
}

bool HCPUIWindow::areWindowsOpen()
{
    return !s_windows.empty();
}

HCPUIWindow::HCPUIWindow(const char* title) :
    m_title(title),
    m_shouldClose(false)
{
    width = 550;
    m_viewport.height = 200;
}

HCPUIWindow::~HCPUIWindow()
{
    
}

void HCPUIWindow::drawContents()
{
    hcpui::genQuad(0, 0, m_viewport.width, m_viewport.height, 0XFFFFFFFF);
}

void HCPUIWindow::setShouldClose(bool shouldClose)
{
    m_shouldClose = shouldClose;
}

bool HCPUIWindow::shouldClose() const
{
    return m_shouldClose;
}

void HCPUIWindow::draw()
{
    const float titleBarHeight = 40.0f;
    const float edgeSize = 5.0f;

    height = titleBarHeight + m_viewport.height + edgeSize;
    x = (hcpui::getUIWidth() - width) / 2.0f;
    y = (hcpui::getUIHeight() - height) / 2.0f;

    HCPViewport box;
    box.x = x;
    box.y = y;
    box.width = width;
    box.height = height;
    HCPViewport titleBar;
    titleBar.width = box.width;
    titleBar.height = titleBarHeight;

    box.start(false);
    {
        hcpui::genGradientQuad(HCPDirection::BOTTOM, -edgeSize, height, width + edgeSize, height + 30.0f, -1946157056, 0, 0);
        hcpui::genGradientQuad(HCPDirection::BOTTOM, -edgeSize, -edgeSize, width + edgeSize, height + edgeSize, -12632257, -14277082, 0);

        titleBar.start(false);
        {
            hcpui::genGradientQuad(HCPDirection::RIGHT, 0, 0, width, titleBarHeight, -1728053248, 1275068416, 0);

            hcpui::genString(HCPAlignment::CENTER_LEFT, m_title.c_str(), edgeSize, titleBarHeight / 2.0f, 22.0f, 0xFFFFFFFF);

            m_closeButton.width = m_closeButton.height = titleBarHeight - edgeSize * 2.0f;
            m_closeButton.x = width - m_closeButton.width - edgeSize;
            m_closeButton.y = edgeSize;
            m_closeButton.draw();
        }
        titleBar.end();

        m_viewport.x = 0;
        m_viewport.y = titleBarHeight + edgeSize;
        m_viewport.width = box.width;

        m_viewport.start(false);
        {
            drawContents();
        }
        m_viewport.end();
    }
    box.end();

    if(m_closeButton.isPressed()) setShouldClose(true);
}