#include <iostream>

#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydroponics Control Panel", NULL, NULL);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}