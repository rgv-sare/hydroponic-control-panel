#include <iostream>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Logger.hpp>
#include <Inputs.hpp>
#include <MeshBuilder.hpp>
#include <Shaders.hpp>
#include <UIRender.hpp>
#include <Button.hpp>

HCPLogger mainLogger("Main");

int main()
{
    glfwInit();

    mainLogger.infof("Initializing Window");
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);
    HCPInputContext* input = HCPInputs::registerWindow(window);

    mainLogger.infof("Loading OpenGL");
    HCPUIRender::init(window);

    // Create a button
    HCPButton button("Le Button");

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        if(input->isKeyHeld(GLFW_KEY_SPACE)) glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        else glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        HCPUIRender::setupUIRendering();
        HCPUIRender::genQuad(10, 10, 100, 100, 0XFFFFFFFF, 0);
        HCPUIRender::genQuad(10, 300, 100, 400, 0XFF00FFFF, 0);
        HCPUIRender::genString("§0Wass§6up", 0, 0, 30, 0xFFFFFFFF);
        button.draw();
        HCPUIRender::renderBatch();

        if(button.isPressed()) mainLogger.infof("Button Pressed");

        HCPInputs::update();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}