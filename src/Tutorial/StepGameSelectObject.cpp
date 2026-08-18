#include "Tutorial/StepGameSelectObject.h"

#include "Game.h"
#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameSelectObject::StepGameSelectObject(const Game * game, const IsoMap * im, const GameObject * obj,
                                           const char * text, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(500, 150)
    , mFocusArea(new IsoFocusArea(im))
    , mObj(obj)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString(text), 0.f, false, false, [this, game]
                        {
                            // FOCUS AREA
                            mFocusArea->SetCellArea(mObj->GetRow0(), mObj->GetCol0(),
                                                    mObj->GetRow1(), mObj->GetCol1());
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            const auto isoObj = mObj->GetIsoObject();
                            const int objX = isoObj->GetX();
                            const int objY = isoObj->GetY();
                            const int objW = isoObj->GetWidth();
                            const int objH = isoObj->GetHeight();

                            auto cf = GetClickFilter();
                            cf->SetWorldClickableArea(objX, objY, objW, objH);
                            cf->SetButtonToAllow(game->GetButtonSelect());
                        });
}

StepGameSelectObject::~StepGameSelectObject()
{
    delete mFocusArea;
}

void StepGameSelectObject::Update(float)
{
    if(mObj->IsSelected())
        SetDone();
}

} // namespace game
