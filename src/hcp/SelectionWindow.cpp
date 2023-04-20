#include "hcp/SelectionWindow.hpp"

#include "UIRender.hpp"

#include <glm/glm.hpp>

HCPSelectionWindow::HCPSelectionWindow(const char* title, const char* items[], int numItems, int* selected) : 
    HCPUIWindow(title),
    m_selected(selected),
    m_madeSelection(false)
{
    for(int i = 0; i < numItems; i++)
    {
        m_buttons.push_back(new HCPButton(items[i]));
    }

    m_viewport.height = 300;
    m_maxScroll = numItems * 45.0f + 5.0f * (numItems - 1) - m_viewport.height;
}

HCPSelectionWindow::~HCPSelectionWindow()
{
    
}

void HCPSelectionWindow::drawContents()
{
    m_madeSelection = false;

    HCPViewport listViewport;
    listViewport.x = listViewport.y = 0;
    listViewport.width = m_viewport.width;
    listViewport.height = m_viewport.height;

    listViewport.start(true);
    {
        float buttonY = 0.0f;
        for(int i = 0; i < m_buttons.size(); i++)
        {
            HCPButton* button = m_buttons[i];

            button->width = m_viewport.width;
            button->height = 40;
            button->y = buttonY - m_scroll.getValuef();
            button->draw();

            if(button->isPressed())
            {
                if(m_selected) *m_selected = i;
                m_madeSelection = true;
            }

            buttonY += button->height + 5.0f;
        }
    }
    listViewport.end();

    HCPInputContext* input = hcpui::getInputContext();
    if(input->justScrolled())
    {
        float grabbingTo = (float) m_scroll.grabbingTo();
        grabbingTo = glm::max(0.0f, glm::min(grabbingTo + input->scrollDeltaY() * 30.0f, m_maxScroll));
        m_scroll.grab(grabbingTo);
    }
}

bool HCPSelectionWindow::madeSelection() const
{
    return m_madeSelection;
}