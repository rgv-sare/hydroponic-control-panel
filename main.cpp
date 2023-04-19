#include <iostream>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <Logger.hpp>
#include <Inputs.hpp>
#include <MeshBuilder.hpp>

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

    // Create mesh builder
    HCPVertexFormat vtxFmt;
    vtxFmt.size = 1;
    vtxFmt[0].data = HCPVF_ATTRB_USAGE_POS
                   | HCPVF_ATTRB_TYPE_FLOAT
                   | HCPVF_ATTRB_SIZE(3)
                   | HCPVF_ATTRB_NORMALIZED_FALSE;

    HCPMeshBuilder meshBuilder(vtxFmt);

    mainLogger.infof("Entering Program Loop");
    while(!glfwWindowShouldClose(window))
    {
        if(input->isKeyHeld(GLFW_KEY_SPACE)) glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        else glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        meshBuilder.reset();
        meshBuilder.position(-0.5f, -0.5f, 0.0f);
        meshBuilder.position( 0.5f, -0.5f, 0.0f);
        meshBuilder.position( 0.0f, 0.5f, 0.0f);
        
        meshBuilder.drawArrays(GL_TRIANGLES);

        HCPInputs::update();
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}