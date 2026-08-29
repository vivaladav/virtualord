#pragma once

#include "Structure.h"

namespace game
{

class WallGate : public Structure
{
public:
    enum GateOrientation : GameObjectVariantId
    {
        HORIZ,
        VERT,

        NUM_GATE_ORIENTATIONS
    };

public:
    WallGate(const ObjectData & data, const ObjectInitData & initData,
             GameObjectVariantId orientation);
    ~WallGate();

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    bool IsOpen() const;
    bool Toggle();

    void OnNewTurn(PlayerFaction faction) override;

    static unsigned int GetCostEnergy(unsigned int level);
    static unsigned int GetCostMaterial(unsigned int level);

private:
    void OnLinkedChanged() override;

    void UpdateGraphics() override;

    void SetImage();

private:
    bool mOpen = false;
};

inline bool WallGate::IsOpen() const
{
    return mOpen;
}

} // namespace game
