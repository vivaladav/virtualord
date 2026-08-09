#include "Tutorial/StepGameWaitCityConquered.h"

#include "Game.h"
#include "IsoMap.h"
#include "GameObjects/CityGroup.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWaitCityConquered::StepGameWaitCityConquered(CityGroup * city,
                                                     const Cell2D & tl, const Cell2D & br,
                                                     const IsoMap * isoMap, const Game * game,
                                                     const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 150)
    , mFocusArea(new IsoFocusArea(isoMap))
    , mCity(city)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCellArea(br.row, br.col, tl.row, tl.col);
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddActionEntry(sm->GetCString("TUT_GAME_CITY_2"), 0.f, false, false,
                        [this, isoMap, game, tl, br]
                        {
                            // FOCUS
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->AddClickableCells(isoMap, tl.row, tl.col, br.row, br.col);
                        });
}

StepGameWaitCityConquered::~StepGameWaitCityConquered()
{
    delete mFocusArea;
}

void StepGameWaitCityConquered::Update(float)
{
    if(mCity->IsCityConquered())
        SetDone();
}

} // namespace game
