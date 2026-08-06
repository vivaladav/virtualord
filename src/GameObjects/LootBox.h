#pragma once

#include "GameObject.h"

namespace game
{

class LootBox : public GameObject
{
public:
    enum Prize : unsigned int
    {
        LB_BLOBS,
        LB_DIAMONDS,
        LB_ENERGY,
        LB_MATERIAL,
        LB_MONEY,
        LB_RESEARCH,

        NUM_LB_PRIZES,

        LB_NULL
    };

public:
    LootBox(const ObjectData & data, const ObjectInitData & initData);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    bool IsExploding() const;

    void Open(Player * p);

private:
    void UpdateGraphics() override;

    void SetPrize();

    void SetImage();
    void SetObjColors();

private:
    friend class StepGameSetLootboxPrize;

    int mPrizeQuantity = 0;
    Prize mPrizeType = LB_NULL;
};


} // namespace game
