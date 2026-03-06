#include "Widgets/PanelPlanetActions.h"

#include "MapsRegistry.h"
#include "Widgets/ButtonPlanetMap.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr int marginL = 20;
constexpr int marginT = 15;
}

namespace game
{

PanelPlanetActions::PanelPlanetActions()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_ACTIONS);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelTitle,
                                      WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mTitle = new sgui::Label(sm->GetCString("ACTIONS"), fnt, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);
    mTitle->SetPosition(marginL, marginT);

    // BUTTONS
    mButtons[EXPLORE] = new ButtonPlanetMap(this);
    mButtons[EXPLORE]->SetLabel(sm->GetCString("EXPLORE"));
    mButtons[EXPLORE]->SetTooltipText(sm->GetCString("TT_PA_EXPLORE"));

    mButtons[CONQUER] = new ButtonPlanetMap(this);
    mButtons[CONQUER]->SetLabel(sm->GetCString("CONQUER"));
    mButtons[CONQUER]->SetTooltipText(sm->GetCString("TT_PA_CONQUER"));

    mButtons[SEND_AI] = new ButtonPlanetMap(this);
    mButtons[SEND_AI]->SetLabel(sm->GetCString("SEND_AI"));
    mButtons[SEND_AI]->SetTooltipText(sm->GetCString("TT_PA_SEND_AI"));

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

    int x;
    int y;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

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

void PanelPlanetActions::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mTitle->SetText(sm->GetCString("ACTIONS"));

    mButtons[EXPLORE]->SetLabel(sm->GetCString("EXPLORE"));
    mButtons[EXPLORE]->SetTooltipText(sm->GetCString("TT_PA_EXPLORE"));

    mButtons[CONQUER]->SetLabel(sm->GetCString("CONQUER"));
    mButtons[CONQUER]->SetTooltipText(sm->GetCString("TT_PA_CONQUER"));

    mButtons[SEND_AI]->SetLabel(sm->GetCString("SEND_AI"));
    mButtons[SEND_AI]->SetTooltipText(sm->GetCString("TT_PA_SEND_AI"));
}
} // namespace game
