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
    static constexpr int MAX_PRIORITY = 99;
    static constexpr int MIN_PRIORITY = 1;

public:
    PlayerAI(Player * player, const ObjectsDataRegistry * dataReg);
    ~PlayerAI();

    void SetGameMap(GameMap * gm);

    void DecideNextAction();
    // this can be used to request the AI to do something, for example during the tutorial
    void RequestNewAction(ActionAI * action);

    const ActionAI * GetNextActionTodo();

    void HandleObjectDestroyed(GameObject * obj);

    void RegisterActionInProgress(const ActionAI * action);
    bool IsActionHighestPriorityForObject(const ActionAI * action) const;
    void CancelObjectAction(const GameObject * obj);
    void CancelAction(const ActionAI * action);
    void SetActionDone(const ActionAI * action);

    bool IsDoingSomething() const;

    Player * GetPlayer();

    // EXECUTION SUPPORT METHODS
    bool FindWhereToBuildStructure(Unit * unit, Cell2D & target) const;
    bool FindWhereToBuildTower(Unit * unit, Cell2D & target) const;

    bool IsActive() const;
    void SetActive(bool val);

    void StartIdleTurn(float sec);

    void Update(float delta);

private:
    void ClearActionsDone();
    void ClearActionsTodo();

    void HandleObjectDestroyedInTodo(GameObject * obj);
    void HandleObjectDestroyedInDoing(GameObject * obj);

    void PrepareData();

    void UpdatePriorityRange();

    void AddActions();
    void AddRequestedActions();

    void PushAction(ActionAI * action);
    const ActionAI * PopAction();

    void AddNewAction(ActionAI * action);

    void SetIdleTurnDone();

    // GENERIC ACTIONS
    void AddActionEndTurn();
    void AddActionIdleTurn(float sec);
    // STRUCTURE ACTIONS
    void AddActionsStructure(Structure * s);
    void AddActionBaseCreateUnit(Structure * base);
    void AddActionBarrackCreateUnit(Structure * barrack);
    void AddActionStructureUpgrade(Structure * s);
    // UNIT ACTIONS
    void AddActionsUnit(Unit * u);
    void AddActionUnitAttackEnemyUnit(Unit * u);
    void AddActionUnitAttackEnemyTower(Unit * u);
    void AddActionUnitAttackTrees(Unit * u);
    void AddActionUnitBuildStructure(Unit * u);
    void AddActionUnitBuildTower(Unit * u);
    void AddActionUnitBuildUnitCreator(Unit * u, GameObjectTypeId structType, int priority0);
    void AddActionUnitBuildResourceGenerator(Unit * u, ResourceType resType, int priority0);
    void AddActionUnitBuildResourceStorage(Unit * u, ResourceType resType, int priority0);
    void AddActionUnitBuildResearchCenter(Unit * u, int priority0);
    void AddActionUnitBuildPracticeTarget(Unit * u, int priority0);
    void AddActionUnitBuildRadarStructure(Unit * u, GameObjectTypeId structType, int priority0);
    void AddActionUnitCollectBlobs(Unit * u);
    void AddActionUnitCollectDiamonds(Unit * u);
    void AddActionUnitOpenLootbox(Unit * u);
    void AddActionUnitConnectStructure(Unit * u);
    void AddActionUnitConquerCity(Unit * u);
    void AddActionUnitConquerResGen(Unit * u, ResourceType type);
    void AddActionUnitPatrol(Unit * u);
    void AddActionUnitUpgrade(Unit * u);

    // HELPER METHODS FOR ACTIONS
    void AddActionCreateUnit(Structure * gen, const std::vector<GameObjectTypeId> & types);
    void AddActionUpgrade(GameObject * obj, const int weights[], AIActionType type);

    int GetMaxDistanceForObject(const GameObject * obj) const;
    int GetStructurePriorityBonusEnergy(const Structure * s, float bonus) const;
    int GetUnitPriorityBonusDistance(const Unit * u, int dist, float bonus) const;
    int GetUnitPriorityBonusEnergy(const Unit * u, float bonus) const;
    int GetUnitPriorityBonusHealth(const Unit * u, float bonus) const;

    bool HasPlayerResourcesToBuild(GameObjectTypeId t) const;
    int GetPriorityBonusStructureBuildCost(GameObjectTypeId t, float bonus) const;
    int GetPriorityBonusSameStructureCreated(GameObjectTypeId t, float bonus) const;

    int GetNumStructuresInArea(const Cell2D & tl, const Cell2D & br,
                               GameObjectTypeId type) const;

    void PrintdActionDebug(const char * title, const ActionAI * a);

private:
    static unsigned int mNumActions;

private:
    std::vector<ActionAI *> mActionsTodo;
    std::vector<ActionAI *> mActionsRequested;
    std::vector<const ActionAI *> mActionsDoing;
    std::vector<const ActionAI *> mActionsDone;

    // shared data
    std::vector<GameObject *> mCollectables;
    std::vector<GameObject *> mInteractiveObjects;

    std::vector<GameObject *> mOwnStructures;
    std::vector<GameObject *> mOwnUnits;
    std::vector<GameObject *> mResGenerators;
    std::vector<GameObject *> mTrees;
    std::vector<GameObject *> mVisibleEnemies;
    std::vector<GameObject *> mVisibleEnemyStructures;
    std::vector<GameObject *> mVisibleEnemyUnits;

    Player * mPlayer = nullptr;

    const ObjectsDataRegistry * mDataReg;

    GameMap * mGm = nullptr;

    int mMinPriority = 0;

    float mTimerIdle = 0.f;

    bool mActive = true;
    bool mIdle = false;
};

inline void PlayerAI::SetGameMap(GameMap * gm) { mGm = gm; }

inline Player * PlayerAI::GetPlayer() { return mPlayer; }

inline bool PlayerAI::IsDoingSomething() const { return !mActionsDoing.empty(); }

inline void PlayerAI::RegisterActionInProgress(const ActionAI * action)
{
    mActionsDoing.push_back(action);
}

inline bool PlayerAI::IsActive() const  { return mActive; }
inline void PlayerAI::SetActive(bool val) { mActive = val; }

inline void PlayerAI::StartIdleTurn(float sec)
{
    mTimerIdle = sec;
    mIdle = true;
}

} // namespace game
