#include "Tutorial/StepGameConquerGeneratorIntro.h"

#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConquerGeneratorIntro::StepGameConquerGeneratorIntro(const GameObject * gen,
                                                             const char * text,
                                                             const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const int objX = gen->GetX();
    const int objY = gen->GetY();
    const int objW = gen->GetWidth();
    const int objH = gen->GetHeight();

    mFocusArea->SetWorldArea(objX, objY, objW, objH, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(text), 8.f, true, false, [this]
                       {
                           mFocusArea->SetVisible(true);
                       });

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

StepGameConquerGeneratorIntro::~StepGameConquerGeneratorIntro()
{
    delete mFocusArea;
}

} // namespace game
