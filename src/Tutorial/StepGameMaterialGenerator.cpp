#include "Tutorial/StepGameMaterialGenerator.h"

#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMaterialGenerator::StepGameMaterialGenerator(const GameObject * materialGen)
    : TutorialInfoStep(700, 300)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const auto isoObj = materialGen->GetIsoObject();
    const int objX = isoObj->GetX();
    const int objY = isoObj->GetY();
    const int objW = isoObj->GetWidth();
    const int objH = isoObj->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH);
    mFocusArea->SetCornersColor(TutorialConstants::colorFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1150, 350);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_MAT_GEN_1"),
                       TutorialConstants::colorText, 9.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_MAT_GEN_2"),
                       TutorialConstants::colorText, 10.f, true, false, [this]
                       {
                           mFocusArea->SetVisible(true);
                       });
    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONQUER_MAT_GEN_3"),
                       TutorialConstants::colorText, 8.f, true, false);

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameMaterialGenerator::~StepGameMaterialGenerator()
{
    delete mFocusArea;
}

} // namespace game
