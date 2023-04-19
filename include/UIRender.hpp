#ifndef HCP_UIRENDER_HPP
#define HCP_UIRENDER_HPP

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "FontRenderer.hpp"
#include "Inputs.hpp"

enum HCPDirection
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

class HCPUIRender
{
public:
    static void init(GLFWwindow* window);
    
    static HCPInputContext* getInputContext();

    static void setupUIRendering();

    static void genQuad(float left, float top, float right, float bottom, uint32_t color, int texID = 0);
    static void genGradientQuad(HCPDirection direction, float left, float top, float right, float bottom, uint32_t color1, uint32_t color2, int texID = 0);
    static void genVerticalLine(float x, float top, float bottom, uint32_t color, float width = 1.0f);
    static void genHorizontalLine(float y, float left, float right, uint32_t color, float width = 1.0f);
    static void genString(HCPAlignment alignment, const char* str, float x, float y, float scale, uint32_t color);
    static void genString(const char* str, float x, float y, float scale, uint32_t color);

    static void genQuad(float left, float top, float right, float bottom, const glm::vec4& color, int texID = 0);
    static void genGradientQuad(HCPDirection direction, float left, float top, float right, float bottom, const glm::vec4& color1, const glm::vec4& color2, int texID = 0);
    static void genVerticalLine(float x, float top, float bottom, const glm::vec4& color, float width = 1.0f);
    static void genHorizontalLine(float y, float left, float right, const glm::vec4& color, float width = 1.0f);
    static void genString(HCPAlignment alignment, const char* str, float x, float y, float scale,  const glm::vec4& color);
    static void genString(const char* str, float x, float y, float scale, const glm::vec4& color);
    
    static void renderBatch();

    static float getStringwidth(const char* str, float scale);

    static int getWindowWidth();
    static int getWindowHeight();

    static float getUIWidth();
    static float getUIHeight();
    static float getUIScale();

    static float getUICursorX();
    static float getUICursorY();

    static void pushStack();
    static void popStack();
    static void translate(float x, float y);
    static void scale(float x, float y);
    static void rotate(float angle);
    static void multiplyMatrix(const glm::mat4& matrix);
    static void setMatrix(const glm::mat4& matrix);
    static glm::mat4& getModelViewMatrix();
};

#endif // HCP_UIRENDER_HPP