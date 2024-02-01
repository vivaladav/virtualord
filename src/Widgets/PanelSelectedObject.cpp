#include "Widgets/PanelSelectedObject.h"

#include "GameUIData.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ImageButton.h>

namespace game
{

// ========== PANEL ==========

PanelSelectedObject::PanelSelectedObject(sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
{
    using namespace sgl;

    // BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, ID_PAN_SELOBJ_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    PositionElements();
}

void PanelSelectedObject::AddFunctionOnClose(const std::function<void()> & f)
{

}

void PanelSelectedObject::SetObject(GameObject * obj)
{
    // object already set -> exit
    if(obj == mObj)
        return ;
}

void PanelSelectedObject::HandlePositionChanged()
{
    PositionElements();
}

void PanelSelectedObject::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0 ,y0);
}

} // namespace game
