#include "PlayerAI.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/ResourceGenerator.h"
#include "GameObjects/Structure.h"
#include "GameObjects/Unit.h"

#include <sgl/utilities/LoadedDie.h>

#include <algorithm>
#include <iostream>

namespace
{
constexpr int MAX_PRIORITY = 100;
}

namespace game
{

PlayerAI::PlayerAI(Player * player, const ObjectsDataRegistry * dataReg)
    : mPlayer(player)
    , mDataReg(dataReg)
{
}

PlayerAI::~PlayerAI()
{
    ClearActionsTodo();
    ClearActionsDone();
}

void PlayerAI::DecideNextAction()
{
    // TODO track time and keep it into consideration when defining priorities
    // TODO use memory pools for actions
    ClearActionsTodo();
    ClearActionsDone();

    PrepareData();

    UpdatePriorityRange();

    AddActions();
}

void PlayerAI::PrepareData()
{
    // clear data
    mCollectables.clear();
    mResGenerators.clear();
    mStructures.clear();
    mTrees.clear();
    mUnits.clear();

    // collect data
    const std::vector<GameObject *> & objects = mGm->GetObjects();

    for(GameObject * obj : objects)
    {
        const GameObjectCategoryId objCat = obj->GetObjectCategory();

        // store structures
        if(obj->IsStructure())
        {
            mStructures.push_back(obj);

            // store resource generators
            if(objCat == GameObject::CAT_RES_GENERATOR)
                mResGenerators.push_back(obj);
        }
        else if(obj->GetObjectCategory() == GameObject::CAT_UNIT)
            mUnits.push_back(obj);
        else if(obj->CanBeCollected())
            mCollectables.push_back(obj);
        else if(obj->GetObjectType() == GameObject::TYPE_TREES)
            mTrees.push_back(obj);
    }
}

void PlayerAI::UpdatePriorityRange()
{
    // TODO define min priority based on current situation

    mMinPriority = 50;
}

void PlayerAI::AddActions()
{
    // STRUCTURES
    for(unsigned int i = 0; i < mPlayer->GetNumStructures(); ++i)
    {
        Structure * s = mPlayer->GetStructure(i);

        const GameObjectTypeId objType = s->GetObjectType();

        if(objType == GameObject::TYPE_BASE)
            AddActionBaseCreateUnit(s);
    }

    // UNITS
    for(unsigned int i = 0; i < mPlayer->GetNumUnits(); ++i)
    {
        Unit * u = mPlayer->GetUnit(i);
        AddActionsUnit(u);
    }

    // KEEP THI LAST
    AddActionEndTurn();
}

const ActionAI * PlayerAI::GetNextActionTodo()
{
    // return NOP action if queue is empty
    if(mActionsTodo.empty())
        return nullptr;

    // return top action
    return PopAction();
}

void PlayerAI::HandleObjectDestroyed(GameObject * obj)
{
    HandleObjectDestroyedInTodo(obj);
    HandleObjectDestroyedInDoing(obj);
}

bool PlayerAI::IsActionHighestPriorityForObject(const ActionAI * action) const
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        const ActionAI * a = *it;

        if(a->ObjSrc == action->ObjSrc)
            return action->priority > a->priority;
        else
            ++it;
    }

    return true;
}

void PlayerAI::CancelObjectAction(const GameObject * obj)
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        const ActionAI * action = *it;

        if(obj == action->ObjSrc)
        {
            mActionsDoing.erase(it);

            PrintdActionDebug("PlayerAI::CancelObjectActions - ACTION CANCELLED", action);

            delete action;

            return ;
        }
        else
            ++it;
    }

    std::cout << "PlayerAI::CancelObjectActions - can't find any action for object "
              << obj->GetObjectId() << std::endl;
}

void PlayerAI::CancelAction(const ActionAI * action)
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        if(action->actId == (*it)->actId)
        {
            mActionsDoing.erase(it);

            PrintdActionDebug("PlayerAI::CancelAction - ACTION CANCELLED", action);

            delete action;

            return ;
        }
        else
            ++it;
    }

    PrintdActionDebug("PlayerAI::CancelAction - can't find action", action);

    delete action;
}

