#include "PanelUnitResourcesUsage.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

PanelUnitResourcesUsage::PanelUnitResourcesUsage()
    : mBg(new sgl::graphic::Image)
{
    using namespace sgl;

    // panel is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    // BACKGROUND
    RegisterRenderable(mBg);

    // init graphics
    UpdateGraphics();
}

void PanelUnitResourcesUsage::SetValue(int val)
{

}

void PanelUnitResourcesUsage::SetDoable(bool doable)
{
    if(doable == mDoable)
        return ;

    mDoable = doable;

    UpdateGraphics();
}

void PanelUnitResourcesUsage::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelUnitResourcesUsage::UpdateGraphics()
{
    using namespace sgl;

    // INIT VARIABLES
    unsigned int texId;

    if(mDoable)
    {
        texId = ID_PUA_RESOURCES_BG;

    }
    else
    {
        texId = ID_PUA_RESOURCES_BG2;
    }

    // BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFilePanelUnitActions, texId);
    mBg->SetTexture(tex);

    // UPDATE SIZE
    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

}

void PanelUnitResourcesUsage::UpdatePositions()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

} // namespace sgl
