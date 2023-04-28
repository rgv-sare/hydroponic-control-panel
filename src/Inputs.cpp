#include "Inputs.hpp"

#include <cstring>

static HCPInputContext* s_activeContext = NULL;
static std::map<GLFWwindow*, HCPInputContext> s_inputContexts;

// Callbacks
static bool i_controllerCallbackSet = false;
static void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
static void onCursorMove(GLFWwindow* window, double cursorX, double cursorY);
static void onScroll(GLFWwindow* window, double scrollX, double scrollY);
static void onControllerConnect(int id, int event);

static void i_setNthBit(uint32_t* flagBuffer, int bit);
static void i_clearNthBit(uint32_t* flagBuffer, int bit);
static int i_getNthBit(const uint32_t* flagBuffer, int bit);

HCPGameController::HCPGameController() :
    m_id(-1),
    m_connected(false),
    m_numButtons(0),
    m_numAxes(0)
{
    m_name[0] = '\0';
}

int HCPGameController::getID() const
{
    return m_id;
}

const char* HCPGameController::getName() const
{
    return m_name;
}

bool HCPGameController::isConnected() const
{
    return m_connected;
}

int HCPGameController::numButtons() const
{
    return m_numButtons;
}

bool HCPGameController::isButtonHeld(int button) const
{
    return (bool) i_getNthBit(m_buttonHeldStates, button);
}

bool HCPGameController::isButtonPressed(int button) const
{
    return (bool) i_getNthBit(m_buttonPressedStates, button);
}

bool HCPGameController::isButtonReleased(int button) const
{
    return (bool) i_getNthBit(m_buttonReleasedStates, button);
}

int HCPGameController::numAxes() const
{
    return m_numAxes;
}

bool HCPGameController::axisJustMoved(int axis) const
{
    return (bool) i_getNthBit(m_axisMovedStates, axis);
}

float HCPGameController::axis(int axis) const
{
    return m_axes[axis];
}

float HCPGameController::axisDelta(int axis) const
{
    return m_axesDeltas[axis];
}

HCPGameController::HCPGameController(int id, const char* name) :
    m_id(id),
    m_connected(true),
    m_numButtons(0),
    m_numAxes(0)
{
    strncpy_s(m_name, name, 128);

    memset(m_buttonHeldStates, 0, sizeof(m_buttonHeldStates));
    memset(m_buttonPressedStates, 0, sizeof(m_buttonPressedStates));
    memset(m_buttonReleasedStates, 0, sizeof(m_buttonReleasedStates));
    memset(m_axisMovedStates, 0, sizeof(m_axisMovedStates));
    memset(m_axes, 0, sizeof(m_axes));
    memset(m_axesDeltas, 0, sizeof(m_axesDeltas));
}

void HCPGameController::update()
{
    memset(m_buttonPressedStates, 0, sizeof(m_buttonPressedStates));
    memset(m_buttonReleasedStates, 0, sizeof(m_buttonReleasedStates));
    memset(m_axisMovedStates, 0, sizeof(m_axisMovedStates));

    const float* axes = glfwGetJoystickAxes(m_id, &m_numAxes);
    for (int i = 0; i < m_numAxes; i++)
    {
        m_axesDeltas[i] = axes[i] - m_axes[i];
        m_axes[i] = axes[i];
        if (m_axesDeltas[i] != 0.0f)
        {
            i_setNthBit(m_axisMovedStates, i);
        }
    }

    const unsigned char* buttons = glfwGetJoystickButtons(m_id, &m_numButtons);
    for (int i = 0; i < m_numButtons; i++)
    {
        bool held = buttons[i] == GLFW_PRESS;
        bool pressed = held && !i_getNthBit(m_buttonHeldStates, i);
        bool released = !held && i_getNthBit(m_buttonHeldStates, i);
        if (pressed) i_setNthBit(m_buttonPressedStates, i);
        if (released) i_setNthBit(m_buttonReleasedStates, i);
        if (held) i_setNthBit(m_buttonHeldStates, i);
        else i_clearNthBit(m_buttonHeldStates, i);
    }
}

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

int HCPInputContext::numGameControllers()
{
    return m_numGameControllers;
}

const HCPGameController& HCPInputContext::getGameController(int id)
{
    return m_gameControllers[id];
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

int HCPInputContext::m_numGameControllers = 0;
HCPGameController HCPInputContext::m_gameControllers[16];

HCPInputContext* hcpi::registerWindow(GLFWwindow* window)
{
    s_inputContexts[window] = HCPInputContext(window);

    glfwSetKeyCallback(window, onKeyEvent);
    glfwSetMouseButtonCallback(window, onMouseEvent);
    glfwSetCursorPosCallback(window, onCursorMove);
    glfwSetScrollCallback(window, onScroll);

    if(!i_controllerCallbackSet)
    {
        glfwSetJoystickCallback(onControllerConnect);

        // Check for already connected controllers
        for(int i = 0; i < 16; i++)
        {
            if(glfwJoystickPresent(i))
                onControllerConnect(i, GLFW_CONNECTED);
        }

        i_controllerCallbackSet = true;
    }

    if(!s_activeContext) s_activeContext = &s_inputContexts[window];

    return &s_inputContexts[window];
}

HCPInputContext* hcpi::setActiveWindow(GLFWwindow* window)
{
    auto inputContext = s_inputContexts.find(window);

    if(inputContext != s_inputContexts.end())
    {
        s_activeContext = &inputContext->second;
    }

    return &inputContext->second;
}

HCPInputContext* hcpi::get(GLFWwindow* window)
{
    auto inputContext = s_inputContexts.find(window);

    if(inputContext != s_inputContexts.end())
    {
        return &inputContext->second;
    }

    return NULL;
}

HCPInputContext* hcpi::get()
{
    return s_activeContext;
}

void hcpi::update()
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

    for(int j = 0; j < 16; j++)
    {
        if(!HCPInputContext::m_gameControllers[j].isConnected()) continue;

        // If the controller was just disconnected
        if(!glfwJoystickPresent(j))
        {
            HCPInputContext::m_gameControllers[j] = HCPGameController();
            HCPInputContext::m_numGameControllers--;
            continue;
        }

        HCPInputContext::m_gameControllers[j].update();
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
    HCPInputContext* context = hcpi::get(window);
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
    HCPInputContext* context = hcpi::get(window);
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
    HCPInputContext* context = hcpi::get(window);
    if(!context) return;
    
    context->m_cursorPosX = (float) cursorX;
    context->m_cursorPosY = (float) cursorY;
}

static void onScroll(GLFWwindow* window, double scrollX, double scrollY)
{
    HCPInputContext* context = hcpi::get(window);
    if(!context) return;

    context->m_scrollDeltaX = (float) scrollX;
    context->m_scrollDeltaY = (float) scrollY;
    context->m_justScrolled = true;
}

static void onControllerConnect(int id, int event)
{
    if(event == GLFW_CONNECTED)
    {
        const char* name = glfwGetJoystickName(id);
        HCPInputContext::m_numGameControllers++;
        HCPInputContext::m_gameControllers[id] = HCPGameController(id, name);
    }
}