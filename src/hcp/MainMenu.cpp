#include "hcp/MainMenu.hpp"

#include "UIRender.hpp"

#include <glm/glm.hpp>
#include <cstring>

HCPMainMenu::HCPMainMenu() :
    HCPScreen(Type::MAIN_MENU, "Main Menu"),
    m_manualControlEnabled(true)
{
    snprintf(m_splashText, 256, "Hydroponic Control Panel - %s", hcpr::getAppVersion());
}

void HCPMainMenu::setup()
{
    m_nasaMindsLogo = hcpr::getImage("nasa_minds_logo");
    m_manualControlButton.setText("Manual Control: §2On");
}

void HCPMainMenu::draw()
{
    const int edgeSize = 5;

    hcpui::setupUIRendering();

    hcpui::genString(m_splashText, 0, 0, 18.0f, 0xFF777777);

    m_viewport.width = glm::max(800.0f, hcpui::getUIWidth() * 0.85f);
    m_viewport.height = glm::max(600.0f, m_viewport.width * 0.5625f);
    m_viewport.x = (hcpui::getUIWidth() - m_viewport.width) / 2.0f;
    m_viewport.y = (hcpui::getUIHeight() - m_viewport.height) / 2.0f;

    m_viewport.start(false);
    {
        hcpui::genGradientQuad(HCPDirection::BOTTOM, -edgeSize, m_viewport.height + edgeSize, m_viewport.width + edgeSize, m_viewport.height + 30.0f, -1946157056, 0, 0);
        hcpui::genQuad(-edgeSize, -edgeSize, m_viewport.width + edgeSize, m_viewport.height + edgeSize, 0x4C000000);

        HCPViewport headerViewport;
        headerViewport.width = m_viewport.width;
        headerViewport.height = m_viewport.height * 0.20f;
        
        headerViewport.start(false);
        {
            hcpui::genQuad(0, 0, headerViewport.width, headerViewport.height, 0x11FFFFFF);
            m_nasaMindsLogo->bindTexture(1);
            hcpui::genQuad(headerViewport.width - headerViewport.height, 0, headerViewport.width, headerViewport.height, 0x16000000, 0);
            hcpui::genQuad(headerViewport.width - headerViewport.height, 0, headerViewport.width, headerViewport.height, 0xFFFFFFFF, 1);

            hcpui::genQuad(0, 0, headerViewport.width - headerViewport.height, headerViewport.height * 0.35f + edgeSize * 2, 0x44000000);
            hcpui::pushStack();
            {
                hcpui::translate(edgeSize, edgeSize);

                float titleSize = (headerViewport.height - edgeSize * 2) * 0.35f;
                hcpui::genString("Hydroponics System Control Panel", 4, 4, titleSize, 0x22000000);
                hcpui::genString("Hydroponics System Control Panel", 0, 0, titleSize, 0xFFFFFFFF);

                hcpui::genString("Serial Port: COM3 §2Online", 0, titleSize + edgeSize * 2, titleSize * 0.68f, 0xFFAAAAAA);

                m_manualControlButton.height = titleSize * 0.68f;
                m_manualControlButton.y = titleSize + edgeSize * 3 + titleSize * 0.68f;
                m_manualControlButton.draw();

                hcpui::genString("Controller Detected", m_manualControlButton.width + edgeSize, m_manualControlButton.y, titleSize * 0.68f, 0xFFAAAAAA);
            }
            hcpui::popStack();
        }
        headerViewport.end();
    }
    m_viewport.end();

    HCPUIWindow::drawWindows();

    hcpui::renderBatch();

    handleInput();
}

void HCPMainMenu::close()
{

}

void HCPMainMenu::handleInput()
{
    if(m_manualControlButton.isPressed())
    {
        m_manualControlEnabled = !m_manualControlEnabled;
        m_manualControlButton.setText(m_manualControlEnabled ? "Manual Control: §2On" : "Manual Control: §4Off");
    }
}