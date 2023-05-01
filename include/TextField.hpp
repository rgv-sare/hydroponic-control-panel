#ifndef HCP_TEXT_FIELD_HPP
#define HCP_TEXT_FIELD_HPP

#include "Widget.hpp"
#include "Viewport.hpp"
#include "Animation.hpp"
#include "Inputs.hpp"

class HCPTextField : public HCPWidget
{
public:
    HCPTextField();
    HCPTextField(const char* title);

    bool isFocused() const;
    void setFocused(bool focused);

    const char* getTitle() const;
    void setTitle(const char* title);
    void setCursors(int cursorPos, int secondCursorPos);
    void setCursorAtStart(bool autoScroll = true);
    void setCursorAtEnd(bool autoScroll = true);

protected:
    void doDraw() override;
private:
    static double s_lastTimeCursorMoved;

    char m_title[256];
    bool m_focused;
    float m_textSize;

    int m_cursorPos;
    int m_secondCursorPos;
    int m_lastCursorPos;

    HCPViewport m_viewport;

    HCPSmoother m_textScroll;
    HCPSmoother m_titlePosition;
    HCPSmoother m_titleOpacity;

    HCPInputContext* m_inputContext;

    void handleKeyInputs();
    void handleMouseInputs();
    void handleCharInputs();
    void handleBackSpace();

    void positionTitle();

    void autoScroll(float amount);
};

#endif // HCP_TEXT_FIELD_HPP