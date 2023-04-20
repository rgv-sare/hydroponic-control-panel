#ifndef HCP_UIWINDOW_HPP
#define HCP_UIWINDOW_HPP

#include "Widget.hpp"
#include "Button.hpp"
#include "Viewport.hpp"

#include <list>
#include <memory>

class HCPUIWindow : public HCPWidget
{
public:
    static std::list<std::shared_ptr<HCPUIWindow>> s_windows;

    template <typename T, typename... Args>
    static std::weak_ptr<T> createWindow(Args&&... args)
    {
        int newZLevel = (int) s_windows.size() + 1;
        HCPWidget::setCurrentZLevel(newZLevel);

        std::shared_ptr<T> window = std::make_shared<T>(std::forward<Args>(args)...);
        s_windows.push_back(window);
        return window;
    }
    static void drawWindows();
    static void closeWindows();
    static bool areWindowsOpen();

    HCPUIWindow(const char* title);
    HCPUIWindow(const HCPUIWindow& copy) = delete;
    ~HCPUIWindow();

    void setShouldClose(bool shouldClose);
    bool shouldClose() const;
protected:
    HCPViewport m_viewport;
    virtual void drawContents();
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

    void draw();
};

#endif // HCP_UIWINDOW_HPP