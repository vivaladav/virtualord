#pragma once

#include "Structure.h"

namespace game
{

class CityBlock : public Structure
{
public:
    enum CityBlockParts : GameObjectVariantId
    {
        CBP_TL1 = 0,
        CBP_T1,
        CBP_T2,
        CBP_TR1,
        CBP_L1,
        CBP_C1,
        CBP_C2,
        CBP_R1,
        CBP_L2,
        CBP_C3,
        CBP_C4,
        CBP_R2,
        CBP_BL1,
        CBP_B1,
        CBP_B2,
        CBP_BR1,

        NUM_CITYBLOCK_PARTS,
    };

public:
    CityBlock(const ObjectData & data, const ObjectInitData & initData,
              GameObjectVariantId part);

    bool IsBorder() const;
    void SetBorder(bool val);

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;

private:
    void UpdateGraphics() override;

    void OnFactionChanged() override;

    void SetImage();

private:
    bool mBorder = false;
};

inline bool CityBlock::IsBorder() const { return mBorder; }
inline void CityBlock::SetBorder(bool val) { mBorder = val; }

} // namespace game
