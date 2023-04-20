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
#include <Images.hpp>
#include <Mesh.hpp>

HCPLogger mainLogger("Main");

int main()
{
    glfwInit();

    mainLogger.infof("Initializing Window");
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);
    HCPInputContext* input = HCPInputs::registerWindow(window);

    mainLogger.infof("Loading OpenGL");
    hcpui::init(window);

    // Create a button and viewport
    HCPButton button("Le Button");
    HCPViewport viewport;
    viewport.x = 100;
    viewport.width = viewport.height = 100;

    // Load an image texture
    HCPImagePtr image = hcpimg::loadImage("res/texture.png");

    // Load a mesh
    HCPVertexFormat format;
    format.size = 2;
    format[0].data = HCPVF_ATTRB_USAGE_POS
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(3);
    format[1].data = HCPVF_ATTRB_USAGE_COLOR
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(4);
    HCPMeshPtr mesh = hcpm::loadMeshes("res/piston.ply")[0];
    mesh->makeRenderable(format);

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, hcpui::getWindowWidth(), hcpui::getWindowHeight());

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        hcps::setProjectionMatrix(glm::mat4(1.0f));
        hcps::POS_COLOR();

        mesh->render();

        hcpui::setupUIRendering();
        hcpui::genQuad(10, 10, 100, 100, 0XFFFFFFFF, 0);
        hcpui::genQuad(10, 300, 100, 400, 0XFF00FFFF, 0);
        image->bindTexture(1);
        hcpui::genQuad(400, 400, 600, 600, 0xFFFFFFFF, 1);
        hcpui::genString("§0Wa§lss§6up", 0, 0, 30, 0xFFFFFFFF);
        viewport.start(false);
        {
            button.draw();
            button.setText(std::to_string(button.localCursorX()).c_str());
        }
        viewport.end();
        hcpui::renderBatch();

        if(button.isPressed()) mainLogger.infof("Button Pressed");

        HCPInputs::update();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}