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
    mResGenerators.clear();
    mStructures.clear();
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
            AddActionsBase(s);
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

            std::cout << "PlayerAI::CancelObjectActions - ACTION CANCELLED - id: " << action->actId
                      << " - type: " << action->type << " - obj: " << obj << std::endl;

            delete action;

            return ;
        }
        else
            ++it;
    }

    std::cout << "PlayerAI::CancelObjectActions - can't find any action" << std::endl;
}

void PlayerAI::CancelAction(const ActionAI * action)
{
    auto it = mActionsDoing.begin();

    while(it != mActionsDoing.end())
    {
        if(action->actId == (*it)->actId)
        {
            mActionsDoing.erase(it);

            std::cout << "PlayerAI::CancelAction - ACTION CANCELLED - id: " << action->actId
                      << " - type: " << action->type << std::endl;

            delete action;

            return ;
        }
        else
            ++it;
    }

    std::cout << "PlayerAI::CancelAction - can't find action" << std::endl;
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

            std::cout << "PlayerAI::SetActionDone - ACTION DONE - id: " << action->actId
                      << " - type: " << action->type << std::endl;

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

    // NOTE not checking existing actions for now as all actions should be unique
    // as they are created by different objects (at least the ObjSrc is different)
    std::cout << "PlayerAI::AddNewAction - ADDED NEW ACTION id: " << action->actId
              << " - type: " << action->type << " - priority: " << action->priority << std::endl;
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
    const float energyLeftTurn = mPlayer->GetTurnEnergy() * c / mPlayer->GetTurnEnergyMax();

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

void PlayerAI::AddActionsBase(Structure * s)
{
    // check if action is already in progress
    if(IsSimilarActionInProgress(AIA_NEW_UNIT))
        return ;

    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    // can't build more units -> exit
    if(numUnits >= limitUnits)
        return ;

    // define base priority
    // MAX with 0 units and MIN with limit units
    const int maxPriority = 100;
    const int priority = maxPriority * (limitUnits - numUnits) / limitUnits;

    // DECIDE UNIT TYPE
    // TODO keep into consideration faction attitude
    // (i.e.: if faction is more inclined to attack then prefer soldier over builder)
    std::vector<int> priorities(GameObject::NUM_UNIT_TYPES, priority);
    std::vector<int> costs(GameObject::NUM_UNIT_TYPES, 0);
    std::vector<int> relCosts(GameObject::NUM_UNIT_TYPES, 0);

    // 1- exclude units that can't be built and check cost
    const int multEnergy = 1;
    const int multMaterial = 2;
    const int multBlobs = 10;
    const int multDiamonds = 20;
    int maxCost = 0;
    int validUnits = 0;

    for(unsigned int i = 0; i < GameObject::NUM_UNIT_TYPES; ++i)
    {
        const GameObjectTypeId ut = Unit::IndexToType(i);

        if(!mPlayer->IsUnitAvailable(ut))
            continue;

        if(!mGm->CanCreateUnit(ut, s, mPlayer))
        {
            priorities[i] = 0;
            continue;
        }

        const ObjectFactionData & fData = mDataReg->GetFactionData(mPlayer->GetFaction(), ut);

        // total cost
        costs[i] = fData.costs[RES_ENERGY] * multEnergy + fData.costs[RES_MATERIAL1] * multMaterial +
                   fData.costs[RES_BLOBS] * multBlobs + fData.costs[RES_DIAMONDS] * multDiamonds;

        if(costs[i] > maxCost)
            maxCost = costs[i];

        // relative cost
        int costsIncluded = 0;

        const int energy = mPlayer->GetStat(Player::ENERGY).GetIntValue();
        const int material = mPlayer->GetStat(Player::MATERIAL).GetIntValue();
        const int blobs = mPlayer->GetStat(Player::BLOBS).GetIntValue();
        const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetIntValue();

        if(energy > 0)
        {
            relCosts[i] += fData.costs[RES_ENERGY] * 100 / energy;
            ++costsIncluded;
        }
        if(material > 0)
        {
            relCosts[i] += fData.costs[RES_MATERIAL1] * 100 / material;
            ++costsIncluded;
        }
        if(blobs > 0)
        {
            relCosts[i] += fData.costs[RES_BLOBS] * 100 / blobs;
            ++costsIncluded;
        }
        if(diamonds > 0)
        {
            relCosts[i] += fData.costs[RES_DIAMONDS] * 100 / diamonds;
            ++costsIncluded;
        }

        if(costsIncluded > 0)
            relCosts[i] /= costsIncluded;

        ++validUnits;
    }

    // can't create any unit -> exit
    if(0 == validUnits)
        return ;

    // 2- apply bonuses based on existing units
    const int bonusExistingUnit = -25;

    for(unsigned int i = 0; i < mPlayer->GetNumUnits(); ++i)
    {
        Unit * u = mPlayer->GetUnit(i);
        const GameObjectTypeId typeId = u->GetObjectType();
        const unsigned int ind = Unit::TypeToIndex(typeId);

        if(priorities[ind] <= 0)
            continue ;

        priorities[ind] += bonusExistingUnit;
    }

    // 3- apply bonuses based on unit type
    const int bonusCost = -15;
    const int bonusRelCost = -20;

    for(unsigned int i = 0; i < GameObject::NUM_UNIT_TYPES; ++i)
    {
        if(priorities[i] <= 0)
            continue ;

        // NOTE eventually costs can be weighted on the current resources.
        // i.e.: if you have a lot of X it doesn't really matter if a unit requires 20 or 40.
        priorities[i] += bonusCost * costs[i] / maxCost;
        priorities[i] += bonusRelCost * relCosts[i] / 100;
    }

    // 4- pick highest priority
    // check at least 1 priority is enough
    bool priorityOk = false;

    for(unsigned int i = 0; i < GameObject::NUM_UNIT_TYPES; ++i)
    {
        if(priorities[i] >= mMinPriority)
        {
            priorityOk = true;
            break;
        }
    }

    // can't find anything good enough
    if(!priorityOk)
        return ;

    // create action
    auto action = new ActionAINewUnit;
    action->type = AIA_NEW_UNIT;
    action->ObjSrc = s;
    action->priority = 0;
    action->unitType = GameObject::TYPE_NULL;

    // for now picking first of list when priorities are the same
    for(unsigned int i = 0; i < GameObject::NUM_UNIT_TYPES; ++i)
    {
        if(priorities[i] > action->priority)
        {
            action->priority = priorities[i];
            action->unitType = Unit::IndexToType(i);
        }
    }

    // no valid unit was found -> exit
    if(GameObject::TYPE_NULL == action->unitType)
    {
        mActionsDone.push_back(action);
        return ;
    }

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionsUnit(Unit * u)
{
    // ATTACK ENEMY
    AddActionUnitAttackEnemyUnit(u);

    // CONNECT STRUCTURES
    AddActionUnitConnectStructure(u);

    // CONQUEST RESOURCE GENERATORS
    AddActionUnitConquestResGen(u, RES_ENERGY);
    AddActionUnitConquestResGen(u, RES_MATERIAL1);
}

void PlayerAI::AddActionUnitAttackEnemyUnit(Unit * u)
{
    // nothing to do if there's no units on the map
    if(mUnits.empty())
        return ;

    if(IsObjectAlreadyDoingSimilarAction(u, AIA_UNIT_ATTACK_ENEMY_UNIT))
        return ;

    const PlayerFaction faction = mPlayer->GetFaction();
    const unsigned int numUnits = mUnits.size();

    // check if there's any unit to shoot at
    const int maxDist = mGm->GetNumRows() + mGm->GetNumCols();

    unsigned int bestUnitInd = numUnits;
    int minDist = maxDist;
    int priority = 100;

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

    auto action = new ActionAI;
    action->type = AIA_UNIT_ATTACK_ENEMY_UNIT;
    action->ObjSrc = u;
    action->ObjDst = mUnits[bestUnitInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConnectStructure(Unit * u)
{
    int priority = 100;

    // check if there's any structure to connect
    const unsigned int numStructures = mStructures.size();
    const int maxDist = mGm->GetNumRows() + mGm->GetNumCols();

    unsigned int bestStructInd = numStructures;
    int minDist = maxDist;

    const PlayerFaction faction = mPlayer->GetFaction();

    for(unsigned int i = 0; i < numStructures; ++i)
    {
        auto s = static_cast<Structure *>(mStructures[i]);

        // own structure which is not linked
        if(s->GetFaction() == faction && !s->IsLinked())
        {
            const int dist = mGm->ApproxDistance(u, s);

            if(dist < minDist)
            {
                minDist = dist;
                bestStructInd = i;
            }
        }
    }

    // can't find any structure to connect
    if(bestStructInd == numStructures)
        return ;

    // check active actions to avoid duplicates
    for(auto action : mActionsDoing)
    {
        // any unit is already doing the same -> exit
        if(action->type == AIA_UNIT_CONNECT_STRUCTURE)
            return ;
        // unit is already doing something -> exit
        // NOTE keeping the case as this might change to lower priority instead of nothing
        else if(action->ObjSrc == u)
            return ;
    }

    // scale priority based on unit's energy
    priority = priority * u->GetEnergy() / u->GetMaxEnergy();

    // scale priority based on unit's health
    priority = priority * u->GetHealth() / u->GetMaxHealth();

    // bonus distance
    const int bonusDist = -25;
    priority += bonusDist * minDist / maxDist;

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONNECT_STRUCTURE;
    action->ObjSrc = u;
    action->ObjDst = mStructures[bestStructInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConquestResGen(Unit * u, ResourceType type)
{
    const enum Player::Stat types[NUM_RESOURCES] =
    {
        Player::Stat::ENERGY,
        Player::Stat::MATERIAL,
        Player::Stat::DIAMONDS,
        Player::Stat::BLOBS
    };

    const int basePriorities[NUM_RESOURCES] =
    {
        90,     //RES_ENERGY
        75,     //RES_MATERIAL1
        45,     //RES_DIAMONDS
        45      //RES_BLOBS
    };

    const enum Player::Stat ptype = types[type];
    const StatValue & stat = mPlayer->GetStat(ptype);

    int priority = basePriorities[type];

    // check active actions to avoid duplicates
    for(auto action : mActionsDoing)
    {
        // any unit is already doing the same -> exit
        if(action->type == AIA_UNIT_CONQUER_GEN)
            return ;
        // unit is already doing something -> exit
        // NOTE keeping the case as this might change to lower priority instead of nothing
        else if(action->ObjSrc == u)
            return ;
    }

    // scale priority based on unit's energy
    priority = priority * u->GetEnergy() / u->GetMaxEnergy();

    // scale priority based on unit's health
    priority = priority * u->GetHealth() / u->GetMaxHealth();

    // bonus resource availability level
    const int bonusResAvailable = -20;

    priority += bonusResAvailable * stat.GetIntValue() / stat.GetIntMax();

    // visit all generators
    const int maxDist = mGm->GetNumRows() + mGm->GetNumCols();
    const unsigned int numGens = mResGenerators.size();

    const int bonusDist = -60;
    const int bonusOwned = -40;

    unsigned int indexMax = numGens;
    int maxPriority = 0;

    int totGenerators = 0;
    int ownedGenerators = 0;
    int unlinkedGenerators = 0;

    for(unsigned int i = 0; i < numGens; ++i)
    {
        auto resGen = static_cast<ResourceGenerator *>(mResGenerators[i]);

        if(resGen->GetResourceType() != type)
            continue;

        ++totGenerators;

        const PlayerFaction playerFaction = mPlayer->GetFaction();
        const PlayerFaction resGenFaction = resGen->GetFaction();

        if(playerFaction == resGenFaction)
        {
            ++ownedGenerators;

            if(!resGen->IsLinked())
                ++unlinkedGenerators;

            continue;
        }

        int loopPriority = priority;

        // bonus distance
        const int dist = mGm->ApproxDistance(u, resGen);
        loopPriority += bonusDist * dist / maxDist;

        // bonus owned by enemy
        const int owned = resGenFaction != NO_FACTION;
        loopPriority += owned * bonusOwned;

        if(loopPriority > maxPriority)
        {
            maxPriority = loopPriority;
            indexMax = i;
        }
    }

    priority = maxPriority;

    // bonus owned and unlinked generators
    if(ownedGenerators > 0)
    {
        const int bonusOwnned = -30;
        priority += bonusOwned * ownedGenerators / totGenerators;

        const int bonusUnlinked = -10;
        priority += bonusUnlinked * unlinkedGenerators / ownedGenerators;
    }

    // bonus unit speed
    const int bonusSpeed = 10;
    priority += bonusSpeed * u->GetStat(OSTAT_SPEED) / ObjectFactionData::MAX_STAT_VAL;

    // bonus unit conquest
    const int bonusConquest = 20;
    priority += bonusConquest * u->GetStat(OSTAT_CONQUEST) / ObjectFactionData::MAX_STAT_VAL;

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONQUER_GEN;
    action->ObjSrc = u;
    action->ObjDst = mResGenerators[indexMax];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

bool PlayerAI::IsObjectAlreadyDoingSimilarAction(GameObject * obj, AIActionType type) const
{
    for(const ActionAI * a : mActionsDoing)
    {
        if(a->ObjSrc == obj && a->type == type)
            return true;
    }

    return false;
}

bool PlayerAI::IsSimilarActionInProgress(AIActionType type) const
{
    for(const ActionAI * a : mActionsDoing)
    {
        if(a->type == type)
            return true;
    }

    return false;
}

} // namespace game
