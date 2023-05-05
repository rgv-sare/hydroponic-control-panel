#ifndef HCP_START_MENU_HPP
#define HCP_START_MENU_HPP

#include "hcp/Screen.hpp"
#include "hcp/Resources.hpp"
#include "hcp/SelectionWindow.hpp"

#include "Viewport.hpp"
#include "Button.hpp"

class HCPStartMenu : public HCPScreen
{
public:
    HCPStartMenu();

    void setup() override;
    void draw() override;
    void close() override;
private:
    HCPImagePtr m_nasaMindsLogo;

    HCPViewport m_viewport;
    HCPButton m_selectSerialPortButton;
    HCPButton m_okButton;

    std::vector<char*> m_serialPorts;
    std::weak_ptr<HCPSelectionWindow> m_serialPortSelectionWindow;

    char m_splashText[256];

    int m_selectedSerialPort;

    void handleInput();
};

#endif // HCP_START_MENU_HPP