#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class LootBox;

class StepGameSetLootboxPrize : public TutorialStep
{
public:
    StepGameSetLootboxPrize(LootBox * lb, unsigned int prize, int quantity);

    void OnStart() override;

private:
    LootBox * mLootbox = nullptr;

    unsigned int mPrize;
    unsigned int mQuantity;
};

} // namespace game
