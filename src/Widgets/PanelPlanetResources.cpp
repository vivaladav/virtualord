#include "Widgets/PanelPlanetResources.h"

#include "GameConstants.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/Point.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

PanelPlanetResources::PanelPlanetResources()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_RESOURCES);
    mBg = new sgl::graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // HEADER
    const unsigned int colorHeader = 0xe9f7fbcc;
    const int marginHeaderX = 20;
    const int marginHeaderY = 15;

    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelTitle,
                                      WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mTitle = new sgui::Label(sm->GetCString("RESOURCES"), fnt, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);
    mTitle->SetPosition(marginHeaderX, marginHeaderY);

    // ICONS
    mIcons.assign(NUM_RESOURCES, nullptr);

    const unsigned int texIdsIcons[NUM_RESOURCES] =
    {
        IND_PM_ICON_ENERGY,
        IND_PM_ICON_MATERIAL,
        IND_PM_ICON_DIAMONDS,
        IND_PM_ICON_BLOBS
    };

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        tex = tm->GetSprite(SpriteFilePlanetMap, texIdsIcons[i]);
        mIcons[i] = new graphic::Image(tex);
        RegisterRenderable(mIcons[i]);
    }

    // BARS
    mBars.assign(NUM_RESOURCES, nullptr);
    mBarsValue.assign(NUM_RESOURCES, 0);

    const unsigned int texIdBar = IND_PM_PIPS_BAR_0;

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        tex = tm->GetSprite(SpriteFilePlanetMap, texIdBar);
        mBars[i] = new graphic::Image(tex);
        RegisterRenderable(mBars[i]);
    }

    UpdatePositions();
}

void PanelPlanetResources::SetResourceValue(unsigned int res, unsigned int value)
{
    // check data is valid
    if(res >= NUM_RESOURCES)
        return ;

    if(mBarsValue[res] == value)
        return ;

    const int maxVal = 10;

    if(value > maxVal)
        return ;

    // update value
    mBarsValue[res] = value;

    // update texture
    auto tm = sgl::graphic::TextureManager::Instance();
    const unsigned int texIdBar = IND_PM_PIPS_BAR_0 + mBarsValue[res];
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, texIdBar);
    mBars[res]->SetTexture(tex);
}

void PanelPlanetResources::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelPlanetResources::HandleStateEnabled()
{
    // ICONS
    const unsigned int texIdsIcons[NUM_RESOURCES] =
    {
        IND_PM_ICON_ENERGY,
        IND_PM_ICON_MATERIAL,
        IND_PM_ICON_DIAMONDS,
        IND_PM_ICON_BLOBS
    };

    auto tm = sgl::graphic::TextureManager::Instance();

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, texIdsIcons[i]);
        mIcons[i]->SetTexture(tex);
    }

    // BARS
    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        const unsigned int texIdBar = IND_PM_PIPS_BAR_0 + mBarsValue[i];
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, texIdBar);
        mBars[i]->SetTexture(tex);
    }
}

void PanelPlanetResources::HandleStateDisabled()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // ICONS
    const unsigned int texIdsIcons[NUM_RESOURCES] =
    {
        IND_PM_ICON_ENERGY_DIS,
        IND_PM_ICON_MATERIAL_DIS,
        IND_PM_ICON_DIAMONDS_DIS,
        IND_PM_ICON_BLOBS_DIS
    };

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, texIdsIcons[i]);
        mIcons[i]->SetTexture(tex);
    }

    // BARS
    const unsigned int texIdBar = IND_PM_PIPS_BAR_DIS;

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, texIdBar);
        mBars[i]->SetTexture(tex);
    }
}

void PanelPlanetResources::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // ICONS
    const int iconX0 = x0 + 20;
    const int iconX1 = x0 + 260;
    const int iconY0 = y0 + 70;
    const int iconY1 = y0 + 140;

    const sgl::core::Pointd2D posIcons[NUM_RESOURCES] =
    {
        { iconX0, iconY0 },
        { iconX1, iconY0 },
        { iconX0, iconY1 },
        { iconX1, iconY1 },
    };

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
        mIcons[i]->SetPosition(posIcons[i].x, posIcons[i].y);

    // BARS
    const int marginBarX = 10;

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        const int barX = posIcons[i].x + mIcons[i]->GetWidth() + marginBarX;
        const int barY = posIcons[i].y + (mIcons[i]->GetHeight() - mBars[i]->GetHeight()) * 0.5f;

        mBars[i]->SetPosition(barX, barY);
    }
}

void PanelPlanetResources::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mTitle->SetText(sm->GetCString("RESOURCES"));
}

} // namespace game
