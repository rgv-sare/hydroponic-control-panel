#include "Widget.hpp"

#include "UIRender.hpp"
#include "Shaders.hpp"

// Clipping (with viewports)
std::stack<HCPWidget::ClippingState> HCPWidget::m_clippingStack;

static int m_currentZLevel = 0;

int HCPWidget::getCurrentZLevel()
{
    return m_currentZLevel;
}

void HCPWidget::setCurrentZLevel(int zLevel)
{
    m_currentZLevel = zLevel;
}

HCPWidget::HCPWidget() :
    x(0.0f), y(0.0f), width(0.0f), height(0.0f),
    m_hovered(false),
    m_visible(true),
    m_enabled(true),
    m_zLevel(0),
    m_type(WIDGET),
    m_modelView(1.0f),
    m_localCursor(0.0f, 0.0f),
    m_justPressed(false),
    m_pressed(false),
    m_justReleased(false)
{
    if(m_clippingStack.empty())
    {
        m_clippingStack.push({glm::mat4(1.0f), 0.0f, 0.0f, 0.0f, 0.0f, false});
    }
}

HCPWidget::~HCPWidget()
{

}

bool HCPWidget::isHovered() const
{
    return m_hovered;
}

bool HCPWidget::isVisible() const
{
    return m_visible;
}

bool HCPWidget::isEnabled() const
{
    return m_enabled;
}

bool HCPWidget::isOnCurrentZLevel() const
{
    return m_zLevel == m_currentZLevel;
}

bool HCPWidget::isHeld() const
{
    return m_pressed;
}

bool HCPWidget::isPressed() const
{
    return m_justPressed;
}

bool HCPWidget::isReleased() const
{
    return m_justReleased;
}

void HCPWidget::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void HCPWidget::setVisible(bool visible)
{
    m_visible = visible;
}

void HCPWidget::setZLevel(int zLevel)
{
    m_zLevel = zLevel;
}

void HCPWidget::setText(const char* text)
{
    m_text = text;
}

HCPWidget::Type HCPWidget::getType() const
{
    return m_type;
}

int HCPWidget::getZLevel() const
{
    return m_zLevel;
}

const char* HCPWidget::getText() const
{
    return m_text.c_str();
}

float HCPWidget::localCursorX() const
{
    return m_localCursor.x;
}

float HCPWidget::localCursorY() const
{
    return m_localCursor.y;
}

void HCPWidget::draw()
{
    m_modelView = HCPShaders::getModelViewMatrix();
    m_modelView = m_modelView * HCPUIRender::getModelViewMatrix();

    HCPInputContext* input = HCPUIRender::getInputContext();
    updateHoverState();

    doDraw();

    m_justPressed = m_justReleased = false;
    if(input->isMButtonPressed(GLFW_MOUSE_BUTTON_1) && m_hovered)
    {
        m_justPressed = true;
        m_pressed = true;
    }
    else if(input->isMButtonReleased(GLFW_MOUSE_BUTTON_1))
    {
        m_pressed = false;
        m_justReleased = true;
    }
}

void HCPWidget::doDraw()
{
    HCPUIRender::genQuad(x, y, x + width, y + height, 0xFFFFFFFF);
}

void HCPWidget::updateHoverState()
{
    ClippingState clip = m_clippingStack.top();

    glm::mat4 inverseModelView = glm::inverse(clip.modelView);
    glm::vec4 uiCursorWidget(
        HCPUIRender::getUICursorX(),
        HCPUIRender::getUICursorY(),
        0.0f,
        1.0f
    );
    glm::vec4 uiCursorClip = uiCursorWidget;

    uiCursorClip = inverseModelView * uiCursorClip;

    bool cursorInBounds = clip.clipping ? uiCursorClip.x > clip.left && uiCursorClip.y > clip.top && uiCursorClip.x < clip.right && uiCursorClip.y < clip.bottom : true;
    
    inverseModelView = glm::inverse(m_modelView);

    uiCursorWidget = inverseModelView * uiCursorWidget;
    m_localCursor.x = uiCursorWidget.x - x;
    m_localCursor.y = uiCursorWidget.y - y;

    bool hovered = uiCursorWidget.x >= x && uiCursorWidget.y >= y && uiCursorWidget.x < x + width && uiCursorWidget.y < y + height;

    m_hovered = isOnCurrentZLevel() && hovered && cursorInBounds;
}