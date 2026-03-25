#include "PlayerAI.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "GameObjects/CityBlock.h"
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
    mOwnStructures.clear();
    mOwnUnits.clear();
    mResGenerators.clear();
    mTrees.clear();
    mVisibleEnemies.clear();
    mVisibleEnemyStructures.clear();
    mVisibleEnemyUnits.clear();

    // collect data
    const std::vector<GameObject *> & objects = mGm->GetObjects();

    const PlayerFaction factionAI = mPlayer->GetFaction();

    for(GameObject * obj : objects)
    {
        const GameObjectCategoryId objCat = obj->GetObjectCategory();
        const PlayerFaction objFaction = obj->GetFaction();

        // store ALL resource generators
        if(objCat == ObjectData::CAT_RES_GENERATOR)
            mResGenerators.push_back(obj);

        // own stuff
        if(objFaction == factionAI)
        {
            // own structures
            if(obj->IsStructure())
                mOwnStructures.push_back(obj);
            // own units
            else if(obj->GetObjectCategory() == ObjectData::CAT_UNIT)
                mOwnUnits.push_back(obj);
        }
        // enemies
        else if(objFaction != NO_FACTION)
        {
            const int cellIdxObj = obj->GetRow0() * mGm->GetNumCols() + obj->GetCol0();

            if(mPlayer->IsCellVisible(cellIdxObj))
            {
                mVisibleEnemies.emplace_back(obj);

                // enemy structures
                if(obj->IsStructure())
                    mVisibleEnemyStructures.push_back(obj);
                // enemy units
                else if(obj->GetObjectCategory() == ObjectData::CAT_UNIT)
                    mVisibleEnemyUnits.push_back(obj);
            }
        }
        // NO FACTION
        else
        {
            // store all collectables
            if(obj->CanBeCollected())
                mCollectables.push_back(obj);
            // store all trees
            else if(obj->GetObjectType() == ObjectData::TYPE_TREES)
                mTrees.push_back(obj);
        }
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
        AddActionsStructure(s);
    }

    // UNITS
    for(unsigned int i = 0; i < mPlayer->GetNumUnits(); ++i)
    {
        Unit * u = mPlayer->GetUnit(i);
        AddActionsUnit(u);
    }

    // KEEP THIS LAST
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

bool PlayerAI::FindWhereToBuildStructure(Unit * unit, Cell2D & target) const
{
    const GameObjectTypeId type = unit->GetStructureToBuild();
    const ObjectData & data = mDataReg->GetObjectData(type);
    const int rows = data.GetRows();
    const int cols = data.GetCols();

    // DECIDE WHERE TO LOOK FOR BUILDING AREA
    const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
    Cell2D cellStart;

    std::vector<Structure *> structures;

    // build close to existing similar structure
    if(mPlayer->HasStructure(type))
        structures = mPlayer->GetStructuresByType(type);
    // no similar structure -> build close to base
    else
        structures = mPlayer->GetStructuresByType(ObjectData::TYPE_BASE);

    // find similar structure which is closest to unit
    unsigned int bestInd = 0;
    int bestDist = mGm->GetNumRows() + mGm->GetNumCols();

    for(unsigned int s = 0; s < structures.size(); ++s)
    {
        Structure * structure = structures[s];

        // strucure made of multiple cells -> check 4 corners
        if(rows > 1 || cols > 1)
        {
            // top-left
            const Cell2D tl(structure->GetRow1(), structure->GetCol1());
            const int distTL = mGm->ApproxDistance(cellUnit, tl);

            if(distTL < bestDist)
            {
                cellStart = tl;
                bestDist = distTL;
            }

            // top-right
            const Cell2D tr(structure->GetRow1(), structure->GetCol0());
            const int distTR = mGm->ApproxDistance(cellUnit, tr);

            if(distTR < bestDist)
            {
                cellStart = tr;
                bestDist = distTR;
            }

            // bottom-left
            const Cell2D bl(structure->GetRow0(), structure->GetCol1());
            const int distBL = mGm->ApproxDistance(cellUnit, bl);

            if(distBL < bestDist)
            {
                cellStart = bl;
                bestDist = distBL;
            }
        }

        // bottom-right
        const Cell2D br(structure->GetRow0(), structure->GetCol0());
        const int distBR = mGm->ApproxDistance(cellUnit, br);

        if(distBR < bestDist)
        {
            cellStart = br;
            bestDist = distBR;
        }
    }

    // find suitable spot close to cellStart
    const int maxRadius = mGm->GetNumRows() / 2;

    // first try to find an area big enough to have all sides free
    if(mGm->FindFreeArea(cellStart, rows + 2, cols + 2, maxRadius, target))
    {
        target.row -= 1;
        target.col -= 1;

        return true;
    }
    else
        return mGm->FindFreeArea(cellStart, rows, cols, maxRadius, target);
}