void PlayerAI::SetActionDone(const ActionAI * action)
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        if(action->actId == (*it)->actId)
        {
            mActionsDoing.erase(it);

            mActionsDone.push_back(action);

            PrintdActionDebug("PlayerAI::SetActionDone | ACTION DONE", action);

            return ;
        }
        else
            ++it;
    }

    std::cout << "PlayerAI::SetActionDone - ACTION DONE - can't find action" << std::endl;
}

void PlayerAI::ClearActionsDone()
{
    for(const ActionAI * a : mActionsDone)
        delete a;

    mActionsDone.clear();
}

void PlayerAI::ClearActionsTodo()
{
    for(ActionAI * a : mActionsTodo)
        delete a;

    mActionsTodo.clear();
}

void PlayerAI::HandleObjectDestroyedInTodo(GameObject * obj)
{
    auto it = mActionsTodo.begin();

    while(it != mActionsTodo.end())
    {
        const ActionAI * action = *it;

        if(action->ObjSrc == obj || action->ObjDst == obj)
        {
            delete action;
            mActionsTodo.erase(it);
            break;
        }
        else
            ++it;
    }
}

void PlayerAI::HandleObjectDestroyedInDoing(GameObject * obj)
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        const ActionAI * action = *it;

        if(action->ObjSrc == obj || action->ObjDst == obj)
        {
            delete action;
            mActionsDoing.erase(it);
            break;
        }
        else
            ++it;
    }
}

void PlayerAI::PushAction(ActionAI * action)
{
    mActionsTodo.emplace_back(action);
    std::push_heap(mActionsTodo.begin(), mActionsTodo.end(), ActionAiComp{});
}

const ActionAI * PlayerAI::PopAction()
{
    std::pop_heap(mActionsTodo.begin(), mActionsTodo.end(), ActionAiComp{});

    ActionAI * elem = mActionsTodo.back();
    mActionsTodo.pop_back();

    return elem;
}

void PlayerAI::AddNewAction(ActionAI * action)
{
    // assign unique ID to action
    static unsigned int num = 0;

    action->actId = ++num;

    // clamp priority
    if(action->priority > MAX_PRIORITY)
        action->priority = MAX_PRIORITY;

    PrintdActionDebug("PlayerAI::AddNewAction | ADDED NEW ACTION", action);

    // NOTE not checking existing actions for now as all actions should be unique
    // as they are created by different objects (at least the ObjSrc is different)
    PushAction(action);
}

void PlayerAI::AddActionEndTurn()
{
    // create action
    auto action = new ActionAINewUnit;
    action->type = AIA_END_TURN;
    action->priority = MAX_PRIORITY;

    // immediately push action if there's nothing else to do
    if(mActionsTodo.empty())
    {
        PushAction(action);
        return ;
    }

    const float c = 100.f;

    // consider turn energy
    const float energyLeftTurn = mPlayer->GetTurnEnergy() * c / mPlayer->GetTurnMaxEnergy();

    // consider total units energy
    float totEnergy = 0.f;
    float totMaxEnergy = 0.f;

    for(GameObject * obj : mUnits)
    {
        totEnergy += obj->GetEnergy();
        totMaxEnergy += obj->GetMaxEnergy();
    }

    const float energyLeftUnits = totEnergy * c / totMaxEnergy;

    // consider total structures energy
    totEnergy = 0.f;
    totMaxEnergy = 0.f;

    for(GameObject * obj : mUnits)
    {
        // only consider structure that have AI actions
        if(obj->GetObjectType() == GameObject::TYPE_BASE)
        {
            totEnergy += obj->GetEnergy();
            totMaxEnergy += obj->GetMaxEnergy();
        }
    }

    const float energyLeftStructs = totEnergy * c / totMaxEnergy;

    // combine units and structure energy left
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int maxUnits = mPlayer->GetMaxUnits();
    const float unitsProportion = (static_cast<float>(numUnits) / static_cast<float>(maxUnits));
    const float facUnits0 = 0.5f;
    const float facUnitsVar = 0.3f * unitsProportion;
    const float facUnits = facUnits0 + facUnitsVar;
    const float factStructs = 1.f - facUnits;
    const float energyLeftObjs = energyLeftUnits * facUnits + energyLeftStructs * factStructs;

    // decrease priority base on global energy left (turn + objects)
    const float facTurn = 0.5f;
    const float facObjs = 1.f - facTurn;
    const int decPriority = std::roundf(facTurn * energyLeftTurn + facObjs * energyLeftObjs);
    action->priority -= decPriority;

    // push action if above priority threshold
    if(action->priority >= mMinPriority)
        PushAction(action);
}

