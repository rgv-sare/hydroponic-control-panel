#ifndef HCP_ROBOTRENDERER_HPP
#define HCP_ROBOTRENDERER_HPP

namespace HCPRobotRenderer
{
    void init();
    void terminate();

    void loadResources();

    void drawAll();
    void drawArm();

    void setX(float x);
    void setY(float y);
    void setSwivel(float swivel);
    void setClaw(float claw);

    float getX();
    float getY();
    float getSwivel();
    float getClaw();

    void reset();
} // namespace HCPRobotRenderer


#endif // HCP_ROBOTRENDERER_HPP