bool PlayerAI::FindWhereToBuildTower(Unit * unit, Cell2D & target) const
{
    const GameObjectTypeId type = unit->GetStructureToBuild();
    const ObjectData & data = mDataReg->GetObjectData(type);
    const int rows = data.GetRows();
    const int cols = data.GetCols();

    // DECIDE WHERE TO LOOK FOR BUILDING AREA
    const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());

    // std::cout << "PlayerAI::FindWhereToBuildTower - unit: " << unit->GetRow0() << ","
    //           << unit->GetCol0() << std::endl;

    // define current base area
    const int mapRows = mGm->GetNumRows();
    const int mapCols = mGm->GetNumCols();
    const int maxDist = mapRows + mapCols;
    const Cell2D mapCenter(mapRows / 2, mapCols / 2);

    const unsigned int numStruct = mOwnStructures.size();
    std::vector<int> scores(numStruct, MAX_PRIORITY);

    const int radius = 3;

    for(unsigned int i = 0; i < numStruct; ++i)
    {
        const auto s = mOwnStructures[i];
        const auto st = s->GetObjectType();

        // ignore other towers
        if(st == ObjectData::TYPE_BUNKER || st == ObjectData::TYPE_DEFENSIVE_TOWER)
        {
            scores[i] = 0;
            continue;
        }

        // penalize structures already surrounded by towers
        const Cell2D tl(s->GetRow1() - radius, s->GetCol1() - radius);
        const Cell2D br(s->GetRow0() + radius, s->GetCol0() + radius);
        const int bonusOthers = -20;
        scores[i] += GetNumStructuresInArea(tl, br, type) * bonusOthers;

        // penalize structures that are far
        const int dist = mGm->ApproxDistance(unit, s);
        const int bonusDist = -30;
        scores[i] += bonusDist * dist / maxDist;
    }

    // for(unsigned int i = 0; i < numStruct; ++i)
    //     std::cout << "PlayerAI::FindWhereToBuildTower - structure: "
    //               << mOwnStructures[i]->GetRow0() << "," << mOwnStructures[i]->GetCol0()
    //               << " score[" << i << "] = " << scores[i] << std::endl;

    int bestScore = 0;
    unsigned int bestInd = numStruct;

    for(unsigned int i = 0; i < numStruct; ++i)
    {
        if(scores[i] > bestScore)
        {
            bestScore = scores[i];
            bestInd = i;
        }
    }

    if(bestInd == numStruct)
        return false;

    // find suitable spot close to cellStart
    const auto bestStructure = mOwnStructures[bestInd];
    const int areaRows = rows + 2;
    const int areaCols = cols + 2;

    // start searching from corner closest to unit
    const Cell2D structTL(bestStructure->GetRow1(), bestStructure->GetCol1());
    const Cell2D structBR(bestStructure->GetRow0(), bestStructure->GetCol0());

    const Cell2D start = (mGm->ApproxDistance(cellUnit, structTL) >
                          mGm->ApproxDistance(cellUnit, structBR)) ?
                         structBR : structTL;

    // try to find a bigger area first
    if(mGm->FindFreeArea(start, areaRows, areaCols, radius, target))
    {
        target.row -= 1;
        target.col -= 1;

        // std::cout << "PlayerAI::FindWhereToBuildTower - start: "
        //           << start.row << "," << start.col << " - target : "
        //           << target.row << "," << target.col << std::endl;

        return true;
    }
    else
        return mGm->FindFreeArea(start, rows, cols, radius, target);
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

    //PrintdActionDebug("PlayerAI::AddNewAction | ADDED NEW ACTION", action);

    // NOTE not checking existing actions for now as all actions should be unique
    // as they are created by different objects (at least the ObjSrc is different)
    PushAction(action);
}

