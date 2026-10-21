#include "Tutorial/StepGameObjectAttackSimple.h"

#include "Game.h"
#include "IsoMap.h"
#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameObjectAttackSimple::StepGameObjectAttackSimple(const Game * game,
                                                       const GameObject * Object,
                                                       const IsoMap * isoMap,
                                                       const Cell2D & target,
                                                       const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(500, 130)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mObject(Object)
    , mTarget(target)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_ATTACK_6"), 0.f, false, false, [this, isoMap, game]
                        {
                            // FOCUS
                            mFocusArea->SetCell(mTarget.row, mTarget.col);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->SetClickableCell(isoMap, mTarget.row, mTarget.col);
                        });
}

StepGameObjectAttackSimple::~StepGameObjectAttackSimple()
{
    delete mFocusArea;
}

void StepGameObjectAttackSimple::Update(float)
{
    if(mObject != nullptr && mObject->GetCurrentAction() == GameObjectActionType::ATTACK)
        SetDone();
}

} // namespace game
