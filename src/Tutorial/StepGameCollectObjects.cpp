#include "Tutorial/StepGameCollectObjects.h"

#include "Game.h"
#include "GameMap.h"
#include "IsoMap.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/IsoFocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameCollectObjects::StepGameCollectObjects(const Unit * unit, GameObjectTypeId objType,
                                               const Game * game, const GameMap * gm, const IsoMap * im,
                                               const Cell2D & areaTL, const Cell2D & areaBR,
                                               const char * intro, const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new IsoFocusArea(im))
    , mAreaTL(areaTL)
    , mAreaBR(areaBR)
    , mGameMap(gm)
    , mUnit(unit)
    , mObjType(objType)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCellArea(areaBR.row, areaBR.col, areaTL.row, areaTL.col);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString(intro), 0.f, false, false, [this, im, game]
                        {
                            mFocusArea->SetVisible(true);

                            auto cf = GetClickFilter();
                            cf->SetButtonToAllow(game->GetButtonAction());
                            cf->AddClickableCells(im, mAreaTL, mAreaBR);
                        });
}

StepGameCollectObjects::~StepGameCollectObjects()
{
    delete mFocusArea;
}

void StepGameCollectObjects::Update(float)
{
    unsigned int objsLeft = 0;

    for(int r = mAreaTL.row; r <= mAreaBR.row; ++r)
    {
        for(int c = mAreaTL.col; c <= mAreaBR.col; ++c)
        {
            objsLeft += mGameMap->HasObjectType(mObjType, r, c);
        }
    }

    const float energyMove = mUnit->GetEnergyForActionStep(MOVE) * 2.f;

    if(objsLeft == 0 || mUnit->GetEnergy() < energyMove)
        SetDone();
}

} // namespace game
