#ifndef HCP_SELECTION_WINDOW_HPP
#define HCP_SELECTION_WINDOW_HPP

#include "UIWindow.hpp"
#include "Button.hpp"
#include "Animation.hpp"

#include <vector>

class HCPSelectionWindow : public HCPUIWindow
{
public:
    HCPSelectionWindow(const char* title, const char* items[], int numItems, int* selected);
    ~HCPSelectionWindow();

    void drawContents() override;
    bool madeSelection() const;
private:
    int* m_selected;
    std::vector<HCPButton*> m_buttons;
    bool m_madeSelection;
    HCPSmoother m_scroll;
    float m_maxScroll;
};

#endif // HCP_SELECTION_WINDOW_HPP