#include <iostream>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Logger.hpp>
#include <Inputs.hpp>
#include <MeshBuilder.hpp>
#include <Shaders.hpp>
#include <UIRender.hpp>

HCPLogger mainLogger("Main");

int main()
{
    glfwInit();

    mainLogger.infof("Initializing Window");
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);
    HCPInputContext* input = HCPInputs::registerWindow(window);

    mainLogger.infof("Loading OpenGL");
    HCPUIRender::init(window);

    // Create mesh builder
    HCPVertexFormat vtxFmt;
    vtxFmt.size = 1;
    vtxFmt[0].data = HCPVF_ATTRB_USAGE_POS
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(3)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        if(input->isKeyHeld(GLFW_KEY_SPACE)) glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        else glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        HCPUIRender::setupUIRendering();
        HCPUIRender::genQuad(10, 10, 100, 100, 0XFFFFFFFF, 0);
        HCPUIRender::genQuad(10, 300, 100, 400, 0XFF00FFFF, 0);
        HCPUIRender::renderBatch();

        HCPInputs::update();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}