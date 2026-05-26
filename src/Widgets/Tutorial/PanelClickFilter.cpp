#include "Widgets/Tutorial/PanelClickFilter.h"

#include "Cell2D.h"
#include "IsoMap.h"
#include <sgl/core/event/MouseButtonEvent.h>
#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Renderer.h>

namespace game
{

PanelClickFilter::PanelClickFilter()
    : mButton(sgl::core::MouseEvent::BUTTON_NULL)
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
    // check if only 1 button is allowed
    if(mButton != sgl::core::MouseEvent::BUTTON_NULL && event.GetButton() != mButton)
    {
        event.SetConsumed();
        return ;
    }

    const int x = event.GetX();
    const int y = event.GetY();

    auto cam = sgl::graphic::Camera::GetDefaultCamera();
    const int wX = cam->GetScreenToWorldX(x);
    const int wY = cam->GetScreenToWorldY(y);

    // first filter by mouse position
    // position in world
    if(mAreaWorld)
    {
        if(wX < mXtl || wX > mXbr || wY < mYtl || wY > mYbr)
        {
            event.SetConsumed();
            return;
        }
    }
    // position in screen
    else
    {
        if(x < mXtl || x > mXbr || y < mYtl || y > mYbr)
        {
            event.SetConsumed();
            return;
        }
    }

    // then filter by map cell if required
    if(mIsoMap)
    {
        const Cell2D cell = mIsoMap->CellFromWorldPoint(wX, wY);

        // single cell check
        if(mRow != -1)
        {
            if(cell.row != mRow || cell.col != mCol)
            {
                event.SetConsumed();
                return;
            }
        }
        // cells areas check
        else
        {
            bool inside = false;

            for(const CellsArea & area : mCellAreas)
            {
                if(cell.row >= area.rowTL && cell.row <= area.rowBR &&
                   cell.col >= area.colTL && cell.col <= area.colBR)
                {
                    inside = true;
                    break;
                }
            }

            if(!inside)
            {
                event.SetConsumed();
                return;
            }
        }
    }
}

void PanelClickFilter::ClearButtonToAllow()
{
    mButton = sgl::core::MouseEvent::BUTTON_NULL;
}

void PanelClickFilter::ExpandClickableArea()
{
    auto renderer = sgl::graphic::Renderer::Instance();
    const int rendW = renderer->GetWidth();
    const int rendH = renderer->GetHeight();

    SetScreenClickableArea(0, 0, rendW, rendH);
}

} // namespace game
