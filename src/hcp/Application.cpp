#include "hcp/Application.hpp"

#include "UIRender.hpp"
#include "Logger.hpp"
#include "Images.hpp"

#include "hcp/Resources.hpp"
#include "hcp/StartMenu.hpp"
#include "hcp/MainMenu.hpp"
#include "hcp/RobotRenderer.hpp"

HCPLogger mainLogger("Main");

HCPApplication::HCPApplication(const char* title) :
    m_title(title),
    m_shouldClose(false),
    m_currentScreen(nullptr)
{

}

void HCPApplication::setup()
{
    mainLogger.infof("Setting up GLFW window");
    glfwInit();

    m_window = glfwCreateWindow(1280, 720, m_title, nullptr, nullptr);

    if (!m_window)
    {
        mainLogger.errorf("Failed to create GLFW window");
        m_shouldClose = true;
        return;
    }

    m_inputContext = hcpi::registerWindow(m_window);
    hcpui::init(m_window);

    HCPRobotRenderer::init();

    loadResources();
    setCurrentScreen(new HCPStartMenu());
}

void HCPApplication::loop()
{
    glViewport(0, 0, hcpui::getWindowWidth(), hcpui::getWindowHeight());
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if(m_currentScreen)
        m_currentScreen->draw();

    hcpi::update();
    glfwPollEvents();
    glfwSwapBuffers(m_window);
    m_shouldClose = glfwWindowShouldClose(m_window);
}

void HCPApplication::terminate()
{
    mainLogger.infof("Terminating GLFW window");
    glfwTerminate();

    HCPRobotRenderer::terminate();
}

bool HCPApplication::shouldClose() const
{
    return m_shouldClose;
}

void HCPApplication::setCurrentScreen(HCPScreen* screen)
{
    if(m_currentScreen == screen)
        return;

    HCPScreen* previousScreen = m_currentScreen;

    if(previousScreen)
        previousScreen->close();

    if(screen)
        screen->setup();

    m_currentScreen = screen;
}

void HCPApplication::loadResources()
{
    mainLogger.infof("Loading Resources");

    hcpr::addImage(hcpimg::loadImage("res/texture.png"), "nasa_minds_logo");

    HCPRobotRenderer::loadResources();
}