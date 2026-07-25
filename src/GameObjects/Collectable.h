#pragma once

#include "GameObject.h"

namespace game
{

class Player;

class Collectable : public GameObject
{
public:
    Collectable(const ObjectData & data, const ObjectInitData & initData,
                unsigned int min, unsigned int max);

    bool Save(sgl::utilities::BinaryFile & bf) const override;

    int GetNumUnits() const;
    void RandomizeNumUnits(unsigned int min, unsigned int max);
    void MaximizeUnits();
    void MinimizeUnits();

    virtual void Collected(Player * collector) = 0;

private:
    unsigned int mMin = 0;
    unsigned int mMax = 0;
    unsigned int mNum = 0;
};

inline int Collectable::GetNumUnits() const { return mNum; }

} // namespace game
