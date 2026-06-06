#include "Tutorial/StepGameSetCollectableGeneratorTurns.h"

#include "GameMap.h"
#include "GameObjects/CollectableGenerator.h"

namespace game
{

StepGameSetCollectableGeneratorTurns::StepGameSetCollectableGeneratorTurns(GameMap * gm,
                                                                           GameObjectTypeId type,
                                                                           int min, int max)
    : mGameMap(gm)
    , mGenType(type)
    , mMin(min)
    , mMax(max)
{
}

void StepGameSetCollectableGeneratorTurns::OnStart()
{
    const std::vector<CollectableGenerator *> & gens = mGameMap->GetCollectableGenerators();

    for(CollectableGenerator * gen : gens)
    {
        if(gen->GetProductType() == mGenType)
            gen->ResetCounter(mMin, mMax);
    }

    SetDone();
}

} // namespace game