void PlayerAI::AddActionEndTurn()
{
    // create action
    auto action = new ActionAI;
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

    for(GameObject * obj : mOwnUnits)
    {
        totEnergy += obj->GetEnergy();
        totMaxEnergy += obj->GetMaxEnergy();
    }

    const float energyLeftUnits = totEnergy * c / totMaxEnergy;

    // consider total structures energy
    totEnergy = 0.f;
    totMaxEnergy = 0.f;

    for(GameObject * obj : mOwnStructures)
    {
        const GameObjectTypeId type = obj->GetObjectType();

        // only consider structure that have AI actions
        if(type == ObjectData::TYPE_BASE || type == ObjectData::TYPE_BARRACKS)
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
    // action not added
    else
        delete action;
}

void PlayerAI::AddActionsStructure(Structure * s)
{
    // upgrade when possible
    AddActionStructureUpgrade(s);

    // do not consider unit if energy is too low
    const float minEnergy = 5.f;

    if(s->GetEnergy() < minEnergy || mPlayer->GetTurnEnergy() < minEnergy)
        return ;

    // type specific actions
    const GameObjectTypeId objType = s->GetObjectType();

    if(objType == ObjectData::TYPE_BASE)
        AddActionBaseCreateUnit(s);
    else if (objType == ObjectData::TYPE_BARRACKS)
        AddActionBarrackCreateUnit(s);
}

void PlayerAI::AddActionBaseCreateUnit(Structure * base)
{
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    // can't build more units -> exit
    if(numUnits >= limitUnits)
        return ;

    // DECIDE UNIT TYPE
    const auto availableUnits = mPlayer->GetAvailableUnits();

    std::vector<GameObjectTypeId> types;

    for(GameObjectTypeId t : availableUnits)
    {
        const ObjectData & data = mDataReg->GetObjectData(t);

        if(OCU_WORKER == data.GetClass())
            types.push_back(t);
    }

    // try to create one
    AddActionCreateUnit(base, types);
}

void PlayerAI::AddActionBarrackCreateUnit(Structure * barrack)
{
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    // can't build more units -> exit
    if(numUnits >= limitUnits)
        return ;

    // DECIDE UNIT TYPE
    const auto availableUnits = mPlayer->GetAvailableUnits();

    std::vector<GameObjectTypeId> types;

    for(GameObjectTypeId t : availableUnits)
    {
        const ObjectData & data = mDataReg->GetObjectData(t);

        if(OCU_SOLDIER == data.GetClass())
            types.push_back(t);
    }

    // try to create one
    AddActionCreateUnit(barrack, types);
}

void PlayerAI::AddActionStructureUpgrade(Structure * s)
{
    if(!s->CanBeUpgraded())
        return ;

    // define importance of each attribute for this object
    const int weights[] =
    {
        8, 9, 0, 6, 8, 7, 0, 0, 0, 0, 0, 6, 9
    };

    static_assert(sizeof(weights) / sizeof(int) == NUM_BASIC_ATTRIBUTES);

    AddActionUpgrade(s, weights, AIA_UPGRADE_STRUCTURE);
}

void PlayerAI::AddActionsUnit(Unit * u)
{
    // upgrade immediatly when possible
    AddActionUnitUpgrade(u);

    // do not consider unit if energy is too low
    const float minEnergy = 5.f;

    if(u->GetEnergy() < minEnergy || mPlayer->GetTurnEnergy() < minEnergy)
        return ;

    // ATTACK
    if(u->CanAttack())
    {
        AddActionUnitAttackEnemyUnit(u);
        AddActionUnitAttackTrees(u);
        AddActionUnitPatrol(u);
    }

    if(u->CanConquer())
    {
        // CONNECT STRUCTURES
        AddActionUnitConnectStructure(u);

        // CONQUEST RESOURCE GENERATORS
        AddActionUnitConquerResGen(u, RES_ENERGY);
        AddActionUnitConquerResGen(u, RES_MATERIAL1);
        AddActionUnitConquerCity(u);
    }

    // BUILD
    if(u->CanBuild())
    {
        AddActionUnitBuildStructure(u);
        AddActionUnitBuildTower(u);
    }

    // COLLECTABLES
    AddActionUnitCollectBlobs(u);
    AddActionUnitCollectDiamonds(u);
    AddActionUnitCollectLootbox(u);
}

void PlayerAI::AddActionUnitAttackEnemyUnit(Unit * u)
{
    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::ATTACK))
        return ;

    // nothing to do if there's no visible enemy units
    if(mVisibleEnemyUnits.empty())
        return ;

    const unsigned int numUnits = mVisibleEnemyUnits.size();

    // check if there's any unit to shoot at
    const int maxDist = GetMaxDistanceForObject(u);

    unsigned int bestUnitInd = numUnits;
    int minDist = maxDist;
    int priority = MAX_PRIORITY;

    for(unsigned int i = 0; i < numUnits; ++i)
    {
        auto unit = static_cast<Unit *>(mVisibleEnemyUnits[i]);

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
    action->ObjDst = mVisibleEnemyUnits[bestUnitInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitAttackTrees(Unit * u)
{
    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::ATTACK))
        return ;

    if(mTrees.empty())
        return ;

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
    const unsigned int numTrees = mTrees.size();
    const int maxDist = mGm->GetNumRows() * mGm->GetNumCols();
    const Base * base = mPlayer->GetBase();

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
    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::BUILD_STRUCTURE))
        return ;

    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -30.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // decrease priority based on distance from Base
    const GameObject * base = mPlayer->GetBase();
    const int distBase = mGm->ApproxDistance(u, base);
    const int maxDist = GetMaxDistanceForObject(u);
    const float bonusDistBase = -10.f;
    priority += std::roundf(bonusDistBase * distBase / maxDist);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // RESOURCE GENERATORS
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR))
        AddActionUnitBuildResourceGenerator(u, RES_ENERGY, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT))
        AddActionUnitBuildResourceGenerator(u, RES_MATERIAL1, priority);

    // RESOURCE STORAGE
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_STORAGE_ENERGY))
        AddActionUnitBuildResourceStorage(u, RES_ENERGY, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_STORAGE_MATERIAL))
        AddActionUnitBuildResourceStorage(u, RES_MATERIAL1, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_STORAGE_BLOBS))
        AddActionUnitBuildResourceStorage(u, RES_BLOBS, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RES_STORAGE_DIAMONDS))
        AddActionUnitBuildResourceStorage(u, RES_DIAMONDS, priority);

    // UNIT CREATION
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_BARRACKS))
        AddActionUnitBuildUnitCreator(u, ObjectData::TYPE_BARRACKS, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_HOSPITAL))
        AddActionUnitBuildUnitCreator(u, ObjectData::TYPE_HOSPITAL, priority);

    // TECH
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RESEARCH_CENTER))
        AddActionUnitBuildResearchCenter(u, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RADAR_STATION))
        AddActionUnitBuildRadarStructure(u, ObjectData::TYPE_RADAR_STATION, priority);
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_RADAR_TOWER))
        AddActionUnitBuildRadarStructure(u, ObjectData::TYPE_RADAR_TOWER, priority);

    // OTHER
    if(mPlayer->IsStructureAvailable(ObjectData::TYPE_PRACTICE_TARGET))
        AddActionUnitBuildPracticeTarget(u, priority);
}

