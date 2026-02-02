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
    LootBox(const ObjectData & data, const ObjectInitData & initData);

    int GetPrizeQuantity() const;
    Prize GetPrizeType() const;

    void Collected(Player * collector) override;

private:
    void UpdateGraphics() override;

    void SetPrize();

    void SetImage();
    void SetObjColors();

private:
    int mPrizeQuantity = 0;
    Prize mPrizeType = LB_NULL;
};

inline int LootBox::GetPrizeQuantity() const { return mPrizeQuantity; }
inline LootBox::Prize LootBox::GetPrizeType() const { return mPrizeType; }

} // namespace game
