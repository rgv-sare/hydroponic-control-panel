#include "hcp/MainMenu.hpp"

#include "hcp/RobotRenderer.hpp"

#include "UIRender.hpp"
#include "Shaders.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

static const int edgeSize = 5;

HCPMainMenu::HCPMainMenu() :
    HCPScreen(Type::MAIN_MENU, "Main Menu"),
    m_manualControlEnabled(true)
{
    snprintf(m_splashText, 256, "Hydroponic Control Panel - %s", hcpr::getAppVersion());

    m_xyJoystick.setText("XY Joystick");
    m_clawJoystick.setText("Claw Joystick");

    m_clawJoystick.axesLabels[0] = "CW";
    m_clawJoystick.axesLabels[1] = "CCW";
    m_clawJoystick.axesLabels[2] = "Open";
    m_clawJoystick.axesLabels[3] = "Close";

    m_robX = m_robY = m_robSwivel = m_robClaw = 0.0f;
}

void HCPMainMenu::setup()
{
    m_nasaMindsLogo = hcpr::getImage("nasa_minds_logo");
    m_manualControlButton.setText("Manual Control: §2On");
}

void HCPMainMenu::draw()
{
    hcpui::setupUIRendering();

    hcpui::genString(m_splashText, 0, 0, 18.0f, 0xFF777777);

    HCPInputContext* inputs = hcpi::get();
    hcpui::genString(std::to_string(inputs->numGameControllers()).c_str(), 0, 20, 18.0f, 0xFFFFFFFF);

    m_viewport.width = glm::max(800.0f, hcpui::getUIWidth() * 0.85f);
    m_viewport.height = glm::max(600.0f, m_viewport.width * 0.5625f);
    m_viewport.x = (hcpui::getUIWidth() - m_viewport.width) / 2.0f;
    m_viewport.y = (hcpui::getUIHeight() - m_viewport.height) / 2.0f;

    m_viewport.start(false);
    {
        hcpui::genGradientQuad(HCPDirection::BOTTOM, -edgeSize, m_viewport.height + edgeSize, m_viewport.width + edgeSize, m_viewport.height + 30.0f, -1946157056, 0, 0);
        hcpui::genQuad(-edgeSize, -edgeSize, m_viewport.width + edgeSize, m_viewport.height + edgeSize, 0x4C000000);

        drawHeader();

        drawConsole();

        drawJoysticks();

        drawRobotView();

        drawRobotArmView();
    }
    m_viewport.end();

    HCPUIWindow::drawWindows();

    hcpui::renderBatch();

    handleInput();
}

void HCPMainMenu::close()
{

}

void HCPMainMenu::drawHeader()
{
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
            const char* controllerStatus = HCPInputContext::numGameControllers() == 0 ? "No Controllers Detected" : "Controller Detected";
            hcpui::genString(controllerStatus, m_manualControlButton.width + edgeSize, m_manualControlButton.y, titleSize * 0.68f, 0xFFAAAAAA);
        }
        hcpui::popStack();
    }
    headerViewport.end();
}

void HCPMainMenu::drawConsole()
{
    HCPViewport consoleViewport;
    consoleViewport.width = m_viewport.width * 0.5f;
    consoleViewport.height = m_viewport.height * 0.28f;
    consoleViewport.y = m_viewport.height - consoleViewport.height;

    consoleViewport.start(false);
    {
        hcpui::genQuad(0, 0, consoleViewport.width, consoleViewport.height, 0x22000000);

        HCPViewport consoleHeaderViewport;
        consoleHeaderViewport.width = consoleViewport.width;
        consoleHeaderViewport.height = consoleViewport.height * 0.15f;

        consoleHeaderViewport.start(false);
        {
            hcpui::genQuad(0, 0, consoleHeaderViewport.width, consoleHeaderViewport.height, 0x11000000);
            hcpui::genString("Console", edgeSize, edgeSize, consoleHeaderViewport.height - edgeSize * 2, 0xFFFFFFFF);
        }
        consoleHeaderViewport.end();
    }
    consoleViewport.end();
}

void HCPMainMenu::drawJoysticks()
{
    float joystickSize = m_viewport.height * 0.28f;
    m_xyJoystick.height = m_clawJoystick.height = joystickSize;
    m_xyJoystick.width = m_clawJoystick.width = joystickSize;

    float spacing = (m_viewport.width * 0.5f - joystickSize * 2.0f) / 3.0f;
    m_xyJoystick.x = m_viewport.width * 0.5f + spacing;
    m_clawJoystick.x = m_viewport.width * 0.5f + joystickSize + spacing * 2.0f;

    m_xyJoystick.y = m_clawJoystick.y = m_viewport.height - joystickSize;

    m_xyJoystick.draw();
    m_clawJoystick.draw();
}

