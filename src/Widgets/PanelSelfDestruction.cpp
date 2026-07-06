#include "Widgets/PanelSelfDestruction.h"

#include "Widgets/ButtonObjectActionOption.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

// ===== DIALOG =====
PanelSelfDestruction::PanelSelfDestruction()
{
    using namespace sgl;

    auto sm = utilities::StringManager::Instance();

    SetResizePolicy(sgui::Widget::GROW_ONLY);

    const int marginB = 10;

    // BUTTON DESTROY
    const int tooltipTime1 = 3000;
    mBtnDestroy = new ButtonObjectActionOption(sm->GetCString("DESTROY"),
                                               "1", core::KeyboardEvent::KEY_1, this);
    mBtnDestroy->CreateTooltip(sm->GetCString("TT_DESTROY"), tooltipTime1);

    // BUTTON BLOW UP
    const int tooltipTime2 = 4000;
    mBtnBlowup = new ButtonObjectActionOption(sm->GetCString("BLOW_UP"),
                                              "2", core::KeyboardEvent::KEY_2, this);
    mBtnBlowup->CreateTooltip(sm->GetCString("TT_BLOW_UP"), tooltipTime2);

    mBtnBlowup->SetY(mBtnDestroy->GetHeight() + marginB);

    // play sound
    auto ap = media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/panel-open-01.ogg");
}

PanelSelfDestruction::~PanelSelfDestruction()
{
    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/panel-close-01.ogg");
}

void PanelSelfDestruction::AddFunctionOnDestroy(const std::function<void()> & f)
{
    mBtnDestroy->AddOnClickFunction(f);
}

void PanelSelfDestruction::AddFunctionOnBlowup(const std::function<void()> & f)
{
    mBtnBlowup->AddOnClickFunction(f);
}

void PanelSelfDestruction::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mBtnDestroy->SetText(sm->GetCString("DESTROY"));
    mBtnDestroy->SetTooltipText(sm->GetCString("TT_DESTROY"));

    mBtnBlowup->SetText(sm->GetCString("BLOW_UP"));
    mBtnBlowup->SetTooltipText(sm->GetCString("TT_BLOW_UP"));
}

} // namespace game
