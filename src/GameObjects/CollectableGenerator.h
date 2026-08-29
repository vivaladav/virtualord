#pragma once

#include "GameObjects/GameObjectTypes.h"

namespace sgl { namespace utilities { class BinaryFile; } }

namespace game
{

class Game;
class GameMap;

class CollectableGenerator
{
public:
    CollectableGenerator(const Game * g, GameMap * gm, GameObjectTypeId productType);
    CollectableGenerator(const Game * g, GameMap * gm, int turnsMin, int turnsMax,
                         GameObjectTypeId productType);
    virtual ~CollectableGenerator();

    virtual bool Load(sgl::utilities::BinaryFile & bf);
    virtual bool Save(sgl::utilities::BinaryFile & bf) const;

    void SetCell(int row, int col);
    int GetRow() const;
    int GetCol() const;

    GameObjectTypeId GetProductType() const;

    void OnNewTurn();

    void Update(float delta);

protected:
    GameMap * GetGameMap() const;

private:
    void ResetCounter(int min, int max);

    virtual void OnGeneration();

private:
    friend class StepGameSetCollectableGeneratorTurns;

    const Game * mGame = nullptr;
    GameMap * mGameMap = nullptr;

    GameObjectTypeId mProductType;

    int mRegenTurns = 0;
    int mCounterRegen = 0;

    int mRow = -1;
    int mCol = -1;
};

inline void CollectableGenerator::SetCell(int row, int col)
{
    mRow = row;
    mCol = col;
}

inline int CollectableGenerator::GetRow() const { return mRow; }

inline int CollectableGenerator::GetCol() const { return mCol; }

inline GameObjectTypeId CollectableGenerator::GetProductType() const
{
    return mProductType;
}

inline GameMap * CollectableGenerator::GetGameMap() const
{
    return mGameMap;
}

} // namespace game
