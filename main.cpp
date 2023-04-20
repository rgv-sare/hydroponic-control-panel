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
#include <Viewport.hpp>

HCPLogger mainLogger("Main");

int main()
{
    glfwInit();

    mainLogger.infof("Initializing Window");
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);
    HCPInputContext* input = HCPInputs::registerWindow(window);

    mainLogger.infof("Loading OpenGL");
    HCPUIRender::init(window);

    // Create a button and viewport
    HCPButton button("Le Button");
    HCPViewport viewport;
    viewport.x = 100;
    viewport.width = viewport.height = 100;

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, HCPUIRender::getWindowWidth(), HCPUIRender::getWindowHeight());

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        HCPUIRender::setupUIRendering();
        HCPUIRender::genQuad(10, 10, 100, 100, 0XFFFFFFFF, 0);
        HCPUIRender::genQuad(10, 300, 100, 400, 0XFF00FFFF, 0);
        HCPUIRender::genString("§0Wa§lss§6up", 0, 0, 30, 0xFFFFFFFF);
        viewport.start(false);
        {
            button.draw();
            button.setText(std::to_string(button.localCursorX()).c_str());
        }
        viewport.end();
        HCPUIRender::renderBatch();

        if(button.isPressed()) mainLogger.infof("Button Pressed");

        HCPInputs::update();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}