#pragma once

#include "ActionAI.h"

#include <vector>

namespace game
{

class GameMap;
class GameObject;
class ObjectsDataRegistry;
class Player;
class Structure;
class Unit;

struct GameMapCell;

enum ResourceType : unsigned int;

class PlayerAI
{
public:
    PlayerAI(Player * player, const ObjectsDataRegistry * dataReg);
    ~PlayerAI();

    void SetGameMap(GameMap * gm);

    void DecideNextAction();

    const ActionAI * GetNextActionTodo();

    void HandleObjectDestroyed(GameObject * obj);

    void RegisterActionInProgress(const ActionAI * action);
    bool IsActionHighestPriorityForObject(const ActionAI * action) const;
    void CancelObjectAction(const GameObject * obj);
    void CancelAction(const ActionAI * action);
    void SetActionDone(const ActionAI * action);

    bool IsDoingSomething() const;

    Player * GetPlayer();

private:
    void ClearActionsDone();
    void ClearActionsTodo();

    void HandleObjectDestroyedInTodo(GameObject * obj);
    void HandleObjectDestroyedInDoing(GameObject * obj);

    void PrepareData();

    void UpdatePriorityRange();

    void AddActions();

    void PushAction(ActionAI * action);
    const ActionAI * PopAction();

    void AddNewAction(ActionAI * action);

    void AddActionEndTurn();
    void AddActionBaseCreateUnit(Structure * base);
    void AddActionsUnit(Unit * u);
    void AddActionUnitAttackEnemyUnit(Unit * u);
    void AddActionUnitAttackTrees(Unit * u);
    void AddActionUnitBuildStructure(Unit * u);
    void AddActionUnitBuildResourceGenerator(Unit * u, ResourceType resType, int priority0);
    void AddActionUnitBuildResourceStorage(Unit * u, ResourceType resType, int priority0);
    void AddActionUnitCollectBlobs(Unit * u);
    void AddActionUnitCollectDiamonds(Unit * u);
    void AddActionUnitCollectLootbox(Unit * u);
    void AddActionUnitConnectStructure(Unit * u);
    void AddActionUnitConquestResGen(Unit * u, ResourceType type);

    int GetMaxDistanceForObject(const GameObject * obj) const;
    int GetStructurePiorityBonusEnergy(const Structure * s, float bonus) const;
    int GetUnitPiorityBonusDistance(const Unit * u, int dist, float bonus) const;
    int GetUnitPiorityBonusEnergy(const Unit * u, float bonus) const;
    int GetUnitPiorityBonusHealth(const Unit * u, float bonus) const;

    void PrintdActionDebug(const char * title, const ActionAI * a);

private:
    std::vector<ActionAI *> mActionsTodo;
    std::vector<const ActionAI *> mActionsDoing;
    std::vector<const ActionAI *> mActionsDone;

    // shared data
    std::vector<GameObject *> mCollectables;
    std::vector<GameObject *> mResGenerators;
    std::vector<GameObject *> mStructures;
    std::vector<GameObject *> mTrees;
    std::vector<GameObject *> mUnits;

    Player * mPlayer = nullptr;

    const ObjectsDataRegistry * mDataReg;

    GameMap * mGm = nullptr;

    int mMinPriority = 0;
};

inline void PlayerAI::SetGameMap(GameMap * gm) { mGm = gm; }

inline Player * PlayerAI::GetPlayer() { return mPlayer; }

inline bool PlayerAI::IsDoingSomething() const { return !mActionsDoing.empty(); }

inline void PlayerAI::RegisterActionInProgress(const ActionAI * action)
{
    mActionsDoing.push_back(action);
}

} // namespace game
