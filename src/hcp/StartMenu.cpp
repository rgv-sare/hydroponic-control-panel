#include "hcp/StartMenu.hpp"

#include <iostream>
#include <cstring>

#include "UIRender.hpp"

HCPMainMenu::HCPMainMenu() :
    HCPScreen(Type::MAIN_MENU, "Start Menu")
{
    snprintf(m_splashText, 256, "Hydroponics Control Panel - %s", hcpr::getAppVersion());
}

void HCPMainMenu::setup()
{
    m_nasaMindsLogo = hcpr::getImage("nasa_minds_logo");
    m_selectSerialPortButton.setText("§7Unselected");
    m_okButton.setText("OK");
}

void HCPMainMenu::draw()
{
    hcpui::setupUIRendering();

    hcpui::genString(m_splashText, 0, 0, 18.0f, 0xFF777777);

    m_viewport.width = 800;
    m_viewport.height = 600;
    m_viewport.x = (hcpui::getUIWidth() - m_viewport.width) / 2.0f;
    m_viewport.y = (hcpui::getUIHeight() - m_viewport.height) / 2.0f;

    m_viewport.start(false);
    {
        hcpui::genGradientQuad(HCPDirection::BOTTOM, 0, m_viewport.height, m_viewport.width, m_viewport.height + 30.0f, -1946157056, 0, 0);
        hcpui::genQuad(0, 0, m_viewport.width, m_viewport.height, 0x4C000000);

        HCPViewport logoViewport;
        logoViewport.width = 200;
        logoViewport.height = 200;
        logoViewport.x = (m_viewport.width - logoViewport.width) / 2.0f;
        logoViewport.y = m_viewport.height / 4.0f - logoViewport.height / 2.0f;

        logoViewport.start(false);
        {
            m_nasaMindsLogo->bindTexture(1);
            hcpui::genQuad(0, 0, logoViewport.width, logoViewport.height, 0xFFFFFFFF, 1);
        }
        logoViewport.end();

        hcpui::genHorizontalLine(m_viewport.height * 0.5f, 30, m_viewport.width - 30, 0x22FFFFFF, 3.0f);

        HCPViewport buttonViewport;
        buttonViewport.width = 600;
        buttonViewport.height = 80;
        buttonViewport.x = (m_viewport.width - buttonViewport.width) / 2.0f;
        buttonViewport.y = m_viewport.height * 0.71f - buttonViewport.height / 2.0f;

        buttonViewport.start(false);
        {
            hcpui::genString(HCPAlignment::TOP_CENTER, "Select Serial Port", buttonViewport.width / 2.0f, 0, 22.0f, 0xFFFFFFFF);

            m_selectSerialPortButton.width = buttonViewport.width * 0.9f;
            m_selectSerialPortButton.height = buttonViewport.height - 22.0f;
            m_selectSerialPortButton.y = 30.0f;
            m_selectSerialPortButton.draw();

            m_okButton.height = m_selectSerialPortButton.height;
            m_okButton.width = buttonViewport.width * 0.1f - 5.0f;
            m_okButton.x = buttonViewport.width - m_okButton.width;
            m_okButton.y = 30.0f;
            m_okButton.draw();
        }
        buttonViewport.end();
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
    const char* ports[8] =
    {
        "COM0",
        "COM1",
        "COM2",
        "COM3",
        "COM4",
        "COM5",
        "COM6",
        "COM7"
    };

    if(m_selectSerialPortButton.isPressed())
    {
        m_serialPortSelectionWindow =
        HCPUIWindow::createWindow<HCPSelectionWindow>("Select Serial Port", ports, 7, &m_selectedSerialPort);
    }

    if(!m_serialPortSelectionWindow.expired())
    {
        auto serialPortSelectionWindow = m_serialPortSelectionWindow.lock();

        if(serialPortSelectionWindow->madeSelection())
        {
            serialPortSelectionWindow->setShouldClose(true);
            m_serialPortSelectionWindow.reset();
            m_selectSerialPortButton.setText(ports[m_selectedSerialPort]);
            std::cout << "Selected serial port: " << m_selectedSerialPort << std::endl;
        }
    }
}