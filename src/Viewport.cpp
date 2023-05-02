#include "Viewport.hpp"

#include "UIRender.hpp"
#include "GLInclude.hpp"

HCPViewport::HCPViewport() :
    m_wasClipping(false)
{
    m_type = VIEWPORT;
}

void HCPViewport::start(bool clipping)
{
    draw();

    if(clipping)
    {
        ClippingState clip;
        clip.modelView = m_modelView;
        clip.left = x;
        clip.top = y;
        clip.right = x + width;
        clip.bottom = y + height;
        clip.clipping = true;
        m_clippingStack.push(clip);
        m_wasClipping = true;

        hcpui::renderBatch(); // Render and flush the batch before writing to the stencil buffer
        glClear(GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        hcpui::genQuad(x, y, x + width, y + height, 0xFFFFFFFF);
        glColorMask(0, 0, 0, 0);
        hcpui::renderBatch();
        glColorMask(1, 1, 1, 1);

        glStencilFunc(GL_EQUAL, 1, 0xFF);
    }

    hcpui::pushStack();
    hcpui::translate(x, y);
}

void HCPViewport::end()
{
    if(m_wasClipping)
    {
        hcpui::renderBatch();
        glDisable(GL_STENCIL_TEST);
        m_wasClipping = false;
        m_clippingStack.pop();
    }

    hcpui::popStack();
}

void HCPViewport::doDraw()
{
    
}
