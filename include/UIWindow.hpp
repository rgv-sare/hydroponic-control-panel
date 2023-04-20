#ifndef HCP_UIWINDOW_HPP
#define HCP_UIWINDOW_HPP

#include "Widget.hpp"
#include "Button.hpp"
#include "Viewport.hpp"

#include <list>

class HCPUIWindow : public HCPWidget
{
public:
    static std::list<HCPUIWindow*> s_windows;

    template <typename T, typename... Args>
    static T* createWindow(Args&&... args)
    {
        int newZLevel = (int) s_windows.size() + 1;
        HCPWidget::setCurrentZLevel(newZLevel);

        T* window = new T(std::forward<Args>(args)...);
        s_windows.push_back(window);
        return window;
    }
    static void drawWindows();
    static void closeWindows();
    static bool areWindowsOpen();

    HCPUIWindow(const char* title);
    HCPUIWindow(const HCPUIWindow& copy) = delete;
    ~HCPUIWindow();
protected:
    HCPViewport m_viewport;
    //virtual void drawContents() = 0;
private:
    class CloseButton : public HCPButton
    {
    public:
        CloseButton();
    protected:
        void doDraw() override;
    };

    std::string m_title;
    bool m_shouldClose;
    CloseButton m_closeButton;

    void setShouldClose(bool shouldClose);
    bool shouldClose() const;

    void draw();
};

#endif // HCP_UIWINDOW_HPP