void PlayerAI::AddActionBaseCreateUnit(Structure * base)
{
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    // can't build more units -> exit
    if(numUnits >= limitUnits)
        return ;

    int priority = MAX_PRIORITY;

    // the more units exist the lower the priority
    const float bonusUnits = -25.f;
    priority += std::roundf(bonusUnits * numUnits / limitUnits);

    // decrease priority based on base's energy
    const float bonusEnergy = -20.f;
    priority += GetStructurePriorityBonusEnergy(base, bonusEnergy);

    // priority already too low
    if(priority < mMinPriority)
        return ;

    // DECIDE UNIT TYPE
    std::vector<GameObjectTypeId> types { GameObject::TYPE_UNIT_WORKER1 };
    const unsigned int numTypes = types.size();

    const int energy = mPlayer->GetStat(Player::ENERGY).GetIntValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetIntValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetIntValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetIntValue();

    unsigned int bestInd = numTypes;
    int bestPriority = mMinPriority;

    for(unsigned int i = 0; i < numTypes; ++i)
    {
        const GameObjectTypeId t = types[i];

        // can't create this unit -> next
        if(!mGm->CanCreateUnit(t, base, mPlayer))
            continue;

        int typePriority = priority;

        // reduce priority for existing same type units
        const float bonusSameType = -20.f;

        for(unsigned int j = 0; j < numUnits; ++j)
        {
            if(mPlayer->GetUnit(j)->GetObjectType() == t)
                typePriority += std::roundf(bonusSameType * numUnits / limitUnits);
        }

        // reduce priority based on available resources
        // NOTE all costs are < current resources or CanCreateUnit would have returned false
        const ObjectData & data = mDataReg->GetObjectData(t);
        const auto & costs = data.GetCosts();

        const float bonusRes = -10.f;

        if(costs[RES_ENERGY] > 0)
            typePriority += std::roundf(bonusRes * costs[RES_ENERGY] / energy);

        if(costs[RES_MATERIAL1] > 0)
            typePriority += std::roundf(bonusRes * costs[RES_MATERIAL1] / material);

        if(costs[RES_BLOBS] > 0)
            typePriority += std::roundf(bonusRes * costs[RES_BLOBS] / blobs);

        if(costs[RES_DIAMONDS] > 0)
            typePriority += std::roundf(bonusRes * costs[RES_DIAMONDS] / diamonds);

        if(typePriority > bestPriority)
        {
            bestPriority = typePriority;
            bestInd = i;
        }
    }

    // couldn't find any good
    if(bestInd == numTypes)
        return;

    // create action
    auto action = new ActionAINewUnit;
    action->type = AIA_NEW_UNIT;
    action->ObjSrc = base;
    action->priority = bestPriority;
    action->unitType = types[bestInd];

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionsUnit(Unit * u)
{
    // ATTACK
    if(u->CanAttack())
    {
        AddActionUnitAttackEnemyUnit(u);
        AddActionUnitAttackTrees(u);
    }

    if(u->CanConquer())
    {
        // CONNECT STRUCTURES
        AddActionUnitConnectStructure(u);

        // CONQUEST RESOURCE GENERATORS
        AddActionUnitConquestResGen(u, RES_ENERGY);
        AddActionUnitConquestResGen(u, RES_MATERIAL1);
    }

    // BUILD
    if(u->CanBuild())
    {
        AddActionUnitBuildStructure(u);
    }

    // COLLECTABLES
    AddActionUnitCollectBlobs(u);
    AddActionUnitCollectDiamonds(u);
    AddActionUnitCollectLootbox(u);
}

void PlayerAI::AddActionUnitAttackEnemyUnit(Unit * u)
{
    // nothing to do if there's no units on the map
    if(mUnits.empty())
        return ;

    const PlayerFaction faction = mPlayer->GetFaction();
    const unsigned int numUnits = mUnits.size();

    // check if there's any unit to shoot at
    const int maxDist = GetMaxDistanceForObject(u);

    unsigned int bestUnitInd = numUnits;
    int minDist = maxDist;
    int priority = MAX_PRIORITY;

    for(unsigned int i = 0; i < numUnits; ++i)
    {
        auto unit = static_cast<Unit *>(mUnits[i]);

        const PlayerFaction unitFaction = unit->GetFaction();

        // skip own faction units
        if(unitFaction == faction)
            continue;

        // skip targets out of range
        if(!u->IsTargetAttackInRange(unit))
            continue;

        // basic logic, attack closest one
        const int dist = mGm->ApproxDistance(u, unit);

        if(dist < minDist)
        {
            minDist = dist;
            bestUnitInd = i;
        }
    }

    // didn't find any
    if(bestUnitInd == numUnits)
        return ;

    // decrease priority based on unit's energy
    const float bonusEnergy = -5.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_ATTACK_ENEMY_UNIT;
    action->ObjSrc = u;
    action->ObjDst = mUnits[bestUnitInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitAttackTrees(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -40.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // FIND TREE
    if(mTrees.empty())
        return ;

    const unsigned int numTrees = mTrees.size();
    const int maxDist = mGm->GetNumRows() * mGm->GetNumCols();
    const GameObject * base = mPlayer->GetBase();

    if(nullptr == base)
        return ;

    int minDist = maxDist;
    unsigned int bestInd = numTrees;

    for(unsigned int ind = 0; ind < numTrees; ++ind)
    {
        const GameObject * tree = mTrees[ind];

        int dist = mGm->ApproxDistance(base, tree);

        if(dist < minDist)
        {
            minDist = dist;
            bestInd = ind;
        }
    }

    // couldn't find any
    if(numTrees == bestInd)
        return ;

    // consider distance from base for priority
    const float bonusDistBase = -1.f;
    priority += std::roundf(bonusDistBase * minDist);

    // bonus distance from unit
    const bool inRange = u->IsTargetAttackInRange(mTrees[bestInd]);

    const float bonusDistUnit = inRange ? -10.f : -20.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDistUnit);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_ATTACK_TREES;
    action->ObjSrc = u;
    action->ObjDst = mTrees[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitBuildStructure(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // decrease priority based on distance from Base
    const GameObject * base = mPlayer->GetBase();
    const int distBase = mGm->ApproxDistance(u, base);
    const int maxDist = GetMaxDistanceForObject(u);
    const float bonusDistBase = -15.f;
    priority += std::roundf(bonusDistBase * distBase / maxDist);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // RESOURCE GENERATORS
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_GEN_ENERGY_SOLAR))
        AddActionUnitBuildResourceGenerator(u, RES_ENERGY, priority);
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT))
        AddActionUnitBuildResourceGenerator(u, RES_MATERIAL1, priority);

    // RESOURCE STORAGE
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_STORAGE_ENERGY))
        AddActionUnitBuildResourceStorage(u, RES_ENERGY, priority);
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_STORAGE_MATERIAL))
        AddActionUnitBuildResourceStorage(u, RES_MATERIAL1, priority);
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_STORAGE_BLOBS))
        AddActionUnitBuildResourceStorage(u, RES_BLOBS, priority);
    if(mPlayer->IsStructureAvailable(GameObject::TYPE_RES_STORAGE_DIAMONDS))
        AddActionUnitBuildResourceStorage(u, RES_DIAMONDS, priority);

    // UNIT GENERATION
    AddActionUnitBuildBarracks(u, priority);

    // TODO
    /*
    static const GameObjectTypeId TYPE_HOSPITAL;
    static const GameObjectTypeId TYPE_PRACTICE_TARGET;
    static const GameObjectTypeId TYPE_RADAR_STATION;
    static const GameObjectTypeId TYPE_RADAR_TOWER;
    static const GameObjectTypeId TYPE_RESEARCH_CENTER;
    */
}

