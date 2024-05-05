#pragma once

#include "GameMapCell.h"
#include "GameObjects/GameObjectTypes.h"

#include <sgl/ai/IPathMap.h>

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace game
{

class CollectableGenerator;
class ConquerPath;
class ControlMap;
class Game;
class GameObject;
class IsoMap;
class ObjectPath;
class Player;
class ScreenGame;
class Temple;
class Unit;
class WallBuildPath;

struct Cell2D;
struct GameMapCell;
struct ObjectData;

enum ResourceType : unsigned int;

/// Class that handles most of the logic of what happens on the game map.
class GameMap : public sgl::ai::IPathMap
{
public:
    GameMap(Game * game, ScreenGame * sg, IsoMap * isoMap);
    ~GameMap();

    const ControlMap * GetControlMap() const;

    bool HasObject(unsigned int ind) const;
    bool HasObject(unsigned int r, unsigned int c) const;
    bool HasObject(const GameObject * obj) const;
    bool IsObjectVisibleToLocalPlayer(const GameObject * obj) const;

    const std::vector<GameMapCell> & GetCells() const;
    const std::vector<GameObject *> & GetObjects() const;

    bool IsCellVisibleToLocalPlayer(unsigned int ind) const;
    bool IsCellVisibleToLocalPlayer(unsigned int r, unsigned int c) const;
    bool IsAnyCellVisibleToLocalPlayer(unsigned int rTL, unsigned int cTL,
                                       unsigned int rBR, unsigned int cBR) const;
    bool IsCellWalkable(unsigned int cellInd) const;
    bool IsCellWalkable(unsigned int r, unsigned int c) const override;
    bool IsAnyNeighborCellWalkable(unsigned int r, unsigned int c) const;

    void SetCellWalkable(unsigned int cellInd, bool val);
    void SetCellWalkable(unsigned int r, unsigned int c, bool val);
    void SetCellWalkTarget(unsigned int cellInd, bool val);
    void SetCellType(unsigned int r, unsigned int c, CellTypes type);
    void SetCellType(unsigned int ind, CellTypes type);
    void UpdateCellType(unsigned int ind, const GameMapCell & cell);

    bool IsCellObjectVisited(unsigned int cellInd) const;
    bool IsCellObjectVisited(unsigned int r, unsigned int c) const;

    void SetSize(unsigned int rows, unsigned int cols);

    void CreateCollectableGenerator(unsigned int r, unsigned int c, ResourceType type);

    void ApplyLocalVisibility();
    void ApplyVisibility(Player * player);
    void InitVisibility(Player * player);

    void ApplyLocalVisibilityToObject(GameObject * go);
    void ApplyVisibilityToObject(Player * player, GameObject * go);
    void InitObjectVisibility(Player * player, GameObject * go);

    Player * GetCellOwner(unsigned int r, unsigned int c) const;

    bool IsCellChanging(unsigned int r, unsigned int c) const;
    void SetCellChanging(unsigned int r, unsigned int c, bool changing);

    void CreateObjectFromFile(unsigned int layerId, GameObjectTypeId type,
                              GameObjectVariantId variant, unsigned int faction,
                              unsigned int r0, unsigned int c0);

    GameObject * CreateObject(unsigned int layerId, GameObjectTypeId type,
                              GameObjectVariantId variant, PlayerFaction faction,
                              unsigned int r0, unsigned int c0, bool instantAdd);

    bool RemoveAndDestroyObject(GameObject * obj);

    // player stats
    void RegisterEnemyKill(PlayerFaction killer);
    void RegisterCasualty(PlayerFaction killed);
    unsigned int GetEnemiesKilled(PlayerFaction killer) const;
    unsigned int GetCasualties(PlayerFaction faction) const;

    bool AreObjectsAdjacent(const GameObject * obj1, const GameObject * obj2) const;
    bool AreCellsAdjacent(const Cell2D & cell1, const Cell2D & cell2) const;
    bool AreObjectsOrthoAdjacent(const GameObject * obj1, const GameObject * obj2) const;
    bool AreCellsOrthoAdjacent(const Cell2D & cell1, const Cell2D & cell2) const;

    // cell conquest
    bool HasResourcesToConquerCell(Unit * unit);
    bool CanConquerCell(Unit * unit, const Cell2D & cell, Player * player);
    void StartConquerCell(const Cell2D & cell, Player * player);
    void ConquerCell(const Cell2D & cell, Player * player);
    bool ConquerCells(ConquerPath * path);
    bool AbortCellConquest(GameObject * obj);

    // structure building
    bool CanBuildStructure(Unit * unit, const Cell2D & cell, Player * player, GameObjectTypeId st);
    void StartBuildStructure(const Cell2D & cell, Player * player, GameObjectTypeId st);
    void BuildStructure(const Cell2D & cell, Player * player, GameObjectTypeId st);

    // wall building
    bool HasResourcesToBuildWall(Unit * unit, unsigned int level);
    bool CanBuildWall(const Cell2D & cell, Player * player, unsigned int level);
    void StartBuildWall(const Cell2D & cell, Player * player, unsigned int level);
    void BuildWall(const Cell2D & cell, Player * player, GameObjectTypeId planned);
    bool BuildWalls(WallBuildPath * path);
    bool AbortBuildWalls(GameObject * obj);

    // structure conquest
    bool CanConquerStructure(Unit * unit, const Cell2D & end, Player * player);
    void StartConquerStructure(const Cell2D & end, Player * player);
    void AbortConquerStructure(GameObject * target);
    void ConquerStructure(const Cell2D & end, Player * player);

    void HandleTempleExplorationOutcome(unsigned int outcome, Player * p, Temple * temple);

    // unit create
    bool CanCreateUnit(GameObjectTypeId ut, GameObject * gen, Player * player);
    Cell2D GetNewUnitDestination(GameObject * gen);
    void StartCreateUnit(GameObjectTypeId ut, GameObject * gen, const Cell2D & dest, Player * player);
    void CreateUnit(GameObjectTypeId ut, GameObject * gen, const Cell2D & dest, Player * player);

    // move units
    bool CanUnitMove(const Cell2D & start, const Cell2D & end, Player * player) const;
    bool MoveUnit(ObjectPath * path);
    bool AbortMove(GameObject * obj);

    Cell2D GetCloseMoveTarget(const Cell2D & start, const Cell2D & end) const;
    Cell2D GetAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const;
    Cell2D GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & target) const;
    Cell2D GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const;
    Cell2D GetOrthoAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const;
    Cell2D GetOrthoAdjacentMoveTarget(const Cell2D & start, const Cell2D & target) const;
    Cell2D GetOrthoAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const;
    bool FindAttackPosition(const Unit * u, const GameObject * target, Cell2D & pos);
    bool FindAttackPosition(const Unit * u, const GameObject * target, int dist, Cell2D & pos);

    const GameMapCell & GetCell(unsigned int r, unsigned int c) const;
    const GameMapCell & GetCell(unsigned int ind) const;

    bool MoveObjectDown(GameObject * obj);
    bool MoveObjectUp(GameObject * obj);

    unsigned int GetNumRows() const;
    unsigned int GetNumCols() const;

    int ApproxDistance(const Cell2D & c1, const Cell2D & c2) const;
    int ApproxDistance(const GameObject * obj1, const GameObject * obj2) const;
    bool FindClosestCellConnectedToObject(const GameObject * obj, const Cell2D start, Cell2D & end);
    bool FindClosestLinkedCell(PlayerFaction faction, const Cell2D start, Cell2D & linked);
    bool FindFreeArea(const Cell2D & start, int rows, int cols, int maxRadius, Cell2D & target);
    bool IsAreaFree(int brR, int brC, int rows, int cols);

    // turn system
    void OnNewTurn(PlayerFaction faction);

    void Update(float delta);

private:
    void ClearCell(GameMapCell & gcell);

    int DefineCellType(unsigned int ind, const GameMapCell & cell);

    void UpdateLinkedCells(Player * player);

    void UpdateInfluencedCells(int row, int col);

    void UpdateVisibility(Player * player, bool init);

    bool MoveObjToCell(GameObject * obj, int row, int col);

    Cell2D GetClosestCell(const Cell2D & start, const std::vector<Cell2D> & targets) const;

    struct ObjectToAdd;
    void AddObjectToMap(const ObjectToAdd & o2a);
    void DestroyObject(GameObject * obj);

    // -- player visibility --
    void AddVisibilityToCell(Player * player, int ind);
    void DelVisibilityToCell(Player * player, int ind);

    void AddPlayerObjVisibility(GameObject * obj, Player * player);
    void DelPlayerObjVisibility(GameObject * obj, Player * player);
    void PropagatePlayerObjVisibility(GameObject * obj, std::function<void(int)> visFun);

    void AddPlayerCellVisibility(const GameMapCell & cell, Player * player);
    void DelPlayerCellVisibility(const GameMapCell & cell, Player * player);
    void PropagatePlayerCellVisibility(const GameMapCell & cell, std::function<void(int)> visFun);

    void PropagatePlayerVisibility(const Cell2D & cell1, const Cell2D & cell0, int visLevel,
                                   std::function<void(int)> visFun);

    void DestroyObjectPaths(GameObject * obj);
    void UpdateObjectPaths(float delta);
    void UpdateConquerPaths(float delta);
    void UpdateWallBuildPaths(float delta);
    void UpdateObjectsToAdd();

    void UpdateWalls(const Cell2D & center);
    void UpdateWall(const Cell2D & cell);

    const ObjectData & GetObjectData(GameObjectTypeId t) const;

private:
    struct ObjectToAdd
    {
        GameObject * obj;
        unsigned int r0;
        unsigned int c0;
        unsigned int r1;
        unsigned int c1;
        Player * owner;
        unsigned int layer;
    };

    // to access visibility functions
    friend class ConquerPath;
    friend class ObjectPath;
    friend class WallBuildPath;

    std::vector<GameMapCell> mCells;
    std::vector<GameObject *> mObjects;
    std::vector<ObjectToAdd> mObjectsToAdd;
    std::unordered_set<const GameObject *> mObjectsSet;
    std::vector<CollectableGenerator *> mCollGen;
    std::vector<ObjectPath *> mPaths;
    std::vector<ConquerPath *> mConquerPaths;
    std::vector<WallBuildPath *> mWallBuildPaths;

    ControlMap * mControlMap = nullptr;

    Game * mGame = nullptr;
    ScreenGame * mScreenGame = nullptr;

    IsoMap * mIsoMap = nullptr;

    unsigned int mRows = 0;
    unsigned int mCols = 0;

    // Player stats
    std::unordered_map<PlayerFaction, unsigned int> mEnemiesKilled;
    std::unordered_map<PlayerFaction, unsigned int> mCasualties;
};

