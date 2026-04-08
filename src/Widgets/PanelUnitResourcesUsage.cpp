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

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const int marginL = 15;
    const int marginT = 15;
    const int marginHeaderB = 2;
    const int marginIconR = 5;
    const int marginIconB = 10;

    int x = marginL;
    int y = marginT;

    // panel is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    // BACKGROUND
    RegisterRenderable(mBg);

    // UNIT
    // header
    const int sizeHeader = 16;
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileText, sizeHeader, graphic::Font::NORMAL);
    mHeaderUnit = new sgui::Label(sm->GetCString("UNIT"), fontHeader, this);
    mHeaderUnit->SetColor(WidgetsConstants::colorPanelHeader);
    mHeaderUnit->SetPosition(x, y);

    y += mHeaderUnit->GetHeight() + marginHeaderB;

    // icon energy
    auto tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_16);
    auto icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // value unit energy
    x += icon->GetWidth() + marginIconR;

    const int sizeData = 16;
    auto fontData = fm->GetFont(WidgetsConstants::FontFileText, sizeData, graphic::Font::NORMAL);
    mLabelUnitEnergy = new sgui::Label("0", fontData, this);
    mLabelUnitEnergy->SetPosition(x, y + (icon->GetHeight() - mLabelUnitEnergy->GetHeight()) / 2);

    y += icon->GetHeight() + marginIconB;

    // RESOURCES
    // header
    x = marginL;

    mHeaderRes = new sgui::Label(sm->GetCString("RESOURCES"), fontHeader, this);
    mHeaderRes->SetColor(WidgetsConstants::colorPanelHeader);
    mHeaderRes->SetPosition(x, y);

    y += mHeaderRes->GetHeight() + marginHeaderB;

    // icon energy
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // value resources energy
    x += icon->GetWidth() + marginIconR;

    mLabelResEnergy = new sgui::Label("0", fontData, this);
    mLabelResEnergy->SetPosition(x, y + (icon->GetHeight() - mLabelUnitEnergy->GetHeight()) / 2);

    // icon material
    const int blockResW = 80;
    x = icon->GetX() + blockResW;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MATERIAL_16);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // value resources material
    x += icon->GetWidth() + marginIconR;

    mLabelResMaterial = new sgui::Label("0", fontData, this);
    mLabelResMaterial->SetPosition(x, y + (icon->GetHeight() - mLabelResMaterial->GetHeight()) / 2);

    // init graphics
    UpdateGraphics();
}

void PanelUnitResourcesUsage::SetValues(int unitEnergy, int resEnergy, int resMaterial)
{
    // unit
    mLabelUnitEnergy->SetText(std::to_string(unitEnergy).c_str());
    // resources
    mLabelResEnergy->SetText(std::to_string(resEnergy).c_str());
    mLabelResMaterial->SetText(std::to_string(resMaterial).c_str());
}

void PanelUnitResourcesUsage::SetDoable(bool unitEnergy, bool resEnergy, bool resMaterial)
{
    bool changed = false;

    if(unitEnergy != mDoableUnitEnergy)
    {
        mDoableUnitEnergy = unitEnergy;
        changed = true;
    }

    if(resEnergy != mDoableResEnergy)
    {
        mDoableResEnergy = resEnergy;
        changed = true;
    }

    if(resMaterial != mDoableResMaterial)
    {
        mDoableResMaterial = resMaterial;
        changed = true;
    }

    if(changed)
        UpdateGraphics();
}

void PanelUnitResourcesUsage::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelUnitResourcesUsage::UpdateGraphics()
{
    using namespace sgl;

    const int doable = mDoableUnitEnergy && mDoableResEnergy && mDoableResMaterial;

    // INIT VARIABLES
    const unsigned int texId = doable ? ID_PUA_RESOURCES_BG : ID_PUA_RESOURCES_BG2;

    // BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFilePanelUnitActions, texId);
    mBg->SetTexture(tex);

    // UPDATE SIZE
    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // LABEL UNIT ENERGY
    if(mDoableUnitEnergy)
        mLabelUnitEnergy->SetColor(WidgetsConstants::colorPanelText);
    else
        mLabelUnitEnergy->SetColor(WidgetsConstants::colorDialogBad);

    // LABEL RESOURCE ENERGY
    if(mDoableResEnergy)
        mLabelResEnergy->SetColor(WidgetsConstants::colorPanelText);
    else
        mLabelResEnergy->SetColor(WidgetsConstants::colorDialogBad);

    // LABEL RESOURCE MATERIAL
    if(mDoableResMaterial)
        mLabelResMaterial->SetColor(WidgetsConstants::colorPanelText);
    else
        mLabelResMaterial->SetColor(WidgetsConstants::colorDialogBad);
}

void PanelUnitResourcesUsage::UpdatePositions()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

void PanelUnitResourcesUsage::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mHeaderUnit->SetText(sm->GetCString("UNIT"));
    mHeaderRes->SetText(sm->GetCString("RESOURCES"));
}

} // namespace sgl
