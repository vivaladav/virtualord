#include "Tutorial/StepGameSetCollectableGeneratorTurns.h"

#include "GameMap.h"
#include "GameObjects/CollectableGenerator.h"

namespace game
{

StepGameSetCollectableGeneratorTurns::StepGameSetCollectableGeneratorTurns(const Cell2D & tl,
                                                                           const Cell2D & br,
                                                                           const GameMap * gm,
                                                                           GameObjectTypeId type,
                                                                           int min, int max)
    : mTL(tl)
    , mBR(br)
    , mGameMap(gm)
    , mGenType(type)
    , mMin(min)
    , mMax(max)
{
}

void StepGameSetCollectableGeneratorTurns::OnStart()
{
    TutorialStep::OnStart();

    const std::vector<CollectableGenerator *> & gens = mGameMap->GetCollectableGenerators();

    for(CollectableGenerator * gen : gens)
    {
        const int genR = gen->GetRow();
        const int genC = gen->GetCol();

        if(gen->GetProductType() == mGenType &&
           genR >= mTL.row && genR <= mBR.row && genC >= mTL.col && genC <= mBR.col)
            gen->ResetCounter(mMin, mMax);
    }

    SetDone();
}

} // namespace game
