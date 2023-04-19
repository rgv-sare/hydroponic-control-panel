#include "Inputs.hpp"

static HCPInputContext* s_activeContext = NULL;
static std::map<GLFWwindow*, HCPInputContext> s_inputContexts;

// Callbacks
static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
static void onCursorMove(GLFWwindow* window, double cursorX, double cursorY);
static void onScroll(GLFWwindow* window, double scrollX, double scrollY);

static int i_getNthBit(const uint32_t* flagBuffer, int bit);

GLFWwindow* HCPInputContext::getWindow() const
{
    return m_window;
}

bool HCPInputContext::isKeyHeld(int key) const
{
    return (bool) i_getNthBit(m_keyHeldStates, key);
}

bool HCPInputContext::iskeyPressed(int key) const
{
    return (bool) i_getNthBit(m_keyPressedStates, key);
}

bool HCPInputContext::iskeyReleased(int key) const
{
    return (bool) i_getNthBit(m_keyReleasedStates, key);
}

bool HCPInputContext::iskeyRepeating(int key) const
{
    return (bool) i_getNthBit(m_keyRepeatingStates, key);
}

bool HCPInputContext::isMButtonHeld(int button) const
{
    return (bool) i_getNthBit(&m_mouseHeldStates, button);
}

bool HCPInputContext::isMButtonPressed(int button) const
{
    return (bool) i_getNthBit(&m_mousePressedStates, button);
}

bool HCPInputContext::isMButtonReleased(int button) const
{
    return (bool) i_getNthBit(&m_mouseReleasedStates, button);
}

float HCPInputContext::cursorX() const
{
    return m_cursorPosX;
}

float HCPInputContext::cursorY() const
{
    return m_cursorPosY;
}

float HCPInputContext::cursorDeltaX() const
{
    return m_cursorPosX - m_prevCursorPosX;
}

float HCPInputContext::cursorDeltaY() const
{
    return m_cursorPosY - m_prevCursorPosY;
}

bool HCPInputContext::justScrolled() const
{
    return m_justScrolled;
}

float HCPInputContext::scrollDeltaX() const
{
    return m_scrollDeltaX;
}

float HCPInputContext::scrollDeltaY() const
{
    return m_scrollDeltaY;
}

HCPInputContext::HCPInputContext() :
    HCPInputContext(NULL)
{
}

HCPInputContext::HCPInputContext(GLFWwindow* window) :
    m_window(window),
    m_mouseHeldStates(0),
    m_mousePressedStates(0),
    m_mouseReleasedStates(0),
    m_prevCursorPosX(0.0f),
    m_prevCursorPosY(0.0f),
    m_cursorPosX(0.0f),
    m_cursorPosY(0.0f),
    m_scrollDeltaX(0.0f),
    m_scrollDeltaY(0.0f),
    m_justScrolled(false)
{
    memset(m_keyHeldStates, 0, sizeof(m_keyHeldStates));
    memset(m_keyPressedStates, 0, sizeof(m_keyPressedStates));
    memset(m_keyReleasedStates, 0, sizeof(m_keyReleasedStates));
    memset(m_keyRepeatingStates, 0, sizeof(m_keyRepeatingStates));
}

HCPInputContext* HCPInputs::registerWindow(GLFWwindow* window)
{
    s_inputContexts[window] = HCPInputContext(window);

    glfwSetKeyCallback(window, onKeyEvent);
    glfwSetMouseButtonCallback(window, onMouseEvent);
    glfwSetCursorPosCallback(window, onCursorMove);
    glfwSetScrollCallback(window, onScroll);

    if(!s_activeContext) s_activeContext = &s_inputContexts[window];

    return &s_inputContexts[window];
}

HCPInputContext* HCPInputs::setActiveWindow(GLFWwindow* window)
{
    auto inputContext = s_inputContexts.find(window);

    if(inputContext != s_inputContexts.end())
    {
        s_activeContext = &inputContext->second;
    }

    return &inputContext->second;
}

HCPInputContext* HCPInputs::get(GLFWwindow* window)
{
    auto inputContext = s_inputContexts.find(window);

    if(inputContext != s_inputContexts.end())
    {
        return &inputContext->second;
    }

    return NULL;
}

HCPInputContext* HCPInputs::get()
{
    return s_activeContext;
}

void HCPInputs::update()
{
    auto i = s_inputContexts.begin();

    for(;i != s_inputContexts.end(); i++)
    {
        HCPInputContext& context = i->second;

        memset(context.m_keyPressedStates, 0, sizeof(context.m_keyPressedStates));
        memset(context.m_keyReleasedStates, 0, sizeof(context.m_keyReleasedStates));
        memset(context.m_keyRepeatingStates, 0, sizeof(context.m_keyRepeatingStates));
        context.m_mousePressedStates = 0;
        context.m_mouseReleasedStates = 0;

        context.m_prevCursorPosX = context.m_cursorPosX;
        context.m_prevCursorPosY = context.m_cursorPosY;
        context.m_justScrolled = false;
    }
}

static inline void i_setNthBit(uint32_t* flagBuffer, int bit)
{
    uint32_t* flagChunk = flagBuffer + (bit / sizeof(int32_t));
    int bitPos = bit % 32;

    uint32_t set = ((uint32_t) 1) << bitPos;
    *flagChunk |= set;
}

static inline void i_clearNthBit(uint32_t* flagBuffer, int bit)
{
    uint32_t* flagChunk = flagBuffer + (bit / sizeof(int32_t));
    int bitPos = bit % 32;

    uint32_t set = ~(((uint32_t) 1) << bitPos);
    *flagChunk &= set;
}

static inline int i_getNthBit(const uint32_t* flagBuffer, int bit)
{
    const uint32_t* flagChunk = flagBuffer + (bit / sizeof(int32_t));
    int bitPos = bit % 32;

    uint32_t value = ((uint32_t) 1) << bitPos;
    value &= *flagChunk;
    value = value >> bitPos;

    return value;
}

static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    HCPInputContext* context = HCPInputs::get(window);
    if(!context) return;

    if(action == GLFW_PRESS)
    {
        i_setNthBit(context->m_keyPressedStates, key);
        i_setNthBit(context->m_keyHeldStates, key);
    }
    else if(action == GLFW_RELEASE)
    {
        i_clearNthBit(context->m_keyHeldStates, key);
        i_setNthBit(context->m_keyReleasedStates, key);
    }
    else if(action == GLFW_REPEAT)
    {
        i_setNthBit(context->m_keyRepeatingStates, key);
    }
}

static void onMouseEvent(GLFWwindow* window, int button, int action, int mods)
{
    HCPInputContext* context = HCPInputs::get(window);
    if(!context) return;

    if(action == GLFW_PRESS)
    {
        i_setNthBit(&context->m_mousePressedStates, button);
        i_setNthBit(&context->m_mouseHeldStates, button);
    }
    else if(action == GLFW_RELEASE)
    {
        i_clearNthBit(&context->m_mouseHeldStates, button);
        i_setNthBit(&context->m_mouseReleasedStates, button);
    }
}

static void onCursorMove(GLFWwindow* window, double cursorX, double cursorY)
{
    HCPInputContext* context = HCPInputs::get(window);
    if(!context) return;
    
    context->m_cursorPosX = (float) cursorX;
    context->m_cursorPosY = (float) cursorY;
}

static void onScroll(GLFWwindow* window, double scrollX, double scrollY)
{
    HCPInputContext* context = HCPInputs::get(window);
    if(!context) return;

    context->m_scrollDeltaX = (float) scrollX;
    context->m_scrollDeltaY = (float) scrollY;
    context->m_justScrolled = true;
}