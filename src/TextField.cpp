#include "TextField.hpp"

#include "UIRender.hpp"

#include <cstring>
#include <glm/glm.hpp>
#include <iostream>

const int edgeSize = 4;

double HCPTextField::s_lastTimeCursorMoved = glfwGetTime();

HCPTextField::HCPTextField()
    : HCPTextField("")
{
}

HCPTextField::HCPTextField(const char* title):
    HCPWidget(),
    m_focused(false),
    m_cursorPos(0),
    m_secondCursorPos(0),
    m_lastCursorPos(0)
{
    setTitle(title);
}

void HCPTextField::doDraw()
{
    m_inputContext = hcpi::get();
    m_textSize = height - edgeSize * 2;

    handleKeyInputs();
    handleMouseInputs();
    handleCharInputs();

    if(m_cursorPos != m_lastCursorPos)
    {
        m_lastCursorPos = m_cursorPos;
        s_lastTimeCursorMoved = glfwGetTime();
    }

    if(m_focused && isHovered() && m_inputContext && m_inputContext->justScrolled())
    {
        double maxScroll = hcpui::getStringwidth(getText(), m_textSize) + width * 0.6f;
        m_textScroll.grab(glm::min(0.0, glm::max(m_textScroll.grabbingTo() + m_inputContext->scrollDeltaY() * 40.0, maxScroll)));
    }

    positionTitle();

    if(isPressed()) m_focused = true;
    else if(m_inputContext->isMButtonPressed(GLFW_MOUSE_BUTTON_1)) m_focused = false;

    m_viewport.x = x;
    m_viewport.y = y;
    m_viewport.width = width;
    m_viewport.height = height;

    m_viewport.start(true);
    {
        hcpui::genQuad(0, 0, width, height, 1275068416);

        float titleOffset = m_titlePosition.getValuef();
        float titleOpacity = m_titleOpacity.getValuef();

        hcpui::genString(HCPAlignment::CENTER_LEFT, m_title, titleOffset, height / 2, m_textSize, glm::vec4(0.66, 0.66, 0.66, titleOpacity));

        hcpui::pushStack();
        {
            float scroll = m_textScroll.getValuef();
            hcpui::translate(scroll, 0.0f);

            if(m_focused)
            {
                bool showCursor = long((glfwGetTime() - s_lastTimeCursorMoved) * 1000) / 600 % 2 == 0;
                float cursorPos = hcpui::getStringwidth(getText(), m_cursorPos, m_textSize) + edgeSize;

                if(m_secondCursorPos != m_cursorPos)
                {
                    float secondCursorPos = hcpui::getStringwidth(getText(), m_secondCursorPos, m_textSize) + edgeSize;
                    float selectionBoxX = cursorPos;
                    float selectionBoxWidth = secondCursorPos - cursorPos;

                    hcpui::genQuad(selectionBoxX, edgeSize, selectionBoxX + selectionBoxWidth, height - edgeSize, -1776726785);
                }
                if(showCursor)
                    hcpui::genQuad(cursorPos, edgeSize, cursorPos + 1, height - edgeSize, 0xFFFFFFFF);
            }

            int textColor = !isEnabled() ? -6250336 : isHovered() ? -70 : -2039584;
            hcpui::genString(HCPAlignment::CENTER_LEFT, getText(), edgeSize, height / 2, m_textSize, textColor);
        }
        hcpui::popStack();
    }
    m_viewport.end();
}

bool HCPTextField::isFocused() const
{
    return m_focused;
}

void HCPTextField::setFocused(bool focused)
{
    m_focused = focused;
}

const char* HCPTextField::getTitle() const
{
    return m_title;
}

void HCPTextField::setTitle(const char* title)
{
    strncpy_s(m_title, sizeof(m_title), title, sizeof(m_title) - 1);
}

void HCPTextField::setCursors(int cursorPos, int secondCursorPos)
{
    m_cursorPos = cursorPos;
    m_secondCursorPos = secondCursorPos;
}

void HCPTextField::setCursorAtStart(bool autoScroll)
{
    m_cursorPos = 0;
    m_secondCursorPos = 0;
    s_lastTimeCursorMoved = glfwGetTime();

    if(autoScroll) m_textScroll.grab(0.0);
}

void HCPTextField::setCursorAtEnd(bool autoScroll)
{
    m_cursorPos = (int) strlen(getText());
    m_secondCursorPos = m_cursorPos;
    s_lastTimeCursorMoved = glfwGetTime();

    if(autoScroll)
    {
        double stringWidth = hcpui::getStringwidth(getText(), m_textSize);
        double scroll = -(stringWidth - glm::min(stringWidth, width * 0.66));
        m_textScroll.grab(scroll);
    }
}

