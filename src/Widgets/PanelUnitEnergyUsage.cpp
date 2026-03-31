#include "PanelUnitEnergyUsage.h"

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

#include <string>

namespace game
{

PanelUnitEnergyUsage::PanelUnitEnergyUsage()
{
    using namespace sgl;

    // panel is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePanelUnitActions, ID_PUA_ENERGY_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // ENERGY COST
    const int sizeText = 16;
    auto font = fm->GetFont(WidgetsConstants::FontFilePanelText, sizeText, graphic::Font::NORMAL);

    mLabel = new sgui::Label(font, this);
    mLabel->SetColor(WidgetsConstants::colorPanelText);

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_16);
    mIcon = new sgui::Image(tex, this);
}

void PanelUnitEnergyUsage::SetValue(int val)
{
    mLabel->SetText(std::to_string(val).c_str());

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    const int marginR = 5;
    const int rowW = mLabel->GetWidth() + marginR + mIcon->GetWidth();

    int x = (w - rowW) / 2;
    int y = (h - mIcon->GetHeight()) / 2;
    mIcon->SetPosition(x, y);

    x += mIcon->GetWidth() + marginR;
    y = (h - mLabel->GetHeight()) / 2;
    mLabel->SetPosition(x, y);
}

void PanelUnitEnergyUsage::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelUnitEnergyUsage::UpdatePositions()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

} // namespace sgl
