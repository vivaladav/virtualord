#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class IsoFocusArea;
class IsoMap;
class ObjectData;
class Player;

class StepGameWaitUnitCanBuild : public TutorialInfoStep
{
public:
    StepGameWaitUnitCanBuild(const Player * p, const ObjectData & buildData, const char * text,
                             const IsoMap * isoMap, const Cell2D & areaTL, const Cell2D & areaBR);

    void OnStart() override;

    void Update(float delta) override;

private:
    bool CanBuild() const;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Player * mPlayer = nullptr;

    const ObjectData & mData;
};

} // namespace game