void PlayerAI::AddActionUnitBuildTower(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -25.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // decrease priority based on low number of structures
    const unsigned int numStruct = mOwnStructures.size();
    const unsigned int minStructs = 4;

    if(numStruct < minStructs)
    {
        const float bonusStructs = -10.f;
        priority += bonusStructs * (minStructs - numStruct);
    }

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // define towers to build
    std::vector<GameObjectTypeId> towerIds;

     if(mPlayer->IsStructureAvailable(ObjectData::TYPE_BUNKER))
        towerIds.emplace_back(ObjectData::TYPE_BUNKER);
     if(mPlayer->IsStructureAvailable(ObjectData::TYPE_DEFENSIVE_TOWER))
        towerIds.emplace_back(ObjectData::TYPE_DEFENSIVE_TOWER);

    // no tower available
    if(towerIds.empty())
        return ;

    GameObjectTypeId bestType = ObjectData::TYPE_NULL;
    int bestPriority = 0;
    int typePriority;

    for(GameObjectTypeId type : towerIds)
    {
        if(!HasPlayerResourcesToBuild(type))
            continue;

        typePriority = priority;

        // reduce priority based on available resources
        if(type == ObjectData::TYPE_DEFENSIVE_TOWER)
            typePriority += GetPriorityBonusStructureBuildCost(type, -5.f);
        else
            typePriority += GetPriorityBonusStructureBuildCost(type, -10.f);

        // reduce priority based on same existing structures
        const float bonusSameStruct = -5.f;
        typePriority += GetPriorityBonusSameStructureCreated(type, bonusSameStruct);

        if(typePriority > bestPriority)
        {
            bestPriority = typePriority;
            bestType = type;
        }
    }

    // can't find anything good, this shouldn't happen
    if(bestType == ObjectData::TYPE_NULL)
        return ;

    // check if below current priority threshold
    if(bestPriority < mMinPriority)
        return ;

    // CREATE ACTION
    auto action = new ActionAIBuildStructure;
    action->type = AIA_UNIT_BUILD_TOWER;
    action->ObjSrc = u;
    action->priority = bestPriority;
    action->structType = bestType;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitBuildUnitCreator(Unit * u, GameObjectTypeId structType, int priority0)
{
    // check object type is supported
    if(structType != ObjectData::TYPE_BARRACKS && structType != ObjectData::TYPE_HOSPITAL)
        return ;

    // no need to build these structures more than once
    if(mPlayer->HasStructure(structType))
        return ;

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    // can't build more units -> exit
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    if(numUnits >= limitUnits)
        return ;

    int priority = priority0;

    // the more units exist the higher the priority as no unit can be created yet
    const std::unordered_map<GameObjectTypeId, float> bonusUnits
    {
        {ObjectData::TYPE_BARRACKS , 30.f},
        {ObjectData::TYPE_HOSPITAL , 15.f},
    };

    priority += std::roundf(bonusUnits.at(structType) * numUnits / limitUnits);

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
        ObjectData::TYPE_RES_GEN_ENERGY_SOLAR,
        ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT
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

    const int resCur = res.GetValue();
    const int resMax = res.GetMax();

    const float bonusGen = -35.f;
    priority += std::roundf(bonusGen * resCur / resMax);

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
        ObjectData::TYPE_RES_STORAGE_ENERGY,
        ObjectData::TYPE_RES_STORAGE_MATERIAL,
        ObjectData::TYPE_RES_STORAGE_DIAMONDS,
        ObjectData::TYPE_RES_STORAGE_BLOBS,
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

    const int resCur = res.GetValue();
    const int resMax = res.GetMax();
    const int resSto = resMax - resCur;

    const float bonusStorage = -30.f;
    priority += std::roundf(bonusStorage * resSto / resMax);

    // reduce priority based on available resources
    const float bonusRes = -5.f;
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes);

    // reduce priority based on same existing structures
    const float bonusSameStruct = -15.f;
    priority += GetPriorityBonusSameStructureCreated(structType, bonusSameStruct);

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

void PlayerAI::AddActionUnitBuildResearchCenter(Unit * u, int priority0)
{
    const GameObjectTypeId structType = ObjectData::TYPE_RESEARCH_CENTER;

    // no need to build these structures more than once
    if(mPlayer->HasStructure(structType))
        return ;

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    int priority = priority0;

    // reduce priority based on available resources
    const float bonusRes = -10.f;
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

void PlayerAI::AddActionUnitBuildPracticeTarget(Unit * u, int priority0)
{
    const GameObjectTypeId structType = ObjectData::TYPE_PRACTICE_TARGET;

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    int priority = priority0;

    // reduce priority based on available resources
    const float bonusRes = -30.f;
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes);

    // reduce priority based on number or existing structure
    const float bonusExist = -30.f;
    priority += bonusExist * mPlayer->GetNumStructuresByType(structType);

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

void PlayerAI::AddActionUnitBuildRadarStructure(Unit * u, GameObjectTypeId structType, int priority0)
{
    // check object type is supported
    if(structType != ObjectData::TYPE_RADAR_STATION &&
       structType != ObjectData::TYPE_RADAR_TOWER)
        return ;

    // no need to build these structures more than once
    if(ObjectData::TYPE_RADAR_STATION == structType && mPlayer->HasStructure(structType))
        return ;

    // not enough resources to build
    if(!HasPlayerResourcesToBuild(structType))
        return ;

    int priority = priority0;

    // reduce priority based on available resources
    const std::unordered_map<GameObjectTypeId, float> bonusRes
    {
        {ObjectData::TYPE_RADAR_STATION , -30.f},
        {ObjectData::TYPE_RADAR_TOWER , -40.f},
    };
    priority += GetPriorityBonusStructureBuildCost(structType, bonusRes.at(structType));

    // reduce priority based on number or existing structure
    const float bonusExist = -15.f;
    priority += GetPriorityBonusSameStructureCreated(structType, bonusExist);

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
    priority += std::roundf(decBlobs * blobs.GetValue() / blobs.GetMax());

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
        if(c->GetObjectType() != ObjectData::TYPE_BLOBS)
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
    priority += std::roundf(decDiamonds * diamonds.GetValue() / diamonds.GetMax());

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
        if(c->GetObjectType() != ObjectData::TYPE_DIAMONDS)
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
        const GameObjectTypeId type = c->GetObjectType();

        // no lootbox or special lootbox
        if(type != ObjectData::TYPE_LOOTBOX && type != ObjectData::TYPE_LOOTBOX2)
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

    // double negative bonus for health in case unit is collecting a special LootBox
    if(mCollectables[bestInd]->GetObjectType() == ObjectData::TYPE_LOOTBOX2)
        priority += GetUnitPriorityBonusHealth(u, bonusHealth);

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
    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::CONQUER_CELL))
        return ;

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

    // check if there's any structure to connect
    const unsigned int numStructures = mOwnStructures.size();

    unsigned int bestStructInd = numStructures;
    int minDist = GetMaxDistanceForObject(u);
    Cell2D startConquest;

    const PlayerFaction faction = mPlayer->GetFaction();

    const Cell2D posUnit(u->GetRow0(), u->GetCol0());

    for(unsigned int i = 0; i < numStructures; ++i)
    {
        auto s = static_cast<Structure *>(mOwnStructures[i]);

        // handle not linked only
        if(s->IsLinked())
            continue;

        Cell2D start;

        // SPECIAL CASE: unit is already next to structure -> best option
        if(mGm->AreObjectsOrthoAdjacent(u, s))
        {
            minDist = 0;
            bestStructInd = i;
            startConquest = posUnit;

            // std::cout << "PlayerAI::AddActionUnitConnectStructure - ADJ - structure: " << s->GetObjectId()
            //           << " - min dist: 0"
            //           << " - dest/obj pos: " << startConquest.row << "," << startConquest.col
            //           << " - obj: " << u->GetObjectId() << std::endl;

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

                // std::cout << "PlayerAI::AddActionUnitConnectStructure - A - structure: " << s->GetObjectId()
                //           << " - new min dist: " << dist
                //           << " - dest: " << startConquest.row << "," << startConquest.col
                //           << " - obj: " << u->GetObjectId()
                //           << " - obj pos: " << posUnit.row << "," << posUnit.col << std::endl;
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

                // std::cout << "PlayerAI::AddActionUnitConnectStructure - B - structure: " << s->GetObjectId()
                //           << " - new min dist: " << dist
                //           << " - dest: " << startConquest.row << "," << startConquest.col
                //           << " - obj: " << u->GetObjectId()
                //           << " - obj pos: " << posUnit.row << "," << posUnit.col << std::endl;

                // already found best option
                if(0 == dist)
                    break;
            }
        }
    }

    // can't find any structure to connect
    if(bestStructInd == numStructures)
        return ;

    // bonus distance
    const float bonusDist = -35.f;
    priority += GetUnitPriorityBonusDistance(u, minDist, bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONNECT_STRUCTURE;
    action->ObjSrc = u;
    action->cellSrc = startConquest;
    action->ObjDst = mOwnStructures[bestStructInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConquerCity(Unit * u)
{
    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::CONQUER_STRUCTURE))
        return ;

    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -30.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // bonus energy availability level
    const StatValue & stat = mPlayer->GetStat(Player::Stat::ENERGY);

    const float bonusRes = -40.f;
    priority += std::roundf(bonusRes * stat.GetValue() / stat.GetMax());

    // action is already not doable
    if(priority < mMinPriority)
        return ;

    // FIND CITY BLOCK
    const std::vector<GameObject *> & objects = mGm->GetObjects();

    const PlayerFaction factionAI = mPlayer->GetFaction();

    unsigned int bestDist = mGm->GetNumRows() + mGm->GetNumCols();
    GameObject * bestCB = nullptr;

    for(GameObject * obj : objects)
    {
        // consider only CityBlocks not already conquered and border
        if(obj->GetObjectType() != ObjectData::TYPE_CITY_BLOCK ||
           obj->GetFaction() == factionAI || !static_cast<CityBlock *>(obj)->IsBorder())
            continue;

        const unsigned int dist = mGm->ApproxDistance(obj, u);

        if(dist < bestDist)
        {
            bestDist = dist;
            bestCB = obj;
        }
    }

    // didn't find any
    if(bestCB == nullptr)
        return ;

    // bonus distance
    const float bonusDist = -20.f;
    priority += GetUnitPriorityBonusDistance(u, bestDist, bonusDist);

    // bonus unit conquest
    const float bonusConquest = 20.f;
    priority += std::roundf(bonusConquest *
                            u->GetAttribute(OBJ_ATT_CONQUEST) / ObjectData::MAX_STAT_VAL);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONQUER_STRUCT;
    action->ObjSrc = u;
    action->ObjDst = bestCB;
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitConquerResGen(Unit * u, ResourceType type)
{
    // resource not supported
    if(type != RES_ENERGY && type != RES_MATERIAL1)
        return ;

    // not enough energy to consider this action now
    if(!u->HasEnergyForActionStep(GameObjectActionType::CONQUER_STRUCTURE))
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
    const float bonusHealth = -5.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // bonus resource availability level
    const float bonusRes = -15.f;
    priority += std::roundf(bonusRes * stat.GetValue() / stat.GetMax());

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

    // bonus unit conquest
    const float bonusConquest = 20.f;
    priority += std::roundf(bonusConquest *
                            u->GetAttribute(OBJ_ATT_CONQUEST) / ObjectData::MAX_STAT_VAL);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_CONQUER_STRUCT;
    action->ObjSrc = u;
    action->ObjDst = mResGenerators[bestInd];
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitPatrol(Unit * u)
{
    // DEFINE INITIAL PRIORITY
    int priority = MAX_PRIORITY;

    // decrease priority based on unit's energy
    const float bonusEnergy = -30.f;
    priority += GetUnitPriorityBonusEnergy(u, bonusEnergy);

    // decrease priority based on unit's health
    const float bonusHealth = -15.f;
    priority += GetUnitPriorityBonusHealth(u, bonusHealth);

    // already below current priority threshold
    if(priority < mMinPriority)
        return ;

    // FIND TARGET DESTINATION
    const int maxDist = mGm->GetNumRows() + mGm->GetNumCols();

    Cell2D dest;

    // closest enemy unit
    if(!mVisibleEnemyUnits.empty())
    {
        int bestDist = maxDist;

        for(const auto e : mVisibleEnemyUnits)
        {
            const int d = mGm->ApproxDistance(u, e);

            if(d < bestDist)
            {
                bestDist = d;
                dest.row = e->GetRow0();
                dest.col = e->GetCol0();
            }
        }

        std::cout << "PlayerAI::AddActionUnitPatrol - unit("
                  << u->GetRow0() << "," << u->GetCol0()
                  << ") - TARGET enemy: " << dest.row << ","
                  << dest.col << std::endl;
    }
    else
    {
        // define 4 corners of territory
        enum CornerId : unsigned int
        {
            C_TL,
            C_TR,
            C_BR,
            C_BL,

            NUM_CORNERS,
        };

        std::vector<Cell2D> corners =
        {
            {maxDist, maxDist},
            {maxDist, 0},
            {0, 0},
            {0, maxDist},
        };

        for(const auto os : mOwnStructures)
        {
            const int r0 = os->GetRow0();
            const int c0 = os->GetCol0();

            if(r0 < corners[C_TL].row)
            {
                corners[C_TL].row = r0;
                corners[C_TR].row = r0;
            }
            else if(r0 > corners[C_BR].row)
            {
                corners[C_BL].row = r0;
                corners[C_BR].row = r0;
            }

            if(c0 < corners[C_TL].col)
            {
                corners[C_TL].col = c0;
                corners[C_BL].col = c0;
            }
            else if(c0 > corners[C_BR].col)
            {
                corners[C_TR].col = c0;
                corners[C_BR].col = c0;
            }
        }

        // define probabilities to each corner based on distance
        const Cell2D uc(u->GetRow0(), u->GetCol0());

        std::vector<float> distances(NUM_CORNERS, 0);
        float totDist = 0.f;

        for(unsigned int i = 0; i < NUM_CORNERS; ++ i)
        {
            distances[i] = mGm->ApproxDistance(uc, corners[i]);
            totDist += distances[i];
        }

        const float maxProb = 100.f;
        std::vector<float> probs(NUM_CORNERS, 0.f);

        probs[C_TL] = maxProb * distances[C_TL] / totDist;
        probs[C_TR] = maxProb * distances[C_TR] / totDist;
        probs[C_BL] = maxProb * distances[C_BL] / totDist;
        probs[C_BR] = maxProb - probs[C_TL] - probs[C_TR] - probs[C_BL];

        sgl::utilities::LoadedDie ld(probs);
        dest = corners[ld.GetNextValue()];

        std::cout << "PlayerAI::AddActionUnitPatrol - unit("
                  << u->GetRow0() << "," << u->GetCol0()
                  << ") - TARGET corner: " << dest.row << ","
                  << dest.col << std::endl;
    }

    // destination cell is not walkable -> find closer one
    if(!mGm->IsCellWalkable(dest.row, dest.col))
    {
        dest = mGm->GetCloseMoveTarget(Cell2D(u->GetRow0(), u->GetCol0()), dest);

        // failed to find a target cell
        if(dest.row < 0 || dest.col < 0)
            return ;
    }

    // reduce priority based on Distance from target
    const float bonusDist = -0.5f;
    const int dist = std::abs(dest.row - u->GetRow0()) + std::abs(dest.col - u->GetCol0());
    priority += std::roundf(dist * bonusDist);

    // can't find something that's worth an action
    if(priority < mMinPriority)
        return ;

    auto action = new ActionAI;
    action->type = AIA_UNIT_PATROL;
    action->ObjSrc = u;
    action->cellDst = dest;
    action->priority = priority;

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUnitUpgrade(Unit * u)
{
    if(!u->CanBeUpgraded())
        return ;

    // define importance of each attribute for this object
    const int weights[] =
    {
        9, 8, 0, 6, 7, 6, 3, 3, 3, 6, 6, 3, 0
    };

    static_assert(sizeof(weights) / sizeof(int) == NUM_BASIC_ATTRIBUTES);

    AddActionUpgrade(u, weights, AIA_UPGRADE_UNIT);
}

void PlayerAI::AddActionCreateUnit(Structure * gen, const std::vector<GameObjectTypeId> & types)
{
    const unsigned int numUnits = mPlayer->GetNumUnits();
    const unsigned int limitUnits = mPlayer->GetMaxUnits();

    int priority = MAX_PRIORITY;

    // the more units exist the lower the priority
    const float bonusUnits = -25.f;
    priority += std::roundf(bonusUnits * numUnits / limitUnits);

    // decrease priority based on base's energy
    const float bonusEnergy = -20.f;
    priority += GetStructurePriorityBonusEnergy(gen, bonusEnergy);

    // priority already too low
    if(priority < mMinPriority)
        return ;

    const unsigned int numTypes = types.size();

    const int energy = mPlayer->GetStat(Player::ENERGY).GetValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetValue();

    unsigned int bestInd = numTypes;
    int bestPriority = mMinPriority;

    for(unsigned int i = 0; i < numTypes; ++i)
    {
        const GameObjectTypeId t = types[i];

        // can't create this unit -> next
        if(!mGm->CanCreateUnit(t, gen, mPlayer))
            continue;

        int typePriority = priority;

        // reduce priority for existing same type units
        const float bonusSameType = -25.f;
        typePriority += std::roundf(bonusSameType * mPlayer->GetNumUnitsByType(t));

        // reduce priority based on available resources
        // NOTE all costs are < current resources or CanCreateUnit would have returned false
        const ObjectData & data = mDataReg->GetObjectData(t);
        const auto & costs = data.GetCosts();

        const float bonusRes = -10.f;

        if(costs[RES_ENERGY] > 0)
        {
            if(energy > 0)
                typePriority += std::roundf(bonusRes * costs[RES_ENERGY] / energy);
            else
                typePriority += bonusRes;
        }

        if(costs[RES_MATERIAL1] > 0)
        {
            if(material > 0)
                typePriority += std::roundf(bonusRes * costs[RES_MATERIAL1] / material);
            else
                typePriority += bonusRes;
        }

        if(costs[RES_BLOBS] > 0)
        {
            if(blobs > 0)
                typePriority += std::roundf(bonusRes * costs[RES_BLOBS] / blobs);
            else
                typePriority += bonusRes;
        }

        if(costs[RES_DIAMONDS] > 0)
        {
            if(diamonds > 0)
                typePriority += std::roundf(bonusRes * costs[RES_DIAMONDS] / diamonds);
            else
                typePriority += bonusRes;
        }

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
    action->ObjSrc = gen;
    action->priority = bestPriority;
    action->unitType = types[bestInd];

    // push action to the queue
    AddNewAction(action);
}

void PlayerAI::AddActionUpgrade(GameObject * obj, const int weights[],
                                AIActionType type)
{
    const int level = obj->GetExperienceLevel();
    const int pointsToAssign = GameObject::UPGRADE_POINTS[level];

    const auto attributes = obj->GetAttributes();

    // define points to assign to attributes
    std::vector<int> changes(NUM_BASIC_ATTRIBUTES, 0);

    ObjAttId attToInc = OBJ_ATT_NULL;
    int maxAttScore = 0;

    for(int i = 0; i < pointsToAssign; ++i)
    {
        for(auto it : attributes)
        {
            const ObjAttId attId = it.first;

            // skip non-basic attributes
            if(attId < NUM_BASIC_ATTRIBUTES)
            {
                const int val = it.second + changes[attId];
                const int missingVal = MAX_STAT_IVAL - val;
                const int attScore = missingVal * weights[attId];

                if(attScore > maxAttScore)
                {
                    attToInc = attId;
                    maxAttScore = attScore;
                }
            }
        }

        if(attToInc != OBJ_ATT_NULL)
            ++changes[attToInc];
    }

    // create action
    auto action = new ActionAIUpgradeObject;
    action->type = type;
    action->ObjSrc = obj;
    action->priority = MAX_PRIORITY;
    action->attChanges = std::move(changes);

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
    const float bonusRelDist = energyUnit > 0 ? bonus * energyTot / energyUnit : bonus;
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

    const int energy = mPlayer->GetStat(Player::ENERGY).GetValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetValue();

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

    const int energy = mPlayer->GetStat(Player::ENERGY).GetValue();
    const int material = mPlayer->GetStat(Player::MATERIAL).GetValue();
    const int blobs = mPlayer->GetStat(Player::BLOBS).GetValue();
    const int diamonds = mPlayer->GetStat(Player::DIAMONDS).GetValue();

    float b = 0.f;

    if(costs[RES_ENERGY] > 0)
    {
        if(energy > 0)
            b += bonus * costs[RES_ENERGY] / energy;
        else
            b += bonus;
    }

    if(costs[RES_MATERIAL1] > 0)
    {
        if(material > 0)
            b += bonus * costs[RES_MATERIAL1] / material;
        else
            b += bonus;
    }

    if(costs[RES_BLOBS] > 0)
    {
        if(blobs > 0)
            b += bonus * costs[RES_BLOBS] / blobs;
        else
            b += bonus;
    }

    if(costs[RES_DIAMONDS] > 0)
    {
        if(diamonds > 0)
            b += bonus * costs[RES_DIAMONDS] / diamonds;
        else
            b += bonus;
    }

    return std::roundf(b);
}

int PlayerAI::GetPriorityBonusSameStructureCreated(GameObjectTypeId t, float bonus) const
{
    int numSame = 0;

    for(auto o : mOwnStructures)
        numSame += (o->GetObjectType() == t);

    return std::roundf(numSame * bonus);
}

int PlayerAI::GetNumStructuresInArea(const Cell2D & tl, const Cell2D & br,
                                     GameObjectTypeId type) const
{
    int num = 0;

    for(const auto s : mOwnStructures)
    {
        if(s->GetObjectType() != type)
            continue;

        num += s->GetRow1() >= tl.row && s->GetCol1() >= tl.col &&
               s->GetRow0() <= br.row && s->GetCol0() <= br.col;
    }

    return num;
}

void PlayerAI::PrintdActionDebug(const char * title, const ActionAI * a)
{
    std::cout << title;

    std::cout << " - ID: " << a->actId
              << " - TYPE: " << a->GetTypeStr() << " - PRIO: " << a->priority;

    if(AIA_UNIT_BUILD_STRUCTURE == a->type || AIA_UNIT_BUILD_TOWER == a->type)
    {
        auto ab = static_cast<const ActionAIBuildStructure *>(a);
        std::cout << " | STRUCT: " << ObjectData::TITLES.at(ab->structType);
    }
    else if(AIA_NEW_UNIT == a->type)
    {
        auto an = static_cast<const ActionAINewUnit *>(a);
        std::cout << " | UNIT: " << ObjectData::TITLES.at(an->unitType);
    }

    if(a->ObjSrc != nullptr)
    {
        auto obj = a->ObjSrc;

        // make sure obj is still valid (not destroyed)
        if(mGm->HasObject(obj))
        {
            std::cout << " | OBJ SRC - ID: " << obj->GetObjectId()
                      << " - FACT: " << obj->GetFaction()
                      << " - ENRG: " << obj->GetEnergy() << "/" << obj->GetMaxEnergy()
                      << " - HLTH: " << obj->GetHealth() << "/" << obj->GetMaxHealth();
        }
    }

    if(a->ObjDst != nullptr)
    {
        auto obj = a->ObjDst;

        // make sure obj is still valid (not destroyed)
        if(mGm->HasObject(obj))
        {
            std::cout << " | OBJ DST - ID: " << obj->GetObjectId()
                      << " - FACT: " << obj->GetFaction()
                      << " - ENRG: " << obj->GetEnergy() << "/" << obj->GetMaxEnergy()
                      << " - HLT: " << obj->GetHealth() << "/" << obj->GetMaxHealth();
        }
    }

    if(a->cellSrc.row != -1)
        std::cout << " | CELL SRC: " << a->cellSrc.row << "," << a->cellSrc.col;

    if(a->cellDst.row != -1)
        std::cout << " | CELL DST: " << a->cellDst.row << "," << a->cellDst.col;

    std::cout << " | TURN - ENRG: " << mPlayer->GetTurnEnergy() << "/" << mPlayer->GetTurnMaxEnergy()
              << std::endl;
}

} // namespace game
