#ifndef HCP_INPUTS_HPP
#define HCP_INPUTS_HPP

#include <glfw/glfw3.h>

#include <map>

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

    friend class hcpi;
    friend void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void onMouseEvent(GLFWwindow* window, int button, int action, int mods);
    friend void onCursorMove(GLFWwindow* window, double cursorX, double cursorY);
    friend void onScroll(GLFWwindow* window, double scrollX, double scrollY);
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