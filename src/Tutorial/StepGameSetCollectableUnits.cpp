#include "Tutorial/StepGameSetCollectableUnits.h"

#include "GameMap.h"
#include "GameObjects/Collectable.h"

namespace game
{

StepGameSetCollectableUnits::StepGameSetCollectableUnits(const Cell2D & tl, const Cell2D & br,
                                                       const GameMap * gm, GameObjectTypeId type,
                                                       int min, int max)
    : mTL(tl)
    , mBR(br)
    , mGameMap(gm)
    , mGenType(type)
    , mMin(min)
    , mMax(max)
{
}

void StepGameSetCollectableUnits::OnStart()
{
    TutorialStep::OnStart();

    const std::vector<GameObject *> & objs = mGameMap->GetObjects();

    for(GameObject * obj : objs)
    {
        const int genR = obj->GetRow0();
        const int genC = obj->GetCol0();

        if(obj->GetObjectType() == mGenType &&
           genR >= mTL.row && genR <= mBR.row && genC >= mTL.col && genC <= mBR.col)
            static_cast<Collectable *>(obj)->RandomizeNumUnits(mMin, mMax);
    }

    SetDone();
}

} // namespace game
