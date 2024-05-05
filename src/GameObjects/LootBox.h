#pragma once

#include "Collectable.h"

namespace game
{

class LootBox : public Collectable
{
public:
    enum Prize : unsigned int
    {
        LB_BLOBS,
        LB_DIAMONDS,
        LB_ENERGY,
        LB_MATERIAL,
        LB_MONEY,

        NUM_LB_PRIZES,

        LB_NULL
    };

public:
    LootBox();

    int GetPrizeQuantity() const;
    Prize GetPrizeType() const;

    void Collected(Player * collector) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void SetObjColors();

private:
    int mPrizeQuantity = 0;
    Prize mPrizeType = LB_NULL;
};

inline int LootBox::GetPrizeQuantity() const { return mPrizeQuantity; }
inline LootBox::Prize LootBox::GetPrizeType() const { return mPrizeType; }

} // namespace game
