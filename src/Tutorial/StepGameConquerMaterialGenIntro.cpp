#include "Tutorial/StepGameConquerMaterialGenIntro.h"

#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerMaterialGenIntro::StepGameConquerMaterialGenIntro(const GameObject * materialGen,
                                                                 const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const int objX = materialGen->GetX();
    const int objY = materialGen->GetY();
    const int objW = materialGen->GetWidth();
    const int objH = materialGen->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_MAT_GEN"), 8.f, true, false, [this]
                       {
                           mFocusArea->SetVisible(true);
                       });

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameConquerMaterialGenIntro::~StepGameConquerMaterialGenIntro()
{
    delete mFocusArea;
}

} // namespace game