void HCPMainMenu::drawRobotView()
{
    HCPViewport body;
    body.width = m_viewport.width * 0.6f - edgeSize * 3;
    body.height = m_viewport.height * 0.52f - edgeSize * 4;
    body.y = m_viewport.height * 0.20f + edgeSize * 2;
    body.x = edgeSize;

    body.start(false);
    {
        hcpui::genQuad(-edgeSize, -edgeSize, body.width + edgeSize, body.height + edgeSize, 0x22000000);

        HCPViewport infoArea;
        infoArea.width = body.width * 0.22f;
        infoArea.height = body.height * 0.33f;
        infoArea.x = body.width - infoArea.width;

        infoArea.start(false);
        {
            const float textSize = 14.0f;
            hcpui::genGradientQuad(HCPDirection::BOTTOM, 0, 0, infoArea.width, infoArea.height, 0x22000000, 0x00000000);

            hcpui::translate(edgeSize, edgeSize);
            char info[2][32];
            snprintf(info[0], 32, "X Axis: %2.2f", m_robX);
            snprintf(info[1], 32, "Y Axis: %2.2f", m_robY);

            hcpui::genString(info[0], 0, 0, textSize, 0xFFFFFFFF);
            hcpui::genString(info[1], 0, textSize + edgeSize, textSize, 0xFFFFFFFF);
        }
        infoArea.end();

        HCPViewport cameraViewport;
        cameraViewport.width = body.width - infoArea.width - edgeSize;
        cameraViewport.height = body.height;

        cameraViewport.start(false);
        {
            const float robotScale = cameraViewport.height * 1.844e-2f;
            // Draw pillars
            const float pillarWidth = body.width * 0.035f;
            hcpui::genQuad(0, 0, pillarWidth, body.height, 0xFFAAAAAA);
            hcpui::genQuad(cameraViewport.width - pillarWidth, 0, cameraViewport.width, cameraViewport.height, 0xFFAAAAAA);

            // Draw rack
            const float rackWidth = pillarWidth * 0.5f;
            hcpui::genQuad
            (pillarWidth, cameraViewport.height - rackWidth - m_robY * robotScale, cameraViewport.width - pillarWidth, cameraViewport.height - m_robY * robotScale, 0xFFEAAAAA);

            hcpui::renderBatch();

            // Draw robot
            glEnable(GL_DEPTH_TEST);
            glm::mat4 modelview = hcpui::getModelViewMatrix();
            modelview = glm::translate(modelview, glm::vec3(pillarWidth + 12.0f * robotScale, cameraViewport.height - rackWidth, 0.0f));
            modelview = glm::scale(modelview, glm::vec3(robotScale, -robotScale, robotScale));
            
            hcps::setModelViewMatrix(modelview);
            HCPRobotRenderer::setX(m_robX);
            HCPRobotRenderer::setY(m_robY);
            HCPRobotRenderer::setSwivel(m_robSwivel);
            HCPRobotRenderer::setClaw(m_robClaw);
            HCPRobotRenderer::drawAll();

            hcpui::setupUIRendering();
        }
        cameraViewport.end();
    }
    body.end();
}

