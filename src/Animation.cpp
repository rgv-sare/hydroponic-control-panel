#include "Animation.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// -------------- Smoother Class Begin ------------------ //
HCPSmoother::HCPSmoother() :
    m_acceleration(0.0),
    m_velocity(0.0),
    m_previousTime(0.0),
    m_currentTime(0.0),
    m_grabbed(false),
    m_grabbingTo(0.0),
    m_value(0.0),
    m_springy(false),
    m_speed(10.0),
    m_friction(1.0)
{
}

void HCPSmoother::grab()
{
    m_grabbed = true;
}

void HCPSmoother::grab(double grabTo)
{
    m_grabbed = true;
    m_grabbingTo = grabTo;
}

void HCPSmoother::setValueAndGrab(double value)
{
    m_grabbed = true;
    m_grabbingTo = value;
    m_value = value;
}

void HCPSmoother::release()
{
    m_grabbed = false;
}

bool HCPSmoother::isGrabbed() const
{
    return m_grabbed;
}

bool HCPSmoother::isSpringy() const
{
    return m_springy;
}

void HCPSmoother::setSpringy(bool springy)
{
    m_springy = springy;
}

void HCPSmoother::setSpeed(double speed)
{
    m_speed = speed;
}

void HCPSmoother::setFriction(double friciton)
{
    m_friction = friciton;
}

void HCPSmoother::setValue(double value)
{
    m_value = value;
}

double HCPSmoother::getSpeed() const
{
    return m_speed;
}

double HCPSmoother::getFriction() const
{
    return m_friction;
}

double HCPSmoother::grabbingTo() const
{
    return m_grabbingTo;
}

double HCPSmoother::getValue()
{
    update();
    return m_value;
}

float HCPSmoother::getValuef()
{
    update();
    return (float) m_value;
}

void HCPSmoother::update()
{
    m_previousTime = m_currentTime;
    m_currentTime = glfwGetTime();
    
    double delta = m_currentTime - m_previousTime;

    if(0.2 < delta) delta = 0.2;

    if(m_grabbed)
    {
        if(m_acceleration)
        {
            m_acceleration = (m_grabbingTo - m_value) * 32.0;
            m_velocity += m_acceleration * delta;
            m_velocity *= glm::pow(0.0025 / m_speed, delta);
        }
        else m_velocity = (m_grabbingTo - m_value) * m_speed;
    }

    m_value += m_velocity * delta;
    m_velocity *= glm::pow(0.0625 / (m_speed * m_friction), delta);
}

// -------------- Timer Class Begin ------------------ //

HCPTimer::HCPTimer(double tps) :
    m_tps(tps),
    m_tickDelta(1.0 / m_tps),
    m_nextTick(glfwGetTime())
{
}

int HCPTimer::ticksPassed()
{
    double currentTime = glfwGetTime();
    double result = (m_nextTick - currentTime) / m_tickDelta;
    
    int i = 0;
    while(m_nextTick < currentTime)
    {
        i++;
        m_nextTick += m_tickDelta;
    }

    return i;
}

double HCPTimer::getTPS()
{
    return m_tps;
}

double HCPTimer::partialTicks()
{
    double currentTime = glfwGetTime();
    double result = (m_nextTick - currentTime) / m_tickDelta;
    result = 1 - result;

    return 0 < result ? result : 0;
}

double HCPTimer::lerp(double start, double end)
{
    double pT = partialTicks();
    return start + (end - start) * pT;
}