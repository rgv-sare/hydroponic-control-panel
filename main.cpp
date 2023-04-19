#include <iostream>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Logger.hpp>
#include <Inputs.hpp>

HCPLogger mainLogger("Main");

int main()
{
    glfwInit();

    mainLogger.infof("Initializing Window");
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);
    HCPInputContext* input = HCPInputs::registerWindow(window);

    mainLogger.infof("Loading OpenGL");
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        if(input->isKeyHeld(GLFW_KEY_SPACE)) glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        else glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}