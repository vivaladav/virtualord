#include "Tutorial/StepGameConquerEnergyGenIntro.h"

#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerEnergyGenIntro::StepGameConquerEnergyGenIntro(const GameObject * gen,
                                                             const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const auto isoObj = gen->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_ENE_GEN"), 8.f, true, false, [this]
                       {
                           mFocusArea->SetVisible(true);
                       });

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameConquerEnergyGenIntro::~StepGameConquerEnergyGenIntro()
{
    delete mFocusArea;
}

} // namespace game
