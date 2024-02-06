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
    void AddActionsBase(Structure * s);
    void AddActionsUnit(Unit * u);
    void AddActionUnitAttackEnemyUnit(Unit * u);
    void AddActionUnitConnectStructure(Unit * u);
    void AddActionUnitConquestResGen(Unit * u, ResourceType type);

    bool IsObjectAlreadyDoingSimilarAction(GameObject * obj, AIActionType type) const;
    bool IsSimilarActionInProgress(AIActionType type) const;

private:
    std::vector<ActionAI *> mActionsTodo;
    std::vector<const ActionAI *> mActionsDoing;
    std::vector<const ActionAI *> mActionsDone;

    // shared data,
    std::vector<GameObject *> mResGenerators;
    std::vector<GameObject *> mStructures;
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
