#ifndef HCP_INPUTS_HPP
#define HCP_INPUTS_HPP

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <map>

class HCPGameController
{
public:
    HCPGameController();

    const char* getName() const;
    int getID() const;
    bool isConnected() const;

    int numButtons() const;
    bool isButtonHeld(int button) const;
    bool isButtonPressed(int button) const;
    bool isButtonReleased(int button) const;

    int numAxes() const;
    bool axisJustMoved(int axis) const;
    float axis(int axis) const;
    float axisDelta(int axis) const;
private:
    HCPGameController(int id, const char* name);

    void update();

    char m_name[128];
    int m_id;
    bool m_connected;

    int m_numButtons;
    uint32_t m_buttonHeldStates[2];
    uint32_t m_buttonPressedStates[2];
    uint32_t m_buttonReleasedStates[2];

    int m_numAxes;
    uint32_t m_axisMovedStates[1];
    float m_axes[32];
    float m_axesDeltas[32];

    friend class hcpi;
    friend void onControllerConnect(int id, int event);
};

class HCPInputContext
{
public:
    HCPInputContext();

    GLFWwindow* getWindow() const;

    bool isKeyHeld(int key) const;
    bool iskeyPressed(int key) const;
    bool iskeyReleased(int key) const;
    bool iskeyRepeating(int key) const;

    bool isMButtonHeld(int button) const;
    bool isMButtonPressed(int button) const;
    bool isMButtonReleased(int button) const;

    float cursorX() const;
    float cursorY() const;

    float cursorDeltaX() const;
    float cursorDeltaY() const;

    bool justScrolled() const;
    float scrollDeltaX() const;
    float scrollDeltaY() const;

    bool charWasTyped() const;
    uint32_t getTypedChar() const;

    static int numGameControllers();
    static const HCPGameController& getGameController(int index);
private:
    HCPInputContext(GLFWwindow* window);

    GLFWwindow* m_window;

    uint32_t m_keyHeldStates[100];
    uint32_t m_keyPressedStates[100];
    uint32_t m_keyReleasedStates[100];
    uint32_t m_keyRepeatingStates[100];

    uint32_t m_mouseHeldStates;
    uint32_t m_mousePressedStates;
    uint32_t m_mouseReleasedStates;

    float m_prevCursorPosX;
    float m_prevCursorPosY;

    float m_cursorPosX;
    float m_cursorPosY;

    float m_scrollDeltaX;
    float m_scrollDeltaY;

    bool m_justScrolled;

    bool m_charTyped;
    uint32_t m_typedChar;

    static int m_numGameControllers;
    static HCPGameController m_gameControllers[16];

    friend class hcpi;
    friend void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
    friend void onCursorMove(GLFWwindow* window, double cursorX, double cursorY);
    friend void onScroll(GLFWwindow* window, double scrollX, double scrollY);
    friend void onCharTyped(GLFWwindow* window, uint32_t typedChar);
    friend void onControllerConnect(int id, int event);
};

class hcpi
{
public:
    static HCPInputContext* registerWindow(GLFWwindow* window);
    static HCPInputContext* setActiveWindow(GLFWwindow* window);
    static HCPInputContext* get(GLFWwindow* window);
    static HCPInputContext* get();
    static void update();
};

#endif // HCP_INPUTS_HPP