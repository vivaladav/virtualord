#pragma once

namespace game
{

class GameMap;

class CollectableGenerator
{
public:
    CollectableGenerator(GameMap * gm, int turnsMin, int turnsMax);
    virtual ~CollectableGenerator();

    void SetCell(int row, int col);
    int GetRow() const;
    int GetCol() const;

    void OnNewTurn();

    void Update(float delta);

protected:
    GameMap * GetGameMap() const;

private:
    void ResetCounter(int min, int max);

    virtual void OnGeneration() = 0;

private:
    GameMap * mGameMap = nullptr;

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


inline GameMap * CollectableGenerator::GetGameMap() const
{
    return mGameMap;
}

} // namespace game
