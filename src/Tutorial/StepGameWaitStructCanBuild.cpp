#include "Tutorial/StepGameWaitStructCanBuild.h"

#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWaitStructCanBuild::StepGameWaitStructCanBuild(const Player * p, const ObjectData & buildData,
                                                       GameObjectTypeId structType, const char * text)
    : TutorialInfoStep(550, 130)
    , mPlayer(p)
    , mData(buildData)
    , mStruct(structType)
{
    // OPTONAL INFO
    if(text != nullptr)
    {
        auto sm = sgl::utilities::StringManager::Instance();

        auto info = GetPanelInfo();
        info->AddInfoEntry(sm->GetCString(text), 0.f, false, false);
    }
}

void StepGameWaitStructCanBuild::OnStart()
{
    TutorialInfoStep::OnStart();

    GetClickFilter()->SetEnabled(false);
}

void StepGameWaitStructCanBuild::Update(float delta)
{
    auto obj = mPlayer->GetSelectedObject();

    // no selected object
    if(obj == nullptr)
        return ;

    const float minEnergy = obj->GetEnergyForActionStep(ACT_BUILD_UNIT);

    // selecte object is not an unit OR can't build OR doesn't have enough energy
    if(obj->GetObjectType() != mStruct || obj->GetEnergy() < minEnergy)
       return;

    // check if player can afford building
    const auto & costs = mData.GetCosts();
    const bool costOk = mPlayer->HasEnough(ER_ENERGY, costs[ER_ENERGY]) &&
                        mPlayer->HasEnough(ER_MATERIAL, costs[ER_MATERIAL]) &&
                        mPlayer->HasEnough(ER_DIAMONDS, costs[ER_DIAMONDS]) &&
                        mPlayer->HasEnough(ER_BLOBS, costs[ER_BLOBS]);

    if(costOk)
        SetDone();
}

} // namespace game