void HCPMainMenu::drawRobotArmView()
{
    HCPViewport body;
    body.width = m_viewport.width * 0.4f - edgeSize * 2;
    body.height = m_viewport.height * 0.52f - edgeSize * 4;
    body.y = m_viewport.height * 0.20f + edgeSize * 2;
    body.x = m_viewport.width * 0.6f + edgeSize;

    body.start(false);
    {
        hcpui::genQuad(-edgeSize, -edgeSize, body.width + edgeSize, body.height + edgeSize, 0x22000000);

        float centerX = body.width * 0.5f;
        float centerY = body.height * 0.5f;
        hcpui::genDisc(centerX, centerY, body.height * 0.5f, 0x22000000);

        HCPViewport infoArea;
        infoArea.width = body.width * 0.32f;
        infoArea.height = body.height * 0.33f;
        infoArea.x = body.width - infoArea.width;

        hcpui::renderBatch();

        // Draw arm
        const float robotScale = body.height * 3.5e-2f;
        glEnable(GL_DEPTH_TEST);
        glm::mat4 modelview = hcpui::getModelViewMatrix();
        modelview = glm::translate(modelview, glm::vec3(centerX, centerY, 0.0f));
        modelview = glm::scale(modelview, glm::vec3(robotScale, -robotScale, robotScale));
        modelview = glm::rotate(modelview, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
        
        hcps::setModelViewMatrix(modelview);
        HCPRobotRenderer::setX(m_robX);
        HCPRobotRenderer::setY(m_robY);
        HCPRobotRenderer::setSwivel(m_robSwivel);
        HCPRobotRenderer::setClaw(m_robClaw);
        HCPRobotRenderer::drawArm();
        hcpui::setupUIRendering();

        infoArea.start(false);
        {
            const float textSize = 14.0f;
            hcpui::genGradientQuad(HCPDirection::BOTTOM, 0, 0, infoArea.width, infoArea.height, 0x22000000, 0x00000000);

            hcpui::translate(edgeSize, edgeSize);
            char info[2][32];
            snprintf(info[0], 32, "Swivel: %2.2f", m_robSwivel);
            snprintf(info[1], 32, "Claw: %2.2f", m_robClaw);

            hcpui::genString(info[0], 0, 0, textSize, 0xFFFFFFFF);
            hcpui::genString(info[1], 0, textSize + edgeSize, textSize, 0xFFFFFFFF);
        }
        infoArea.end();
    }
    body.end();
}

void HCPMainMenu::handleInput()
{
    if(HCPInputContext::numGameControllers() == 0)
    {
        m_manualControlButton.setEnabled(false);
        m_manualControlButton.setText("Manual Control: §4Off");
        m_manualControlEnabled = false;

        m_xyJoystick.joyX = m_xyJoystick.joyY = 0.0f;
        m_clawJoystick.joyX = m_clawJoystick.joyY = 0.0f;
    }
    else
    {
        m_manualControlButton.setEnabled(true);

        const HCPGameController& controller = HCPInputContext::getGameController(0);
        m_xyJoystick.joyX = controller.axis(0);
        m_xyJoystick.joyY = controller.axis(1);
        m_robX += controller.axis(0) * 0.15f;
        m_robY -= controller.axis(1) * 0.15f;
        m_robX = glm::max(0.0f, glm::min(m_robX, 65.0f));
        m_robY = glm::max(0.0f, glm::min(m_robY, 28.0f));

        m_clawJoystick.joyX = controller.axis(2);
        m_clawJoystick.joyY = controller.axis(3);
        m_robSwivel -= controller.axis(2) * 0.01f;
        m_robClaw += controller.axis(3) * 0.005f;
        m_robClaw = glm::max(-0.5f, glm::min(m_robClaw, 0.37f));
    }

    if(m_manualControlButton.isPressed())
    {
        m_manualControlEnabled = !m_manualControlEnabled;
        m_manualControlButton
        .setText(m_manualControlEnabled && m_manualControlButton.isEnabled() ? "Manual Control: §2On" : "Manual Control: §4Off");
    }
}

HCPMainMenu::JoyStickVisual::JoyStickVisual()
{
    axesLabels[0] = "+X";
    axesLabels[1] = "-X";
    axesLabels[2] = "+Y";
    axesLabels[3] = "-Y";
}

void HCPMainMenu::JoyStickVisual::doDraw()
{
    HCPViewport body;
    body.width = width;
    body.height = height;
    body.x = x;
    body.y = y;

    body.start(false);
    {
        hcpui::genQuad(0, 0, body.width, body.height, 0x22000000);

        HCPViewport header;
        header.width = body.width;
        header.height = body.height * 0.15f;
        float textSize = header.height - edgeSize * 2;

        header.start(false);
        {
            hcpui::genQuad(0, 0, header.width, header.height, 0x11000000);
            hcpui::genString(getText(), edgeSize, edgeSize, textSize, 0xFFFFFFFF);
        }
        header.end();

        HCPViewport joystickArea;
        joystickArea.width = body.width;
        joystickArea.height = body.height - header.height;
        joystickArea.y = header.height;

        joystickArea.start(false);
        {
            float centerX = joystickArea.width * 0.5f;
            float centerY = joystickArea.height * 0.5f;
            float radius = joystickArea.height * 0.5f - edgeSize;
            hcpui::genDisc(centerX, centerY, radius, 0x22000000);

            float xTravel = joyX * radius * 0.4f;
            float yTravel = joyY * radius * 0.4f;
            hcpui::genDisc(centerX + xTravel, centerY + yTravel, radius * 0.56f, 0x22000000);
            hcpui::genDisc(centerX + xTravel, centerY + yTravel, radius * 0.46f, 0xAAFFFFFF);

            hcpui::genString(HCPAlignment::CENTER, axesLabels[0], centerX + radius, centerY, textSize, 0xFFAAAAAA);
            hcpui::genString(HCPAlignment::CENTER, axesLabels[1], centerX - radius, centerY, textSize, 0xFFAAAAAA);
            hcpui::genString(HCPAlignment::CENTER, axesLabels[2], centerX, centerY - radius, textSize, 0xFFAAAAAA);
            hcpui::genString(HCPAlignment::CENTER, axesLabels[3], centerX, centerY + radius, textSize, 0xFFAAAAAA);
        }
        joystickArea.end();
    }
    body.end();
}