// ==================== INLINE METHODS ====================

inline const ControlMap * GameMap::GetControlMap() const { return mControlMap; }

inline bool GameMap::HasObject(unsigned int ind) const
{
    return mCells[ind].objTop != nullptr;
}

inline bool GameMap::HasObject(unsigned int r, unsigned int c) const
{
    const unsigned int ind = r * mCols + c;

    return ind < mCells.size() && mCells[ind].objTop != nullptr;
}

inline bool GameMap::HasObject(const GameObject * obj) const
{
    return mObjectsSet.find(obj) != mObjectsSet.end();
}

inline const std::vector<GameMapCell> & GameMap::GetCells() const
{
    return mCells;
}

inline const std::vector<GameObject *> & GameMap::GetObjects() const
{
    return mObjects;
}

inline bool GameMap::IsCellWalkable(unsigned int cellInd) const
{
    return mCells[cellInd].walkable;
}

inline void GameMap::SetCellWalkable(unsigned int cellInd, bool val)
{
    mCells[cellInd].walkable = val;
}

inline void GameMap::SetCellWalkable(unsigned int r, unsigned int c, bool val)
{
    const unsigned int ind = r * mCols + c;

    SetCellWalkable(ind, val);
}

inline void GameMap::SetCellWalkTarget(unsigned int cellInd, bool val)
{
    mCells[cellInd].walkTarget = val;
}

