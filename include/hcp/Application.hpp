#ifndef HCP_APPLICATION_HPP
#define HCP_APPLICATION_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Inputs.hpp"
#include "Screen.hpp"

class HCPApplication
{
public:
    HCPApplication(const char* title);

    void setup();
    void loop();
    void terminate();

    bool shouldClose() const;

    void setCurrentScreen(HCPScreen* screen);
private:
    const char* m_title;
    bool m_shouldClose;

    GLFWwindow* m_window;
    HCPInputContext* m_inputContext;

    HCPScreen* m_currentScreen;

    void loadResources();
};

#endif // HCP_APPLICATION_HPP