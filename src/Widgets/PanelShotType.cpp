#include "Widgets/PanelShotType.h"

#include "GameConstants.h"
#include "Widgets/ButtonObjectActionOption.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

// ===== DIALOG =====
PanelShotType::PanelShotType()
    : sgl::sgui::ButtonsGroup(sgl::sgui::ButtonsGroup::VERTICAL)
{
    using namespace sgl;

    auto sm = utilities::StringManager::Instance();

    // spacing between buttons
    const int spacing = 10;
    SetSpacing(spacing);

    // BUTTON QUICK SHOT
    const int tooltipTime1 = 3000;
    auto btn = new ButtonObjectActionOption(sm->GetCString("QUICK_SHOT"),"1",
                                            core::KeyboardEvent::KEY_1, this);
    btn->CreateTooltip(sm->GetCString("TT_QUICK_SHOT"), tooltipTime1);

    AddButton(btn);

    // BUTTON AIMED SHOT
    const int tooltipTime2 = 4000;
    btn = new ButtonObjectActionOption(sm->GetCString("AIMED_SHOT"), "2",
                                       core::KeyboardEvent::KEY_2, this);
    btn->CreateTooltip(sm->GetCString("TT_AIMED_SHOT"), tooltipTime2);

    AddButton(btn);

    // BUTTON BURST SHOT
    btn = new ButtonObjectActionOption(sm->GetCString("BURST_SHOT"), "3",
                                       core::KeyboardEvent::KEY_3, this);
    btn->CreateTooltip(sm->GetCString("TT_BURST_SHOT"), tooltipTime1);
    AddButton(btn);

    SetButtonChecked(0, true);

    // play sound
    auto ap = media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/panel-open-01.ogg");
}

PanelShotType::~PanelShotType()
{
    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/panel-close-01.ogg");
}

void PanelShotType::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    auto btn = static_cast<ButtonObjectActionOption*>(GetButton(ATT_QUICK_SHOT));
    btn->SetText(sm->GetCString("QUICK_SHOT"));
    btn->SetTooltipText(sm->GetCString("TT_QUICK_SHOT"));

    btn = static_cast<ButtonObjectActionOption*>(GetButton(ATT_AIMED_SHOT));
    btn->SetText(sm->GetCString("AIMED_SHOT"));
    btn->SetTooltipText(sm->GetCString("TT_AIMED_SHOT"));

    btn = static_cast<ButtonObjectActionOption*>(GetButton(ATT_BURST_SHOT));
    btn->SetText(sm->GetCString("BURST_SHOT"));
    btn->SetTooltipText(sm->GetCString("TT_BURST_SHOT"));
}
} // namespace game
