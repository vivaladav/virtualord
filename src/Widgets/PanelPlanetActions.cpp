#include "Widgets/PanelPlanetActions.h"

#include "GameConstants.h"
#include "Widgets/ButtonPlanetMap.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

PanelPlanetActions::PanelPlanetActions()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_ACTIONS);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    const char * fileFont = "Lato-Regular.ttf";
    const unsigned int colorTitle = 0xe9f7fbcc;

    graphic::Font * fnt = fm->GetFont(fileFont, WidgetsConstants::FontSizePlanetMapTitle,
                                      graphic::Font::NORMAL);
    mTitle = new graphic::Text("ACTIONS", fnt);
    mTitle->SetColor(colorTitle);
    RegisterRenderable(mTitle);

    // BUTTONS
    mButtons[EXPLORE] = new ButtonPlanetMap(this);
    mButtons[EXPLORE]->SetLabel("EXPLORE");
    mButtons[EXPLORE]->SetTooltipText("Send scouts to explore the territory");

    mButtons[CONQUER] = new ButtonPlanetMap(this);
    mButtons[CONQUER]->SetLabel("CONQUER");
    mButtons[CONQUER]->SetTooltipText("Start a mission to conquer the territory");

    mButtons[SEND_AI] = new ButtonPlanetMap(this);
    mButtons[SEND_AI]->SetLabel("SEND AI");
    mButtons[SEND_AI]->SetTooltipText("Send an AI to conquer the territory");

    // position elements
    UpdatePositions();
}

void PanelPlanetActions::AddOnButtonClickFunction(Button btn, const std::function<void()> & f)
{
    if(btn < NUM_BUTTONS)
        mButtons[btn]->AddOnClickFunction(f);
}

void PanelPlanetActions::UpdateButtons(TerritoryStatus status, bool isPlayerOccupier)
{
    const bool explored = TER_ST_FREE == status ||
                          TER_ST_OCCUPIED == status;

    mButtons[EXPLORE]->SetEnabled(!explored);
    mButtons[CONQUER]->SetEnabled(!isPlayerOccupier);
    mButtons[SEND_AI]->SetEnabled(!isPlayerOccupier);
}

sgl::sgui::AbstractButton * PanelPlanetActions::GetButton(Button btn) const
{
    if(btn < NUM_BUTTONS)
        return mButtons[btn];
    else
        return nullptr;
}

void PanelPlanetActions::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelPlanetActions::HandleStateEnabled()
{
    mButtons[EXPLORE]->SetEnabled(true);
    mButtons[CONQUER]->SetEnabled(true);
    mButtons[SEND_AI]->SetEnabled(true);
}

void PanelPlanetActions::HandleStateDisabled()
{
    mButtons[EXPLORE]->SetEnabled(false);
    mButtons[CONQUER]->SetEnabled(false);
    mButtons[SEND_AI]->SetEnabled(false);
}

void PanelPlanetActions::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    const int marginL = 20;
    const int marginT = 15;

    int x;
    int y;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // TITLE
    x = x0 + marginL;
    y = y0 + marginT;

    mTitle->SetPosition(x, y);

    // BUTTONS
    const int marginButtons = 40;
    const int buttonBlockH = 100;

    x = (GetWidth() - mButtons[EXPLORE]->GetWidth()) * 0.5f;
    y = marginT + mTitle->GetHeight() + marginButtons;

    mButtons[EXPLORE]->SetPosition(x, y);

    y += buttonBlockH;

    mButtons[CONQUER]->SetPosition(x, y);

    y += buttonBlockH;

    mButtons[SEND_AI]->SetPosition(x, y);
}

} // namespace game
