#include "Tutorial/StepGameWaitUnitCanSpawn.h"

#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/Unit.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWaitUnitCanSpawn::StepGameWaitUnitCanSpawn(const Player * p, const ObjectData & buildData,
                                                   unsigned int squads, unsigned int elements,
                                                   const char * text)
    : TutorialInfoStep(550, 130)
    , mPlayer(p)
    , mData(buildData)
    , mSquads(squads)
{
    // OPTONAL INFO
    if(text != nullptr)
    {
        auto sm = sgl::utilities::StringManager::Instance();

        auto info = GetPanelInfo();
        info->AddInfoEntry(sm->GetCString(text), 0.f, false, false);
    }
}

void StepGameWaitUnitCanSpawn::OnStart()
{
    TutorialInfoStep::OnStart();

    GetClickFilter()->SetEnabled(false);
}

void StepGameWaitUnitCanSpawn::Update(float delta)
{
    auto obj = mPlayer->GetSelectedObject();

    // no selected object
    if(obj == nullptr)
        return ;

    const float minEnergy = obj->GetEnergyForActionStep(ACT_SPAWN) * mSquads;

    // selecte object is not an unit OR can't build OR doesn't have enough energy
    if(obj->GetObjectCategory() != ObjectData::CAT_UNIT ||
       !static_cast<Unit *>(obj)->CanSpawn() || obj->GetEnergy() < minEnergy)
       return;

    // check if player can afford building
    const unsigned int totElements = mElements * mSquads;
    const auto & costs = mData.GetCosts();
    const bool costOk = mPlayer->HasEnough(ER_ENERGY, costs[ER_ENERGY] * totElements) &&
                        mPlayer->HasEnough(ER_MATERIAL, costs[ER_MATERIAL]* totElements) &&
                        mPlayer->HasEnough(ER_DIAMONDS, costs[ER_DIAMONDS] * totElements) &&
                        mPlayer->HasEnough(ER_BLOBS, costs[ER_BLOBS] * totElements);

    if(costOk)
        SetDone();
}

} // namespace game