void HCPTextField::handleKeyInputs()
{
    if(!m_focused || !m_inputContext) return;

    bool shiftPressed = m_inputContext->isKeyHeld(GLFW_KEY_LEFT_SHIFT) || m_inputContext->isKeyHeld(GLFW_KEY_RIGHT_SHIFT);
    bool ctrlPressed = m_inputContext->isKeyHeld(GLFW_KEY_LEFT_CONTROL) || m_inputContext->isKeyHeld(GLFW_KEY_RIGHT_CONTROL);

    // Handle backspace key
    if(m_inputContext->iskeyPressed(GLFW_KEY_BACKSPACE) || m_inputContext->iskeyRepeating(GLFW_KEY_BACKSPACE))
        handleBackSpace();

    // Handle left key
    if(m_inputContext->iskeyPressed(GLFW_KEY_LEFT) || m_inputContext->iskeyRepeating(GLFW_KEY_LEFT))
    {
        m_cursorPos = glm::max(0, glm::min(m_cursorPos - 1, (int) strlen(getText())));
        if(!shiftPressed) m_secondCursorPos = m_cursorPos;
        autoScroll(1.0f);
    }

    // Handle right key
    if(m_inputContext->iskeyPressed(GLFW_KEY_RIGHT) || m_inputContext->iskeyRepeating(GLFW_KEY_RIGHT))
    {
        m_cursorPos = glm::max(0, glm::min(m_cursorPos + 1, (int) strlen(getText())));
        if(!shiftPressed) m_secondCursorPos = m_cursorPos;
        autoScroll(1.0f);
    }

    // Handle CTRL + A
    if(ctrlPressed && m_inputContext->iskeyPressed(GLFW_KEY_A))
    {
        setCursorAtEnd();
        m_secondCursorPos = 0;
    }

    // Handle CTRL + C
    if(ctrlPressed && m_inputContext->iskeyPressed(GLFW_KEY_C))
    {
        int start = glm::min(m_cursorPos, m_secondCursorPos);
        int end = glm::max(m_cursorPos, m_secondCursorPos);
        int length = end - start;

        // Substring
        std::string substring = getText();
        substring = substring.substr(start, length);

        glfwSetClipboardString(m_inputContext->getWindow(), substring.c_str());
    }

    // Handle CTRL + V
    if(ctrlPressed && m_inputContext->iskeyPressed(GLFW_KEY_V))
    {
        const char* clipboard = glfwGetClipboardString(m_inputContext->getWindow());
        if(clipboard)
        {
            if(m_secondCursorPos == m_cursorPos)
            {
                // Substring
                std::string substring = getText();
                substring.insert(m_cursorPos, clipboard);
                setText(substring.c_str());

                m_cursorPos += (int) strlen(clipboard);
                m_secondCursorPos = m_cursorPos;
                autoScroll(5.0f);
            }
            else
            {
                int start = glm::min(m_cursorPos, m_secondCursorPos);
                int end = glm::max(m_cursorPos, m_secondCursorPos);
                int length = end - start;

                // Substring
                std::string substring = getText();
                substring.replace(start, length, clipboard);
                setText(substring.c_str());

                m_cursorPos = start + (int) strlen(clipboard);
                m_secondCursorPos = m_cursorPos;
                autoScroll(5.0f);
            }
        }
    }
}

void HCPTextField::handleMouseInputs()
{
    if(!m_inputContext || !m_inputContext->isMButtonHeld(GLFW_MOUSE_BUTTON_1)) return;

    float scroll = m_textScroll.getValuef();
    float cursorX = localCursorX() - edgeSize - scroll;
    float stringWidth = hcpui::getStringwidth(getText(), m_textSize);

    if(stringWidth < cursorX)
    {
        m_cursorPos = (int) strlen(getText());
    }
    else
    {
        m_cursorPos = hcpui::getFontRenderer()->charsToFit(getText(), cursorX);
    }

    if(isPressed())
    {
        m_secondCursorPos = m_cursorPos;
    }

    autoScroll(0.8f);
}

void HCPTextField::handleCharInputs()
{
    if(!m_focused || !m_inputContext || !m_inputContext->charWasTyped()) return;
    
    int charTyped = m_inputContext->getTypedChar();

    if(m_secondCursorPos == m_cursorPos)
    {
        // Substring
        std::string substring = getText();
        substring.insert(m_cursorPos, 1, charTyped);
        setText(substring.c_str());

        m_cursorPos++;
    }
    else
    {
        int start = glm::min(m_cursorPos, m_secondCursorPos);
        int end = glm::max(m_cursorPos, m_secondCursorPos);
        int length = end - start;

        // Substring
        std::string substring = getText();
        substring.replace(start, length, 1, charTyped);
        setText(substring.c_str());

        m_cursorPos = start + 1;
    }

    m_secondCursorPos = m_cursorPos;
    s_lastTimeCursorMoved = glfwGetTime();
    autoScroll(5.0f);
}

void HCPTextField::handleBackSpace()
{
    if(m_secondCursorPos == m_cursorPos)
    {
        if(m_cursorPos > 0)
        {
            // Substring
            std::string substring = getText();
            substring.erase(m_cursorPos - 1, 1);
            setText(substring.c_str());

            m_cursorPos--;
        }
    }
    else
    {
        int start = glm::min(m_cursorPos, m_secondCursorPos);
        int end = glm::max(m_cursorPos, m_secondCursorPos);
        int length = end - start;

        // Substring
        std::string substring = getText();
        substring.erase(start, length);
        setText(substring.c_str());

        m_cursorPos = start;
    }

    m_secondCursorPos = m_cursorPos;
    s_lastTimeCursorMoved = glfwGetTime();
    autoScroll(2.5f);
}

void HCPTextField::positionTitle()
{
    size_t textLen = strlen(getText());

    if(textLen)
    {
        m_titlePosition.grab(hcpui::getStringwidth(getText(), m_textSize) + edgeSize);
        m_titleOpacity.grab(0.0);
    }
    else
    {
        m_titlePosition.grab(edgeSize);
        m_titleOpacity.grab(1.0);
    }
}

void HCPTextField::autoScroll(float amount)
{
    float cursorX = hcpui::getStringwidth(getText(), m_cursorPos, m_textSize);
    float localCursorX = cursorX + edgeSize + (float) m_textScroll.grabbingTo();
    float scrollAmount = m_textSize * amount;

    if(localCursorX < edgeSize)
    {
        m_textScroll.grab(glm::min(0.0, m_textScroll.grabbingTo() + scrollAmount));
    }
    else if(width - edgeSize < localCursorX)
    {
        m_textScroll.grab(m_textScroll.grabbingTo() - scrollAmount);
    }
}