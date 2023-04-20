#ifndef HCP_MAIN_MENU_HPP
#define HCP_MAIN_MENU_HPP

#include "hcp/Screen.hpp"
#include "hcp/Resources.hpp"

#include "UIWindow.hpp"
#include "Viewport.hpp"
#include "Button.hpp"

class HCPMainMenu : public HCPScreen
{
public:
    HCPMainMenu();

    void setup() override;
    void draw() override;
    void close() override;
private:
    char m_splashText[256];
    HCPImagePtr m_nasaMindsLogo;

    HCPViewport m_viewport;

    HCPButton m_manualControlButton;
    bool m_manualControlEnabled;

    void handleInput();
};

#endif // HCP_MAIN_MENU_HPP