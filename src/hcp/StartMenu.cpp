#include "hcp/StartMenu.hpp"

#include <iostream>
#include <cstring>

#include "UIRender.hpp"
#include "hcp/Serial.hpp"
#include "hcp/Application.hpp"
#include "hcp/MainMenu.hpp"

HCPStartMenu::HCPStartMenu() :
    HCPScreen(Type::START_MENU, "Start Menu"),
    m_selectedSerialPort(-1)
{
    snprintf(m_splashText, 256, "Hydroponics Control Panel - %s", hcpr::getAppVersion());
}

void HCPStartMenu::setup()
{
    m_nasaMindsLogo = hcpr::getImage("nasa_minds_logo");
    m_selectSerialPortButton.setText("§7Unselected");
    m_okButton.setText("OK");

    std::vector<std::string> serialPorts = HCPSerial::getSerialPorts();

    m_serialPorts.resize(serialPorts.size());
    for (size_t i = 0; i < serialPorts.size(); i++)
    {
        serialPorts[i].push_back('\0');
        m_serialPorts[i] = new char[serialPorts[i].size()];
        memcpy(m_serialPorts[i], serialPorts[i].c_str(), serialPorts[i].size());
    }
}

void HCPStartMenu::draw()
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

void HCPStartMenu::close()
{
    for(char* portName : m_serialPorts)
    {
        delete[] portName;
    }
}

void HCPStartMenu::handleInput()
{
    if(m_selectSerialPortButton.isPressed())
    {
        m_serialPortSelectionWindow =
        HCPUIWindow::createWindow<HCPSelectionWindow>("Select Serial Port", (const char**) m_serialPorts.data(), (int) m_serialPorts.size(), &m_selectedSerialPort);
    }

    if(!m_serialPortSelectionWindow.expired())
    {
        auto serialPortSelectionWindow = m_serialPortSelectionWindow.lock();

        if(serialPortSelectionWindow->madeSelection())
        {
            serialPortSelectionWindow->setShouldClose(true);
            m_serialPortSelectionWindow.reset();
            m_selectSerialPortButton.setText(m_serialPorts[m_selectedSerialPort]);
        }
    }

    if(m_okButton.isPressed() && m_selectedSerialPort != -1)
    {
        HCPMainMenu* mainMenu = new HCPMainMenu(m_serialPorts[m_selectedSerialPort]);
        HCPApplication::getInstance()->setCurrentScreen(mainMenu);
    }
}