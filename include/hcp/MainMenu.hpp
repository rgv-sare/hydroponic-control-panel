#ifndef HCP_MAIN_MENU_HPP
#define HCP_MAIN_MENU_HPP

#include "hcp/Screen.hpp"
#include "hcp/Resources.hpp"
#include "hcp/Serial.hpp"

#include "UIWindow.hpp"
#include "Viewport.hpp"
#include "Button.hpp"
#include "TextField.hpp"
#include "Animation.hpp"

#include <array>

class HCPMainMenu : public HCPScreen
{
public:
    HCPMainMenu();
    HCPMainMenu(const char* comPort);

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

    class Console : public HCPWidget
    {
    public:
        Console();

        void addLog(const char* log);
        void clearLog();
        const char* getCommand();

        bool commandSendTriggered();
    protected:
        void doDraw() override;
    private:
        int m_scroll;
        size_t m_logLen;
        size_t m_logIndex;
        std::vector<std::pair<const char*, size_t>> m_lines;
        std::array<char, 2049> m_log;
        HCPButton m_sendButton;
        HCPTextField m_commandField;
        HCPViewport m_viewport;

        void handleInput();
    };

    char m_splashText[256];
    HCPImagePtr m_nasaMindsLogo;

    HCPViewport m_viewport;

    HCPButton m_manualControlButton;
    bool m_manualControlEnabled;

    HCPSerial* m_serial;
    Console m_console;

    JoyStickVisual m_xyJoystick;
    JoyStickVisual m_clawJoystick;

    // Float robot values
    float m_robX, m_robY, m_robSwivel, m_robClaw;

    void drawHeader();
    void drawJoysticks();
    void drawRobotView();
    void drawRobotArmView();
    void handleInput();
};

#endif // HCP_MAIN_MENU_HPP