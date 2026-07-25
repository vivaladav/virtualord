#pragma once

#include "GameObject.h"

namespace game
{

class Trees : public GameObject
{
public:
    Trees(const ObjectData & data, const ObjectInitData & initData,
          GameObjectVariantId var);

    bool Save(sgl::utilities::BinaryFile & bf) const override;

    void OnNewTurn(PlayerFaction faction) override;

private:
    void SpawnTree(int r0, int c0);

    void UpdateGraphics() override;

    void SetImage();

    void SetObjColors();

private:
    int mLevel = 0;
    int mNumTrees = 1;

    int mTurnsToChange = 1;
    int mTurns = 0;

    unsigned int mNumVariants = 0;
    unsigned int mMaxNum = 0;
};

} // namespace game