inline void GameMap::SetCellType(unsigned int r, unsigned int c, CellTypes type)
{
    const unsigned int ind = r * mCols + c;

    SetCellType(ind, type);
}

inline bool GameMap::IsCellObjectVisited(unsigned int r, unsigned int c) const
{
    const unsigned int cellInd = r * mCols + c;
    return IsCellObjectVisited(cellInd);
}

/**
 * @brief Returns the player ID of the Player that currently controls the cell.
 * @param r Row index, starting from 0
 * @param c Column index, starting from 0
 * @return A player ID, or -1 if the cell is not controlled by any player
 */
inline Player * GameMap::GetCellOwner(unsigned int r, unsigned int c) const
{
    if(r < mRows && c < mCols)
        return mCells[r * mCols + c].owner;
    else
        return nullptr;
}

/**
 * @brief Checks if a cell is in the process of changing (upgrade in progress).
 * @param r Row index, starting from 0
 * @param c Column index, starting from 0
 * @return TRUE if the cell is changing, FALSE otherwise
 */
inline bool GameMap::IsCellChanging(unsigned int r, unsigned int c) const
{
    if(r < mRows && c < mCols)
        return mCells[r * mCols + c].changing;
    else
        return false;
}

inline void GameMap::SetCellChanging(unsigned int r, unsigned int c, bool changing)
{
    if(r < mRows && c < mCols)
        mCells[r * mCols + c].changing = changing;
}

inline void GameMap::RegisterEnemyKill(PlayerFaction killer) { ++mEnemiesKilled[killer]; }
inline void GameMap::RegisterCasualty(PlayerFaction killed) { ++mCasualties[killed]; }
inline unsigned int GameMap::GetEnemiesKilled(PlayerFaction killer) const { return mEnemiesKilled.at(killer); }
inline unsigned int GameMap::GetCasualties(PlayerFaction faction) const { return mCasualties.at(faction); }

/**
 * @brief Gets a GameMapCell object from the map. No boundaries check is done.
 * @param r Row index, starting from 0
 * @param c Column index, starting from 0
 * @return Const reference to a cell of the map
 */
inline const GameMapCell & GameMap::GetCell(unsigned int r, unsigned int c) const
{
    return mCells[r * mCols + c];
}

inline const GameMapCell & GameMap::GetCell(unsigned int ind) const
{
    return mCells[ind];
}

/**
 * @brief Gets the number of rows of the map.
 * @return Number of map rows
 */
inline unsigned int GameMap::GetNumRows() const { return mRows; }

/**
 * @brief Gets the number of cells of the map.
 * @return Number of map cells
 */
inline unsigned int GameMap::GetNumCols() const { return mCols; }

} // namespace game
