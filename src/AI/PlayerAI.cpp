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

    if(IsObjectAlreadyDoingSimilarAction(u, AIA_UNIT_ATTACK_ENEMY_UNIT))
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
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

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

void PlayerAI::AddActionUnitCollectBlobs(Unit * u)
{
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

    // DEFINE PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on owned blobs
    const int decBlobs = -30;
    const StatValue & blobs = mPlayer->GetStat(Player::BLOBS);
    const int maxBlobs = blobs.GetIntMax();
    const int numBlobs = blobs.GetIntValue();
    priority += decBlobs * numBlobs / maxBlobs;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPiorityBonusDistance(u, minDist, bonusDist);

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

    // DEFINE PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on owned diamonds
    const int decDiamonds = -30;
    const StatValue & diamonds = mPlayer->GetStat(Player::DIAMONDS);
    const int maxDiamonds = diamonds.GetIntMax();
    const int numDiamonds = diamonds.GetIntValue();
    priority += decDiamonds * numDiamonds / maxDiamonds;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPiorityBonusDistance(u, minDist, bonusDist);

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

    // DEFINE PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

    // bonus distance
    const float bonusDist = -25.f;
    priority += GetUnitPiorityBonusDistance(u, minDist, bonusDist);

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

    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -30.f;
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

    // bonus distance
    const float bonusDist = -50.f;
    priority += GetUnitPiorityBonusDistance(u, minDist, bonusDist);

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
    priority += GetUnitPiorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -10.f;
    priority += GetUnitPiorityBonusHealth(u, bonusHealth);

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
    priority += GetUnitPiorityBonusDistance(u, minDist, bonusDist);

    // decrease priority for owned generators
    const float bonusOwned = -30.f;
    priority += std::roundf(bonusOwned * ownedGenerators / totGenerators);

    // bonus unit conquest
    const int bonusConquest = 10;
    priority += bonusConquest * u->GetStat(OSTAT_CONQUEST) / ObjectFactionData::MAX_STAT_VAL;

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

int PlayerAI::GetUnitPiorityBonusDistance(const Unit * u, int dist, float bonus) const
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

int PlayerAI::GetUnitPiorityBonusEnergy(const Unit * u, float bonus) const
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

int PlayerAI::GetUnitPiorityBonusHealth(const Unit * u, float bonus) const
{
    const float maxHealth = u->GetMaxHealth();
    return std::roundf(bonus * (maxHealth - u->GetHealth()) / maxHealth);
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
