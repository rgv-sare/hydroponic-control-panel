#ifndef HCP_UIRENDER_HPP
#define HCP_UIRENDER_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

enum HCPDirection
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

enum HCPAlignment
{
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};

class HCPUIRender
{
public:
    static void init(GLFWwindow* window);

    static void setupUIRendering();

    static void genQuad(float left, float top, float right, float bottom, uint32_t color, int texID);
    static void genGradientQuad(HCPDirection direction, float left, float top, float right, float bottom, uint32_t color1, uint32_t color2, int texID);
    static void genVerticalLine(float x, float top, float bottom, uint32_t color);
    static void genHorizontalLine(float y, float left, float right, uint32_t color);
    static void genString(HCPAlignment alignment, const char* str, float x, float y, float scale, uint32_t color);
    
    static void renderBatch();

    static float getStringwidth(const char* str, float scale);
    static float getCharWidth(int unicode, float scale);

    static int getwindowWidth();
    static int getwindowHeight();

    static float getUIWidth();
    static float getUIHeight();
    static float getUIScale();

    static void pushStack();
    static void popStack();
    static void translate(float x, float y);
    static void scale(float x, float y);
    static void rotate(float angle);
    static void multiplyMatrix(const glm::mat4& matrix);
    static void setMatrix(const glm::mat4& matrix);
    static void getModelViewMatrix(glm::mat4& matrix);
};

#endif // HCP_UIRENDER_HPP