void PlayerAI::AddActionUnitBuildBarracks(Unit * u, int priority0)
{
    const GameObjectTypeId structType = GameObject::TYPE_BARRACKS;

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    // can't build more units -> exit
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    if(numUnits >= limitUnits)
        return ;

    int priority = priority0;

    // the more units exist the higher the priority as no soldier can be created yet
    const float bonusUnits = 30.f;
    priority += std::roundf(bonusUnits * numUnits / limitUnits);

    // reduce priority based on available resources
    const float bonusRes = -5.f;
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes);

    // check if below current priority threshold
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAIBuildStructure;
    action->type = AIA_UNIT_BUILD_STRUCTURE;
    action->ObjSrc = u;
    action->priority = priority;
    action->structType = structType;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitBuildResourceGenerator(Unit * u, ResourceType resType, int priority0)
{
    // resource not supported
    if(resType != RES_ENERGY && resType != RES_MATERIAL1)
        return ;

    const GameObjectTypeId structTypes[] =
    {
        GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
        GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT
    };

    const GameObjectTypeId structType = structTypes[resType];

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    int priority = priority0;

    // decrease priority based on current resource level (high level -> lower priority)
    const Player::Stat ps[] =
    {
        Player::ENERGY,
        Player::MATERIAL,
    };

    const StatValue & res = mPlayer->GetStat(ps[resType]);

    const int resCur = res.GetIntValue();
    const int resMax = res.GetIntMax();

    const float bonusStorage = -40.f;
    priority += std::roundf(bonusStorage * resCur / resMax);

    // reduce priority based on available resources
    const float bonusRes = -5.f;
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes);

    // check if below current priority threshold
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAIBuildStructure;
    action->type = AIA_UNIT_BUILD_STRUCTURE;
    action->ObjSrc = u;
    action->priority = priority;
    action->structType = structType;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitBuildResourceStorage(Unit * u, ResourceType resType, int priority0)
{
    const GameObjectTypeId structTypes[] =
    {
        GameObject::TYPE_RES_STORAGE_ENERGY,
        GameObject::TYPE_RES_STORAGE_MATERIAL,
        GameObject::TYPE_RES_STORAGE_DIAMONDS,
        GameObject::TYPE_RES_STORAGE_BLOBS,
    };

    const GameObjectTypeId structType = structTypes[resType];

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    int priority = priority0;

    // decrease priority based on current storage level
    const Player::Stat ps[] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS
    };

    const StatValue & res = mPlayer->GetStat(ps[resType]);

    const int resCur = res.GetIntValue();
    const int resMax = res.GetIntMax();
    const int resDif = resMax - resCur;

    const float bonusStorage = -50.f;
    priority += std::roundf(bonusStorage * resDif / resMax);

    // reduce priority based on available resources
    const float bonusRes = -5.f;
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes);

    // check if below current priority threshold
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAIBuildStructure;
    action->type = AIA_UNIT_BUILD_STRUCTURE;
    action->ObjSrc = u;
    action->priority = priority;
    action->structType = structType;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitCollectBlobs(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on owned blobs
    const float decBlobs = -30.f;
    const StatValue & blobs = mPlayer->GetStat(Player::BLOBS);
    priority += std::roundf(decBlobs * blobs.GetIntValue() / blobs.GetIntMax());

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // FIND BEST CANDIDATE
    const unsigned int numCollectables = mCollectables.size();

    const int maxDist = GetMaxDistanceForObject(u);
    unsigned int bestInd = numCollectables;
    int minDist = maxDist;

    for(unsigned int i = 0; i < numCollectables; i++)
    {
        const GameObject * c = mCollectables[i];

        // no blobs
        if(c->GetObjectType() != GameObject::TYPE_BLOBS)
            continue;

        // basic logic, collect closest one
        const int dist = mGm->ApproxDistance(u, c);

        if(dist < minDist)
        {
            minDist = dist;
            bestInd = i;
        }
    }

    // none found
    if(bestInd == numCollectables)
        return ;

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAI;
    action->type = AIA_UNIT_COLLECT_BLOBS;
    action->ObjSrc = u;
    action->ObjDst = mCollectables[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitCollectDiamonds(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on owned diamonds
    const float decDiamonds = -30.f;
    const StatValue & diamonds = mPlayer->GetStat(Player::DIAMONDS);
    priority += std::roundf(decDiamonds * diamonds.GetIntValue() / diamonds.GetIntMax());

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // FIND BEST CANDIDATE
    const unsigned int numCollectables = mCollectables.size();

    const int maxDist = GetMaxDistanceForObject(u);
    unsigned int bestInd = numCollectables;
    int minDist = maxDist;

    for(unsigned int i = 0; i < numCollectables; i++)
    {
        const GameObject * c = mCollectables[i];

        // no blobs
        if(c->GetObjectType() != GameObject::TYPE_DIAMONDS)
            continue;

        // basic logic, collect closest one
        const int dist = mGm->ApproxDistance(u, c);

        if(dist < minDist)
        {
            minDist = dist;
            bestInd = i;
        }
    }

    // none found
    if(bestInd == numCollectables)
        return ;

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAI;
    action->type = AIA_UNIT_COLLECT_DIAMONDS;
    action->ObjSrc = u;
    action->ObjDst = mCollectables[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitCollectLootbox(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // FIND BEST CANDIDATE
    const unsigned int numCollectables = mCollectables.size();

    unsigned int bestInd = numCollectables;
    int minDist = GetMaxDistanceForObject(u);

    for(unsigned int i = 0; i < numCollectables; i++)
    {
        const GameObject * c = mCollectables[i];

        // no lootbox
        if(c->GetObjectType() != GameObject::TYPE_LOOTBOX)
            continue;

        // basic logic, collect closest one
        const int dist = mGm->ApproxDistance(u, c);

        if(dist < minDist)
        {
            minDist = dist;
            bestInd = i;
        }
    }

    // none found
    if(bestInd == numCollectables)
        return ;

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAI;
    action->type = AIA_UNIT_COLLECT_LOOTBOX;
    action->ObjSrc = u;
    action->ObjDst = mCollectables[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConnectStructure(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -30.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // check if there's any structure to connect
    const unsigned int numStructures = mStructures.size();

    unsigned int bestStructInd = numStructures;
    int minDist = GetMaxDistanceForObject(u);
    Cell2D startConquest;

    const PlayerFaction faction = mPlayer->GetFaction();

    const Cell2D posUnit(u->GetRow0(), u->GetCol0());

    for(unsigned int i = 0; i < numStructures; ++i)
    {
        auto s = static_cast<Structure *>(mStructures[i]);

        // own structure which is not linked
        if(s->GetFaction() == faction && !s->IsLinked())
        {
            Cell2D start;

            // SPECIAL CASE: unit is already next to structure -> best option
            if(mGm->AreObjectsOrthoAdjacent(u, s))
            {
                minDist = 0;
                bestStructInd = i;
                startConquest = posUnit;

                break;
            }

            // check unit distance from structure
            const int dist = mGm->ApproxDistance(u, s);

            // unit is closer to structure
            if(dist < minDist)
            {
                start = mGm->GetOrthoAdjacentMoveTarget(posUnit, s);

                if(start.row != -1 && start.col != -1)
                {
                    minDist = dist;
                    bestStructInd = i;
                    startConquest = start;

                    std::cout << "PlayerAI::AddActionUnitConnectStructure - A - structure: " << s->GetObjectId()
                              << " - new min dist: " << dist
                              << " - dest: " << startConquest.row << "," << startConquest.col
                              << " - obj: " << u->GetObjectId()
                              << " - obj pos: " << posUnit.row << "," << posUnit.col <<std::endl;
                }
            }

            // check distance to closest connected cell
            if(mGm->FindClosestCellConnectedToObject(s, posUnit, start))
            {
                const GameMapCell & gmc = mGm->GetCell(start.row, start.col);

                // connected cell is occupied and not by unit -> find adjacent
                if(gmc.objTop != nullptr && gmc.objTop != u)
                {
                    start = mGm->GetOrthoAdjacentMoveTarget(posUnit, s);

                    // can't find any
                    if(start.row == -1 || start.col == -1)
                        continue;
                }

                const int dist = mGm->ApproxDistance(posUnit, start);

                // found a closest cell
                if(dist < minDist)
                {
                    minDist = dist;
                    bestStructInd = i;
                    startConquest = start;

                    std::cout << "PlayerAI::AddActionUnitConnectStructure - B - structure: " << s->GetObjectId()
                              << " - new min dist: " << dist
                              << " - dest: " << startConquest.row << "," << startConquest.col
                              << " - obj: " << u->GetObjectId()
                              << " - obj pos: " << posUnit.row << "," << posUnit.col <<std::endl;

                    // already found best option
                    if(0 == dist)
                        break;
                }
            }
        }
    }

    // can't find any structure to connect
    if(bestStructInd == numStructures)
        return ;

    // bonus distance
    const float bonusDist = -50.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONNECT_STRUCTURE;
    action->ObjSrc = u;
    action->cellSrc = startConquest;
    action->ObjDst = mStructures[bestStructInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConquestResGen(Unit * u, ResourceType type)
{
    // resource not supported
    if(type != RES_ENERGY && type != RES_MATERIAL1)
        return ;

    const int supportedRes = 2;

    const enum Player::Stat types[supportedRes] =
    {
        Player::Stat::ENERGY,
        Player::Stat::MATERIAL,
    };

    const enum Player::Stat ptype = types[type];
    const StatValue & stat = mPlayer->GetStat(ptype);

    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -20.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // bonus resource availability level
    const float bonusRes = -25.f;
    priority += std::roundf(bonusRes * stat.GetIntValue() / stat.GetIntMax());

    // action is already not doable
    if(priority < mMinPriority)
        return ;

    // visit all generators
    const int maxDist = GetMaxDistanceForObject(u);
    const unsigned int numGens = mResGenerators.size();

    unsigned int bestInd = numGens;
    int minDist = maxDist;

    int totGenerators = 0;
    int ownedGenerators = 0;

    const PlayerFaction playerFaction = mPlayer->GetFaction();

    // find best candidate
    for(unsigned int i = 0; i < numGens; ++i)
    {
        auto resGen = static_cast<ResourceGenerator *>(mResGenerators[i]);

        if(resGen->GetResourceType() != type)
            continue;

        ++totGenerators;

        // generator already owned by player
        if(playerFaction == resGen->GetFaction())
        {
            ++ownedGenerators;

            continue;
        }

        // basic logic, conquest closest one
        const int dist = mGm->ApproxDistance(u, resGen);

        if(dist < minDist)
        {
            minDist = dist;
            bestInd = i;
        }
    }

    // didn't find any
    if(bestInd == numGens)
        return ;

    // bonus distance
    const float bonusDist = -20.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // decrease priority for owned generators
    const float bonusOwned = -30.f;
    priority += std::roundf(bonusOwned * ownedGenerators / totGenerators);

    // bonus unit conquest
    const float bonusConquest = 10.f;
    priority += std::roundf(bonusConquest * u->GetAttribute(OBJ_ATT_CONQUEST) / ObjectData::MAX_STAT_VAL);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONQUER_GEN;
    action->ObjSrc = u;
    action->ObjDst = mResGenerators[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

int PlayerAI::GetMaxDistanceForObject(const GameObject * obj) const
{
    const int r = obj->GetRow0();
    const int c = obj->GetCol0();

    const int rows = mGm->GetNumRows();
    const int cols = mGm->GetNumCols();
    const int rowsH = rows / 2;
    const int colsH = cols / 2;

    const int distR = (r < rowsH) ? rows - r : r;
    const int distC = (c < colsH) ? cols - c : c;

    return distR + distC;
}

int PlayerAI::GetStructurePriorityBonusEnergy(const Structure * s, float bonus) const
{
    const float energyUnit = s->GetEnergy();
    const float energyUnitMax = s->GetMaxEnergy();
    const float energyTurn = mPlayer->GetTurnEnergy();
    const float energyTurnMax = mPlayer->GetTurnMaxEnergy();

    // bonuses
    const float bonusStruct = bonus * (energyUnitMax - energyUnit) / energyUnitMax;
    const float bonusTurn = bonus * (energyTurnMax - energyTurn) / energyTurnMax;

    // weights
    const float wStruct = 0.4f;
    const float wTurn = 1.f - wStruct;

    return std::roundf(bonusStruct * wStruct + bonusTurn * wTurn);
}

int PlayerAI::GetUnitPriorityBonusDistance(const Unit * u, int dist, float bonus) const
{
    const float energyStep = u->GetEnergyForActionStep(MOVE);
    const float energyTot = energyStep * dist;
    const float energyUnit = u->GetEnergy();
    const float maxDist = GetMaxDistanceForObject(u);

    // weights
    const float wRel = 0.7f;
    const float wAbs = 1.f - wRel;

    // bonuses
    const float bonusRelDist = bonus * energyTot / energyUnit;
    const float bonusAbsDist = bonus * dist / maxDist;

    return std::roundf(bonusRelDist * wRel + bonusAbsDist * wAbs);
}

int PlayerAI::GetUnitPriorityBonusEnergy(const Unit * u, float bonus) const
{
    const float energyUnit = u->GetEnergy();
    const float energyUnitMax = u->GetMaxEnergy();
    const float energyTurn = mPlayer->GetTurnEnergy();
    const float energyTurnMax = mPlayer->GetTurnMaxEnergy();

    // bonuses
    const float bonusUnit = bonus * (energyUnitMax - energyUnit) / energyUnitMax;
    const float bonusTurn = bonus * (energyTurnMax - energyTurn) / energyTurnMax;

    // weights
    const float wUnit = 0.6f;
    const float wTurn = 1.f - wUnit;

    return std::roundf(bonusUnit * wUnit + bonusTurn * wTurn);
}

int PlayerAI::GetUnitPriorityBonusHealth(const Unit * u, float bonus) const
{
    const float maxHealth = u->GetMaxHealth();
    return std::roundf(bonus * (maxHealth - u->GetHealth()) / maxHealth);
}

bool PlayerAI::HasPlayerResourcesToBuild(GameObjectTypeId t) const
{
    const ObjectData & data = mDataReg->GetObjectData(t);
    const auto & costs = data.GetCosts();

    const int energy = mPlayer->GetStat(Player::ENERGY).GetIntValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetIntValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetIntValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetIntValue();

    // not enough resources to build
    return energy >= costs[RES_ENERGY] && material >= costs[RES_MATERIAL1] &&
           diamonds >= costs[RES_DIAMONDS] && blobs >= costs[RES_BLOBS];
}

// NOTE this requires that HasPlayerResourcesToBuild is called before to stop execution
//      in case resources are not enough
int PlayerAI::GetPriorityBonusStructureBuildCost(GameObjectTypeId t, float bonus) const
{
    const ObjectData & data = mDataReg->GetObjectData(t);
    const auto & costs = data.GetCosts();

    const int energy = mPlayer->GetStat(Player::ENERGY).GetIntValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetIntValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetIntValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetIntValue();

    float b = 0.f;

    if(costs[RES_ENERGY] > 0)
        b += bonus * costs[RES_ENERGY] / energy;

    if(costs[RES_MATERIAL1] > 0)
        b += bonus * costs[RES_MATERIAL1] / material;

    if(costs[RES_BLOBS] > 0)
        b += bonus * costs[RES_BLOBS] / blobs;

    if(costs[RES_DIAMONDS] > 0)
        b += bonus * costs[RES_DIAMONDS] / diamonds;

    return std::roundf(b);
}

void PlayerAI::PrintdActionDebug(const char * title, const ActionAI * a)
{
    std::cout << title;

    std::cout << " - ID: " << a->actId
              << " - TYPE: " << a->GetTypeStr() << " - PRIO: " << a->priority;

    if(a->ObjSrc != nullptr)
    {
        auto obj = a->ObjSrc;
        std::cout << " | OBJ SRC - ID: " << obj->GetObjectId()
                  << " - FACT: " << obj->GetFaction()
                  << " - ENRG: " << obj->GetEnergy() << "/" << obj->GetMaxEnergy()
                  << " - HLTH: " << obj->GetHealth() << "/" << obj->GetMaxHealth();
    }

    if(a->ObjDst != nullptr)
    {
        auto obj = a->ObjDst;
        std::cout << " | OBJ DST - ID: " << obj->GetObjectId()
                  << " - FACT: " << obj->GetFaction()
                  << " - ENRG: " << obj->GetEnergy() << "/" << obj->GetMaxEnergy()
                  << " - HLT: " << obj->GetHealth() << "/" << obj->GetMaxHealth();
    }

    if(a->cellSrc.row != -1)
        std::cout << " | CELL SRC: " << a->cellSrc.row << "," << a->cellSrc.col;

    if(a->cellDst.row != -1)
        std::cout << " | CELL DST: " << a->cellDst.row << "," << a->cellDst.col;

    std::cout << " | TURN - ENRG: " << mPlayer->GetTurnEnergy() << "/" << mPlayer->GetTurnMaxEnergy()
              << std::endl;
}

} // namespace game
