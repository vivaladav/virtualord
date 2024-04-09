#include "Widgets/Tutorial/PanelClickFilter.h"

#include <sgl/core/event/MouseButtonEvent.h>
#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Renderer.h>

namespace game
{

PanelClickFilter::PanelClickFilter()
{
    auto renderer = sgl::graphic::Renderer::Instance();
    const int rendW = renderer->GetWidth();
    const int rendH = renderer->GetHeight();

    SetSize(rendW, rendH);
}

void PanelClickFilter::HandleMouseButtonDown(sgl::core::MouseButtonEvent & event)
{
    FilterMouseEvent(event);
}

void PanelClickFilter::HandleMouseButtonUp(sgl::core::MouseButtonEvent & event)
{
    FilterMouseEvent(event);
}

void PanelClickFilter::FilterMouseEvent(sgl::core::MouseButtonEvent & event)
{
    const int x = event.GetX();
    const int y = event.GetY();

    if(mAreaWorld)
    {
        auto cam = sgl::graphic::Camera::GetDefaultCamera();

        const int wX = cam->GetScreenToWorldX(x);
        const int wY = cam->GetScreenToWorldY(y);

        if(wX < mXtl || wX > mXbr || wY < mYtl || wY > mYbr)
            event.SetConsumed();
    }
    else
    {
        if(x < mXtl || x > mXbr || y < mYtl || y > mYbr)
            event.SetConsumed();
    }
}

} // namespace game
