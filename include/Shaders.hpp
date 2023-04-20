#ifndef HCP_SHADERS_HPP
#define HCP_SHADERS_HPP

#include <glm/glm.hpp>

class hcps
{
public:
    static void setProjectionMatrix(const glm::mat4& proj);
    static void setModelViewMatrix(const glm::mat4& modelView);
    static void setColor(const glm::vec4& color);
    static void setColor(float r, float g, float b, float a);

    static glm::mat4 getProjectionMatrix();
    static glm::mat4 getModelViewMatrix();
    static glm::vec4 getColor();

    static void POS();
    static void POS_UV();
    static void POS_UV_COLOR();
    static void POS_COLOR();
    static void POS_UV_COLOR_TEXID();
    static void UI();
};

#endif // HCP_SHADERS_HPP