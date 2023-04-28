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
    class JoyStickVisual : public HCPWidget
    {
    public:
        JoyStickVisual();

        float joyX, joyY;
        const char* axesLabels[4]; // x, -x, y, -y
    protected:
        void doDraw() override;
    };

    char m_splashText[256];
    HCPImagePtr m_nasaMindsLogo;

    HCPViewport m_viewport;

    HCPButton m_manualControlButton;
    bool m_manualControlEnabled;

    JoyStickVisual m_xyJoystick;
    JoyStickVisual m_clawJoystick;

    // Float robot values
    float m_robX, m_robY, m_robSwivel, m_robClaw;

    void drawHeader();
    void drawConsole();
    void drawJoysticks();
    void drawRobotView();
    void drawRobotArmView();
    void handleInput();
};

#endif // HCP_MAIN_MENU_HPP