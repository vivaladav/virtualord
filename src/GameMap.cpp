#include "GameMap.h"

#include "Cell2D.h"
#include "ControlMap.h"
#include "Game.h"
#include "GameConstants.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "AI/ConquerPath.h"
#include "AI/ObjectPath.h"
#include "AI/PlayerAI.h"
#include "AI/WallBuildPath.h"
#include "GameObjects/Barracks.h"
#include "GameObjects/Base.h"
#include "GameObjects/Blobs.h"
#include "GameObjects/BlobsGenerator.h"
#include "GameObjects/DefensiveTower.h"
#include "GameObjects/Diamonds.h"
#include "GameObjects/DiamondsGenerator.h"
#include "GameObjects/Hospital.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/PracticeTarget.h"
#include "GameObjects/RadarStation.h"
#include "GameObjects/RadarTower.h"
#include "GameObjects/ResearchCenter.h"
#include "GameObjects/ResourceGenerator.h"
#include "GameObjects/ResourceStorage.h"
#include "GameObjects/SceneObject.h"
#include "GameObjects/Temple.h"
#include "GameObjects/Trees.h"
#include "GameObjects/Unit.h"
#include "GameObjects/Wall.h"
#include "GameObjects/WallGate.h"
#include "Screens/ScreenGame.h"
#include "Widgets/MiniMap.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace game
{

// NOTE these will be replaced by dynamic values soon
constexpr int COST_CONQUEST_CELL = 1;
constexpr int COST_CONQUEST_RES_GEN = 4;

// ==================== PUBLIC METHODS ====================

GameMap::GameMap(Game * game, ScreenGame * sg, IsoMap * isoMap)
    : mControlMap(new ControlMap(isoMap->GetLayer(MapLayers::FACTION_INFLUENCE), game->GetLocalPlayer()))
    , mGame(game)
    , mScreenGame(sg)
    , mIsoMap(isoMap)
{
    SetSize(isoMap->GetNumRows(), isoMap->GetNumCols());

    mEnemiesKilled[FACTION_1] = 0;
    mEnemiesKilled[FACTION_2] = 0;
    mEnemiesKilled[FACTION_3] = 0;
    mEnemiesKilled[NO_FACTION] = 0;

    mCasualties[FACTION_1] = 0;
    mCasualties[FACTION_2] = 0;
    mCasualties[FACTION_3] = 0;
    mCasualties[NO_FACTION] = 0;
}

GameMap::~GameMap()
{
    delete mControlMap;

    for(GameObject * obj : mObjects)
        delete obj;

    for(CollectableGenerator * cg : mCollGen)
        delete cg;

    for(ObjectPath * op : mPaths)
        delete op;

    for(ConquerPath * cp : mConquerPaths)
        delete cp;
}

bool GameMap::IsObjectVisibleToLocalPlayer(const GameObject * obj) const
{
    Player * p = mGame->GetLocalPlayer();
    return p->IsObjectVisible(obj);
}

bool GameMap::IsCellVisibleToLocalPlayer(unsigned int ind) const
{
    Player * p = mGame->GetLocalPlayer();
    return p->IsCellVisible(ind);
}

bool GameMap::IsCellVisibleToLocalPlayer(unsigned int r, unsigned int c) const
{
    const unsigned int ind = (r * mCols) + c;

    Player * p = mGame->GetLocalPlayer();
    return p->IsCellVisible(ind);
}

bool GameMap::IsAnyCellVisibleToLocalPlayer(unsigned int rTL, unsigned int cTL,
                                            unsigned int rBR, unsigned int cBR) const
{
    Player * p = mGame->GetLocalPlayer();

    for(unsigned int r = rTL; r <= rBR; ++r)
    {
        const unsigned int indBase = (r * mCols);

        for(unsigned int c = cTL; c <= cBR; ++c)
        {
            const unsigned int ind = indBase + c;

            if(p->IsCellVisible(ind))
                return true;
        }
    }

    return false;
}

bool GameMap::IsCellWalkable(unsigned int r, unsigned int c) const
{
    const unsigned int ind = r * mCols + c;

    return mCells[ind].walkable;
}

bool GameMap::IsAnyNeighborCellWalkable(unsigned int r, unsigned int c) const
{
    const bool checkN = r > 0;
    const bool checkS = r < (mRows - 1);
    const bool checkW = c > 0;
    const bool checkE = c < (mCols - 1);

    return (checkN && IsCellWalkable(r - 1, c)) ||
           (checkS && IsCellWalkable(r + 1, c)) ||
           (checkW && IsCellWalkable(r, c - 1)) ||
           (checkE && IsCellWalkable(r, c + 1)) ||
           (checkN && checkW && IsCellWalkable(r - 1, c - 1)) ||
           (checkN && checkE && IsCellWalkable(r - 1, c + 1)) ||
           (checkS && checkW && IsCellWalkable(r + 1, c - 1)) ||
           (checkS && checkE && IsCellWalkable(r + 1, c + 1));
}

void GameMap::SetCellType(unsigned int ind, CellTypes type)
{
    mCells[ind].currType = type;
    mCells[ind].basicType = type;

    mIsoMap->SetCellType(ind, type);
}

void GameMap::UpdateCellType(unsigned int ind, const GameMapCell & cell)
{
    const auto type = static_cast<CellTypes>(DefineCellType(ind, cell));

    mCells[ind].currType = type;

    mIsoMap->SetCellType(ind, type);
}

bool GameMap::IsCellObjectVisited(unsigned int cellInd) const
{
    if(cellInd < mRows * mCols)
        return mCells[cellInd].objTop && mCells[cellInd].objTop->IsVisited();
    else
        return false;
}

void GameMap::SetSize(unsigned int rows, unsigned int cols)
{
    const unsigned int size = rows * cols;

    if(size == mCells.size())
        return ;

    mRows = rows;
    mCols = cols;

    mCells.resize(size);

    // set cell coordinates
    int index = 0;

    for(unsigned int r = 0; r < rows; ++r)
    {
        for(unsigned int c = 0; c < cols; ++c)
        {
            GameMapCell & cell = mCells[index++];

            cell.row = r;
            cell.col = c;
        }
    }

    // init players visibility map
    for(int i = 0; i < mGame->GetNumPlayers(); ++i)
        mGame->GetPlayerByIndex(i)->InitVisibility(mRows, mCols);

    // init control map
    mControlMap->SetSize(rows, cols);
}

void GameMap::CreateCollectableGenerator(unsigned int r, unsigned int c, ResourceType type)
{
    CollectableGenerator * gen = nullptr;

    if(RES_DIAMONDS == type)
        gen = new DiamondsGenerator(this);
    else if(RES_BLOBS == type)
        gen = new BlobsGenerator(this);
    else
        return ;

    gen->SetCell(r, c);

    mCollGen.emplace_back(gen);
}

void GameMap::ApplyLocalVisibility()
{
    Player * p = mGame->GetLocalPlayer();
    ApplyVisibility(p);
}

void GameMap::ApplyVisibility(Player * player)
{
    UpdateVisibility(player, false);
}

void GameMap::InitVisibility(Player * player)
{
    UpdateVisibility(player, true);
}

void GameMap::ApplyLocalVisibilityToObject(GameObject * go)
{
    Player * p = mGame->GetLocalPlayer();

    ApplyVisibilityToObject(p, go);
}

void GameMap::ApplyVisibilityToObject(Player * player, GameObject * go)
{
    const int rTL = go->GetRow1();
    const int cTL = go->GetCol1();
    const int rBR = go->GetRow0();
    const int cBR = go->GetCol0();

    IsoObject * obj = go->GetIsoObject();
    IsoLayer * layer = obj->GetLayer();

    bool visible = false;

    for(int r = rTL; r <= rBR; ++r)
    {
        const int indBase = r * mCols;

        for(int c = cTL; c <= cBR; ++c)
        {
            const int ind = indBase + c;

            if(player->IsCellVisible(ind))
            {
                visible = true;
                break;
            }
        }

        if(visible)
            break;
    }

    // update visibility if status changed
    if(visible != go->IsVisible())
    {
        const PlayerFaction objFaction = go->GetFaction();
        MiniMap * mm = mScreenGame->GetMiniMap();

        // hide objects if not visited or not a structure
        if(!visible && (!go->IsVisited() || !go->IsStatic()))
        {
            layer->SetObjectVisible(obj, false);

            if(objFaction != NO_FACTION && mm != nullptr)
                mm->RemoveElement(go->GetRow0(), go->GetCol0());
        }
        else
        {
            layer->SetObjectVisible(obj, true);

            if(mm != nullptr)
            {
                if(objFaction != NO_FACTION)
                {
                    const auto type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + objFaction);
                    mm->AddElement(go->GetRow0(), go->GetCol0(), go->GetRows(), go->GetCols(), type, objFaction);
                }
                else
                {
                    const auto type = go->CanBeConquered() ? MiniMap::MME_CONQUERABLE : MiniMap::MME_SCENE;
                    mm->AddElement(go->GetRow0(), go->GetCol0(), go->GetRows(), go->GetCols(),
                                   type, NO_FACTION);
                }
            }
        }

        go->SetVisible(visible);
    }

    // update visited flag independently to avoid problems on init
    if(visible)
        go->SetVisited();
}

void GameMap::InitObjectVisibility(Player * player, GameObject * gameObj)
{
    const int rTL = gameObj->GetRow1();
    const int cTL = gameObj->GetCol1();
    const int rBR = gameObj->GetRow0();
    const int cBR = gameObj->GetCol0();

    bool visible = false;

    for(int r = rTL; r <= rBR; ++r)
    {
        const int indBase = r * mCols;

        for(int c = cTL; c <= cBR; ++c)
        {
            const int ind = indBase + c;

            if(player->IsCellVisible(ind))
            {
                visible = true;
                break;
            }
        }

        if(visible)
            break;
    }

    IsoObject * isoObj = gameObj->GetIsoObject();
    IsoLayer * layer = isoObj->GetLayer();
    layer->SetObjectVisible(isoObj, visible);
    gameObj->SetVisible(visible);

    // add visible objects to minimap
    if(visible)
    {
        const PlayerFaction faction = gameObj->GetFaction();
        MiniMap * mm = mScreenGame->GetMiniMap();

        if(faction != NO_FACTION)
        {
            const MiniMap::MiniMapElemType type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);
            mm->AddElement(gameObj->GetRow0(), gameObj->GetCol0(), gameObj->GetRows(), gameObj->GetCols(), type, faction);
        }
        else
        {
            const MiniMap::MiniMapElemType type = gameObj->CanBeConquered() ? MiniMap::MME_CONQUERABLE : MiniMap::MME_SCENE;
            mm->AddElement(gameObj->GetRow0(), gameObj->GetCol0(), gameObj->GetRows(), gameObj->GetCols(),
                           type, NO_FACTION);
        }

        // also mark visible objects as visited
        gameObj->SetVisited();
    }
}

void GameMap::CreateObjectFromFile(unsigned int layerId, GameObjectTypeId type, GameObjectVariantId variant,
                                   unsigned int faction, unsigned int r0, unsigned int c0)
{
    const auto pf = static_cast<PlayerFaction>(faction);

    if(GameObject::TYPE_UNIT_SCOUT1 == type || GameObject::TYPE_UNIT_SOLDIER1 == type ||
       GameObject::TYPE_UNIT_SOLDIER2 == type || GameObject::TYPE_UNIT_WORKER1 == type)
    {
        Player * owner =  mGame->GetPlayerByFaction(pf);

        if(nullptr == owner)
            return ;

        const Cell2D dest(r0, c0);

        CreateUnit(type, nullptr, dest, owner);
    }
    else
        CreateObject(layerId, type, variant, pf, r0, c0, true);
}

GameObject * GameMap::CreateObject(unsigned int layerId, GameObjectTypeId type,
                                   GameObjectVariantId variant, PlayerFaction faction,
                                   unsigned int r0, unsigned int c0, bool instantAdd)
{
    // object origin is out of map
    if(r0 >= mRows || c0 >= mCols)
        return nullptr;

    ObjectToAdd o2a;

    const ObjectData & data = GetObjectData(type);
    const unsigned int rows = data.GetRows();
    const unsigned int cols = data.GetCols();

    o2a.r0 = r0;
    o2a.c0 = c0;
    o2a.r1 = 1 + r0 - rows;
    o2a.c1 = 1 + c0 - cols;
    o2a.layer = layerId;

    // full size is out of map
    if(o2a.r1 >= mRows || o2a.c1 >= mCols)
        return nullptr;

    const unsigned int ind0 = r0 * mCols + c0;

    const GameMapCell & gcell = mCells[ind0];

    // cell is already full
    if(gcell.objTop)
        return nullptr;

    // create game object
    o2a.obj = nullptr;
    o2a.owner = mGame->GetPlayerByFaction(faction);

    if(GameObject::TYPE_MOUNTAINS == type ||
       GameObject::TYPE_ROCKS == type)
        o2a.obj = new SceneObject(type, variant, rows, cols);
    else if(GameObject::TYPE_RES_GEN_ENERGY == type ||
       GameObject::TYPE_RES_GEN_MATERIAL == type ||
       GameObject::TYPE_RES_GEN_ENERGY_SOLAR == type ||
       GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT == type)
        o2a.obj = new ResourceGenerator(type, rows, cols);
    else if(GameObject::TYPE_RES_STORAGE_BLOBS == type ||
            GameObject::TYPE_RES_STORAGE_DIAMONDS == type ||
            GameObject::TYPE_RES_STORAGE_ENERGY == type ||
            GameObject::TYPE_RES_STORAGE_MATERIAL == type)
        o2a.obj = new ResourceStorage(type, rows, cols);
    else if(GameObject::TYPE_DIAMONDS == type)
        o2a.obj = new Diamonds;
    else if(GameObject::TYPE_BLOBS == type)
        o2a.obj  = new Blobs;
    else if(GameObject::TYPE_TREES == type)
        o2a.obj  = new Trees(variant);
    else if(GameObject::TYPE_RADAR_STATION == type)
        o2a.obj = new RadarStation;
    else if(GameObject::TYPE_RADAR_TOWER == type)
        o2a.obj = new RadarTower;
    else if(GameObject::TYPE_BARRACKS == type)
        o2a.obj = new Barracks;
    else if(GameObject::TYPE_RESEARCH_CENTER == type)
        o2a.obj = new ResearchCenter;
    else if(GameObject::TYPE_HOSPITAL == type)
        o2a.obj = new Hospital(data);
    else if(GameObject::TYPE_DEFENSIVE_TOWER == type)
        o2a.obj = new DefensiveTower(data);
    else if(GameObject::TYPE_WALL == type)
        o2a.obj = new Wall(variant);
    else if(GameObject::TYPE_WALL_GATE == type)
        o2a.obj = new WallGate(variant);
    else if(GameObject::TYPE_LOOTBOX == type)
        o2a.obj = new LootBox;
    else if(GameObject::TYPE_TEMPLE == type)
        o2a.obj = new Temple;
    else if(GameObject::TYPE_BASE == type || GameObject::TYPE_BASE_SPOT == type)
    {
        if(GameObject::TYPE_BASE_SPOT == type)
        {
            o2a.owner = mGame->GetPlayerByIndex(variant);
            faction = o2a.owner->GetFaction();
        }

        o2a.obj = new Base;

        // base cells update
        for(unsigned int r = o2a.r1; r <= r0; ++r)
        {
            const unsigned int indBase = r * mCols;

            for(unsigned int c = o2a.c1; c <= c0; ++c)
            {
                const unsigned int ind = indBase + c;
                mCells[ind].owner = o2a.owner;
                mCells[ind].linked = true;

                UpdateInfluencedCells(r, c);
            }
        }

        o2a.owner->SetBase(o2a.obj);
        o2a.owner->SumCells(rows * cols);
    }
    else if(GameObject::TYPE_PRACTICE_TARGET == type)
        o2a.obj = new PracticeTarget;
    // this should never happen
    else
    {
        std::cerr << "[ERR] GameMap::CreateObject - unknown obj type: " << type << std::endl;
        return nullptr;
    }

    // assign owner
    o2a.obj->SetFaction(faction);

    // set object properties
    o2a.obj->SetCell(&mCells[ind0]);

    // links to other objects
    o2a.obj->SetGameMap(this);
    o2a.obj->SetScreen(mScreenGame);

    // schedule object for map addition
    if(instantAdd)
        AddObjectToMap(o2a);
    else
        mObjectsToAdd.emplace_back(o2a);

    return o2a.obj;
}

bool GameMap::RemoveAndDestroyObject(GameObject * obj)
{
    auto it = std::find(mObjects.begin(), mObjects.end(), obj);

    // object not found
    if(mObjects.end() == it)
        return false;

    // remove from objects list and set and then destroy it
    mObjects.erase(it);
    mObjectsSet.erase(obj);

    DestroyObject(obj);

    return true;
}

bool GameMap::AreObjectsAdjacent(const GameObject * obj1, const GameObject * obj2) const
{
    // expand obj1 area by 1
    const int expRowTL = obj1->GetRow1() - 1;
    const int expColTL = obj1->GetCol1() - 1;
    const int expRowBR = obj1->GetRow0() + 1;
    const int expColBR = obj1->GetCol0() + 1;

    // check if expanded area and obj2 intersect
    return expRowTL <= obj2->GetRow0() &&
           obj2->GetRow1() <= expRowBR &&
           expColTL <= obj2->GetCol0() &&
           obj2->GetCol1() <= expColBR;
}

bool GameMap::AreCellsAdjacent(const Cell2D & cell1, const Cell2D & cell2) const
{
    const int distR = std::abs(cell1.row - cell2.row);
    const int distC = std::abs(cell1.col - cell2.col);

    const int maxDist = 1;

    return distR <= maxDist && distC <= maxDist;
}

bool GameMap::AreObjectsOrthoAdjacent(const GameObject * obj1, const GameObject * obj2) const
{
    const Cell2D TL1(obj1->GetRow1(), obj1->GetCol1());
    const Cell2D BR1(obj1->GetRow0(), obj1->GetCol0());
    const Cell2D TL2(obj2->GetRow1(), obj2->GetCol1());
    const Cell2D BR2(obj2->GetRow0(), obj2->GetCol0());

    const int maxD = 1;

    // obj1 above obj2
    if(BR1.row < TL2.row)
    {
        // obj1 left of obj2 OR obj1 right of obj2
        if(BR1.col < TL2.col || TL1.col > BR2.col)
            return false;
        // same cols
        else
            return (TL2.row - BR1.row) == maxD;
    }
    // obj1 below obj2
    else if(TL1.row > BR2.row)
    {
        // obj1 left of obj2 OR obj1 right of obj2
        if(BR1.col < TL2.col || TL1.col > BR2.col)
            return false;
        // same cols
        else
            return (TL1.row - BR2.row) == maxD;
    }
    // same rows
    else
    {
        // obj1 left of obj2
        if(BR1.col < TL2.col)
            return (TL2.col - BR1.col) == maxD;
        // obj1 right of obj2
        else if(TL1.col > BR2.col)
            return (TL1.col - BR2.col) == maxD;
        // same cols, overlapping objects? -> this should never happen
        else
            return false;
    }
}

bool GameMap::AreCellsOrthoAdjacent(const Cell2D & cell1, const Cell2D & cell2) const
{
    const int distR = std::abs(cell1.row - cell2.row);
    const int distC = std::abs(cell1.col - cell2.col);

    const int maxDist = 1;

    return (distR == 0 && distC == maxDist) || (distR == maxDist && distC == 0);
}

bool GameMap::CanConquerCell(Unit * unit, const Cell2D & cell, Player * player)
{
    const unsigned int r = static_cast<unsigned int>(cell.row);
    const unsigned int c = static_cast<unsigned int>(cell.col);

    // out of bounds
    if(!(r < mRows && c < mCols))
        return false;

    const int ind = r * mCols + c;
    GameMapCell & gcell = mCells[ind];

    // already changing
    if(gcell.changing)
        return false;

    // player already owns the cell
    if(gcell.owner == player)
        return false;

    // no unit
    if(nullptr == unit)
        return false;

    // not player's unit
    if(unit->GetFaction() != player->GetFaction())
        return false;

    // unit can't conquer
    if(!unit->CanConquer())
        return false;

    // unit doesn't have enough energy
    if(!unit->HasEnergyForActionStep(CONQUER_STRUCTURE))
        return false;

    // check if player has enough material - LAST CHECK
    return player->HasEnough(Player::Stat::MATERIAL, COST_CONQUEST_CELL);
}

void GameMap::StartConquerCell(const Cell2D & cell, Player * player)
{
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];

    // take player's energy
    player->SumResource(Player::Stat::MATERIAL, -COST_CONQUEST_CELL);

    // mark cell as changing
    gcell.changing = true;
}

void GameMap::ConquerCell(const Cell2D & cell, Player * player)
{
    // check if cell was of another faction
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];
    bool stolen = gcell.owner != nullptr && gcell.owner != player;

    // make cell empty
    ClearCell(gcell);

    // assign owner
    Player * prevOwner = gcell.owner;
    gcell.owner = player;

    // update player
    player->SumCells(1);

    // reset cell's changing flag
    gcell.changing = false;

    // propagate effects of conquest
    UpdateInfluencedCells(cell.row, cell.col);

    UpdateLinkedCells(player);

    // update visibility map
    AddPlayerCellVisibility(gcell, player);

    if(stolen)
        DelPlayerCellVisibility(gcell, prevOwner);

    ApplyLocalVisibility();
}

bool GameMap::ConquerCells(ConquerPath * path)
{
    const bool res = path->Start();

    if(res)
        mConquerPaths.emplace_back(path);

    return res;
}

bool GameMap::AbortCellConquest(GameObject * obj)
{
    for(auto path : mConquerPaths)
    {
        if(path->GetUnit() == obj)
        {
            path->Abort();
            return true;
        }
    }

    return false;
}

bool GameMap::CanBuildStructure(Unit * unit, const Cell2D & cell, Player * player, GameObjectTypeId st)
{
    const unsigned int r = static_cast<unsigned int>(cell.row);
    const unsigned int c = static_cast<unsigned int>(cell.col);

    const ObjectData & data = GetObjectData(st);

    // out of bounds
    if((data.GetRows() - 1) > r || (data.GetCols() - 1) > c || r >= mRows || c >= mCols)
        return false;

    // check costs
    const auto & costs = data.GetCosts();
    const bool costOk = player->HasEnough(Player::ENERGY, costs[RES_ENERGY]) &&
                        player->HasEnough(Player::MATERIAL, costs[RES_MATERIAL1]) &&
                        player->HasEnough(Player::DIAMONDS, costs[RES_DIAMONDS]) &&
                        player->HasEnough(Player::BLOBS, costs[RES_BLOBS]);

    if(!costOk)
        return false;

    // check unit's energy
    if(!unit->HasEnergyForActionStep(BUILD_STRUCTURE))
        return false;

    // check cells
    const unsigned int r0 = 1 + cell.row - data.GetRows();
    const unsigned int c0 = 1 + cell.col - data.GetCols();

    for(int r = r0; r <= cell.row; ++r)
    {
        const int idx0 = r * mCols;

        for(int c = c0; c <= cell.col; ++c)
        {
            const int idx = idx0 + c;
            const GameMapCell & gcell = mCells[idx];

            // already changing or occupied
            if(gcell.changing || !gcell.walkable)
                return false;
        }
    }

    return true;
}

void GameMap::StartBuildStructure(const Cell2D & cell, Player * player, GameObjectTypeId st)
{
    const ObjectData & data = GetObjectData(st);

    // mark cell as changing
    const unsigned int r0 = 1 + cell.row - data.GetRows();
    const unsigned int c0 = 1 + cell.col - data.GetCols();

    for(int r = r0; r <= cell.row; ++r)
    {
        const int idx0 = r * mCols;

        for(int c = c0; c <= cell.col; ++c)
        {
            const int idx = idx0 + c;
            GameMapCell & gcell = mCells[idx];
            gcell.changing = true;
        }
    }

    // make player pay
    const auto & costs = data.GetCosts();

    if(costs[RES_ENERGY] > 0)
        player->SumResource(Player::ENERGY, -costs[RES_ENERGY]);
    if(costs[RES_MATERIAL1] > 0)
        player->SumResource(Player::MATERIAL, -costs[RES_MATERIAL1]);
    if(costs[RES_DIAMONDS] > 0)
        player->SumResource(Player::DIAMONDS, -costs[RES_DIAMONDS]);
    if(costs[RES_BLOBS] > 0)
        player->SumResource(Player::BLOBS, -costs[RES_BLOBS]);
}

void GameMap::BuildStructure(const Cell2D & cell, Player * player, GameObjectTypeId st)
{
    // check if cell was of another faction
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];
    bool stolen = gcell.owner != nullptr && gcell.owner != player;

    // make cell empty
    ClearCell(gcell);

    // assign owner
    Player * prevOwner = gcell.owner;
    gcell.owner = player;

    // update player
    player->SumCells(1);

    // reset cell's changing flag
    gcell.changing = false;

    GameObject * obj = CreateObject(OBJECTS2, st, 0, player->GetFaction(), cell.row, cell.col, true);

    // propagate effects of conquest
    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        const int ind0 = r * mCols;

        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            const int ind = ind0 + c;
            UpdateInfluencedCells(r, c);
        }
    }

    UpdateLinkedCells(player);

    // update surrounding walls if building an object that can connect
    if(obj->GetObjectType() == GameObject::TYPE_DEFENSIVE_TOWER || obj->GetObjectType() == GameObject::TYPE_WALL_GATE)
        UpdateWalls(cell);

    // update visibility map
    AddPlayerCellVisibility(gcell, player);

    if(stolen)
        DelPlayerCellVisibility(gcell, prevOwner);

    ApplyLocalVisibility();
}

bool GameMap::CanBuildWall(const Cell2D & cell, Player * player, unsigned int level)
{
    const unsigned int r = static_cast<unsigned int>(cell.row);
    const unsigned int c = static_cast<unsigned int>(cell.col);

    // out of bounds
    if(!(r < mRows && c < mCols))
        return false;

    const int ind = r * mCols + c;
    GameMapCell & gcell = mCells[ind];

    // already changing
    if(gcell.changing)
        return false;

    // cell is full
    if(!gcell.walkable)
        return false;

    // check if player has enough energy and material
    const int costMat = Wall::GetCostMaterial(level);
    const int costEne = Wall::GetCostEnergy(level);

    return player->HasEnough(Player::Stat::MATERIAL, costMat)  &&
           player->HasEnough(Player::Stat::ENERGY, costEne);
}

void GameMap::StartBuildWall(const Cell2D & cell, Player * player, unsigned int level)
{
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];

    // take player's material
    const int costEne = Wall::GetCostEnergy(level);
    player->SumResource(Player::Stat::ENERGY, -costEne);

    const int costMat = Wall::GetCostMaterial(level);
    player->SumResource(Player::Stat::MATERIAL, -costMat);

    // mark cell as changing
    gcell.changing = true;
}

void GameMap::BuildWall(const Cell2D & cell, Player * player, GameObjectTypeId planned)
{
    // check if cell was of another faction
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];
    bool stolen = gcell.owner != nullptr && gcell.owner != player;

    // make cell empty
    ClearCell(gcell);

    // assign owner
    Player * prevOwner = gcell.owner;
    gcell.owner = player;

    // update player
    player->SumCells(1);

    // reset cell's changing flag
    gcell.changing = false;
    // propagate effects of conquest
    UpdateInfluencedCells(cell.row, cell.col);

    UpdateLinkedCells(player);

    // add object wall
    CreateObject(OBJECTS2, GameObject::TYPE_WALL, planned, player->GetFaction(), cell.row, cell.col, true);

    // update minimap
    if(IsCellVisibleToLocalPlayer(cell.row, cell.col))
    {
        const ObjectData & data = GetObjectData(GameObject::TYPE_WALL);

        const PlayerFaction faction = player->GetFaction();
        const MiniMap::MiniMapElemType type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);
        MiniMap * mm = mScreenGame->GetMiniMap();
        mm->AddElement(cell.row, cell.col, data.GetRows(), data.GetCols(), type, faction);
    }

    // update this wall type and the ones surrounding it
    UpdateWalls(cell);

    // update visibility map
    AddPlayerCellVisibility(gcell, player);

    if(stolen)
        DelPlayerCellVisibility(gcell, prevOwner);

    ApplyLocalVisibility();
}

bool GameMap::BuildWalls(WallBuildPath * path)
{
    const bool res = path->Start();

    if(res)
        mWallBuildPaths.emplace_back(path);

    return res;
}

bool GameMap::AbortBuildWalls(GameObject * obj)
{
    for(auto path : mWallBuildPaths)
    {
        if(path->GetUnit() == obj)
        {
            path->Abort();
            return true;
        }
    }

    return false;
}

bool GameMap::CanConquerStructure(Unit * unit, const Cell2D & end, Player * player)
{
    const unsigned int r1 = static_cast<unsigned int>(end.row);
    const unsigned int c1 = static_cast<unsigned int>(end.col);

    // no unit
    if(nullptr == unit)
        return false;

    // not player's unit
    if(unit->GetFaction() != player->GetFaction())
        return false;

    // unit can't conquer
    if(!unit->CanConquer())
        return false;

    // unit doesn't have enough energy
    if(!unit->HasEnergyForActionStep(CONQUER_STRUCTURE))
        return false;

    const int ind1 = r1 * mCols + c1;
    GameMapCell & gcell1 = mCells[ind1];

    // end is empty
    if(nullptr == gcell1.objTop)
        return false;

    // target object can't be conquered
    if(!gcell1.objTop->CanBeConquered())
        return false;

    // player already owns the structure
    if(gcell1.objTop->GetFaction() == player->GetFaction())
        return false;

    // TEMP - no conquest while another is in progress
    if(gcell1.changing)
        return false;

    return true;
}

void GameMap::StartConquerStructure(const Cell2D & end, Player * player)
{
    // take player's energy
    player->SumResource(Player::Stat::ENERGY, -COST_CONQUEST_RES_GEN);

    // mark object cells as changing
    const int ind1 = end.row * mCols + end.col;
    GameObject * obj = mCells[ind1].objTop;

    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        const unsigned int indBase = r * mCols;

        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            const unsigned int ind = indBase + c;
            mCells[ind].changing = true;
        }
    }
}

void GameMap::AbortConquerStructure(GameObject * target)
{
    // mark object cells as not changing
    for(int r = target->GetRow1(); r <= target->GetRow0(); ++r)
    {
        const unsigned int indBase = r * mCols;

        for(int c = target->GetCol1(); c <= target->GetCol0(); ++c)
        {
            const unsigned int ind = indBase + c;
            mCells[ind].changing = false;
        }
    }
}

void GameMap::ConquerStructure(const Cell2D & end, Player * player)
{
    const int ind = end.row * mCols + end.col;
    GameMapCell & gcell1 = mCells[ind];

    GameObject * obj = gcell1.objTop;

    // assign owner to cells
    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        const unsigned int indBase = r * mCols;

        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            const unsigned int ind = indBase + c;
            mCells[ind].owner = player;
            mCells[ind].changing = false;

            UpdateInfluencedCells(r, c);
        }
    }

    // assign owner to object
    Player * prevOwner = mGame->GetPlayerByFaction(obj->GetFaction());
    obj->SetFaction(player->GetFaction());

    // update player
    player->SumCells(1);

    // update tracking of structures and resource generators
    if(obj->IsStructure())
    {
        auto st = static_cast<Structure *>(obj);

        player->AddStructure(st);

        if(prevOwner)
            prevOwner->RemoveStructure(st);

        if(obj->GetObjectCategory() == GameObject::CAT_RES_GENERATOR)
        {
            auto rg = static_cast<ResourceGenerator *>(obj);

            player->AddResourceGenerator(rg);

            if(prevOwner)
                prevOwner->RemoveResourceGenerator(rg);
        }
    }

    // update map
    UpdateLinkedCells(player);

    // update visibility
    if(prevOwner && prevOwner != player)
        DelPlayerObjVisibility(obj, prevOwner);

    AddPlayerObjVisibility(obj, player);

    ApplyLocalVisibility();
}


void GameMap::HandleTempleExplorationOutcome(unsigned int outcome, Player * p, Temple * temple)
{
    const StatValue & money = p->GetStat(Player::Stat::MONEY);
    const StatValue & energy = p->GetStat(Player::Stat::ENERGY);
    const StatValue & material = p->GetStat(Player::Stat::MATERIAL);
    const StatValue & blobs = p->GetStat(Player::Stat::BLOBS);
    const StatValue & diamonds = p->GetStat(Player::Stat::DIAMONDS);
    const PlayerFaction faction = p->GetFaction();

    // -- REWARDS --
    if(outcome >= Temple::FIRST_EXP_REW && outcome <= Temple::LAST_EXP_REW)
    {
        const Cell2D cell0(temple->GetRow0(), temple->GetCol0());
        ConquerStructure(cell0, p);

        switch(outcome)
        {
            // INCREASE EXISTING RESOURCES
            case Temple::EXP_REW_MULT10_MONEY:
            {
                const int mult = 10;
                p->SetResource(Player::Stat::MONEY, money.GetIntValue() * mult);
            }
            break;

            case Temple::EXP_REW_MAX_RES_ENE_MAT:
            {
                p->SetResource(Player::Stat::ENERGY, energy.GetIntMax());
                p->SetResource(Player::Stat::MATERIAL, material.GetIntMax());
            }
            break;

            case Temple::EXP_REW_MAX_RES_BLO_DIA:
            {
                p->SetResource(Player::Stat::BLOBS, blobs.GetIntMax());
                p->SetResource(Player::Stat::DIAMONDS, diamonds.GetIntMax());
            }
            break;

            case Temple::EXP_REW_MAX_RESOURCES:
            {
                p->SetResource(Player::Stat::ENERGY, energy.GetIntMax());
                p->SetResource(Player::Stat::MATERIAL, material.GetIntMax());
                p->SetResource(Player::Stat::BLOBS, blobs.GetIntMax());
                p->SetResource(Player::Stat::DIAMONDS, diamonds.GetIntMax());
            }
            break;

            // INCREASE PRODUCTION
            case Temple::EXP_REW_INC_ENERGY_PRODUCTION:
            {
                const float mult = 2.f;

                for(GameObject * o : mObjects)
                {
                    if(o->GetFaction() == faction &&
                       o->GetObjectType() == GameObject::TYPE_RES_GEN_ENERGY)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

            case Temple::EXP_REW_INC_MATERIAL_PRODUCTION:
            {
                const float mult = 2.f;

                for(GameObject * o : mObjects)
                {
                    if(o->GetFaction() == faction &&
                       o->GetObjectType() == GameObject::TYPE_RES_GEN_MATERIAL)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

            // MAXIMIZE COLLECTIBLES
            case Temple::EXP_REW_MAX_BLOBS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == GameObject::TYPE_BLOBS)
                        static_cast<Blobs *>(o)->MaximizeUnits();
                }
            }
            break;

            case Temple::EXP_REW_MAX_DIAMONDS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == GameObject::TYPE_DIAMONDS)
                        static_cast<Diamonds *>(o)->MaximizeUnits();
                }
            }
            break;

            // unexpected
            default:
            break;
        }
    }
    // -- PUNISHMENTS --
    else
    {
        switch(outcome)
        {
            // DECREASE EXISTING RESOURCES
            case Temple::EXP_PUN_ZERO_MONEY:
            {
                p->SetResource(Player::Stat::MONEY, 0);
            }
            break;

            case Temple::EXP_PUN_ZERO_RES_ENE_MAT:
            {
                p->SetResource(Player::Stat::ENERGY, 0);
                p->SetResource(Player::Stat::MATERIAL, 0);
            }
            break;

            case Temple::EXP_PUN_ZERO_RES_BLO_DIA:
            {
                p->SetResource(Player::Stat::BLOBS, 0);
                p->SetResource(Player::Stat::DIAMONDS, 0);
            }
            break;

            case Temple::EXP_PUN_ZERO_RESOURCES:
            {
                p->SetResource(Player::Stat::ENERGY, 0);
                p->SetResource(Player::Stat::MATERIAL, 0);
                p->SetResource(Player::Stat::BLOBS, 0);
                p->SetResource(Player::Stat::DIAMONDS, 0);
            }
            break;

                // DECREASE PRODUCTION
            case Temple::EXP_PUN_DEC_ENERGY_PRODUCTION:
            {
                const float mult = 0.5f;

                for(GameObject * o : mObjects)
                {
                    if(o->GetFaction() == faction &&
                       o->GetObjectType() == GameObject::TYPE_RES_GEN_ENERGY)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

            case Temple::EXP_PUN_DEC_MATERIAL_PRODUCTION:
            {
                const float mult = 0.5f;

                for(GameObject * o : mObjects)
                {
                    if(o->GetFaction() == faction &&
                       o->GetObjectType() == GameObject::TYPE_RES_GEN_MATERIAL)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

                // MINIMIZE COLLECTIBLES
            case Temple::EXP_PUN_MIN_BLOBS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == GameObject::TYPE_BLOBS)
                        static_cast<Blobs *>(o)->MinimizeUnits();
                }
            }
            break;

            case Temple::EXP_PUN_MIN_DIAMONDS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == GameObject::TYPE_DIAMONDS)
                        static_cast<Diamonds *>(o)->MinimizeUnits();
                }
            }
            break;

            // unexpected
            default:
            break;
        }
    }
}

bool GameMap::CanCreateUnit(GameObjectTypeId ut, GameObject * gen, Player * player)
{
    // generator is not owned by the player
    if(gen->GetFaction() != player->GetFaction())
        return false;

    // already has enough units
    if(player->GetNumUnits() == player->GetMaxUnits())
        return false;

    // check if generator is valid
    const GameObjectTypeId genType = gen->GetObjectType();

    if(genType != GameObject::TYPE_BASE && genType != GameObject::TYPE_BARRACKS &&
       genType != GameObject::TYPE_HOSPITAL)
        return false;

    // generator is already busy
    if(gen->IsBusy())
       return false;

    // check if player has enough resources
    const auto & costs = GetObjectData(ut).GetCosts();

    if(!player->HasEnough(Player::Stat::ENERGY, costs[RES_ENERGY]) ||
       !player->HasEnough(Player::Stat::MATERIAL, costs[RES_MATERIAL1]) ||
       !player->HasEnough(Player::Stat::DIAMONDS, costs[RES_DIAMONDS]) ||
       !player->HasEnough(Player::Stat::BLOBS, costs[RES_BLOBS]))
        return false;

    // check if there's at least 1 free cell where to place the new unit
    // NOTE this must be the last test or the code below needs to be changed
    const int r1 = gen->GetRow1() > 0 ? gen->GetRow1() - 1 : 0;
    const int c1 = gen->GetCol1() > 0 ? gen->GetCol1() - 1 : 0;
    const int r0 = gen->GetRow0() < static_cast<int>(mRows - 1) ? gen->GetRow0() + 1 : mRows - 1;
    const int c0 = gen->GetCol0() < static_cast<int>(mCols - 1) ? gen->GetCol0() + 1 : mCols - 1;

    const int indGenTop = r1 * mCols;
    const int indGenBottom = r0 * mCols;

    // NOTE for simplicity corner cells are overlapping and sometimes checked twice.
    // This can be optimized, but it's probably not worth it for now.

    // check right (top to bottom)
    for(int r = r1; r <= r0; ++r)
    {
        if(mCells[r * mCols + c0].walkable)
            return true;
    }

    // check top (left to right)
    for(int c = c1; c <= c0; ++c)
    {
        if(mCells[indGenTop + c].walkable)
            return true;
    }

    // check left (bottom to top)
    for(int r = r0; r >= r1; --r)
    {
        if(mCells[r * mCols + c1].walkable)
            return true;
    }

    // check bottom (left to right)
    for(int c = c1; c <= c0; ++c)
    {
        if(mCells[indGenBottom + c].walkable)
            return true;
    }

    // free cell test failed
    return false;
}

Cell2D GameMap::GetNewUnitDestination(GameObject * gen)
{
    const int r1 = gen->GetRow1() > 0 ? gen->GetRow1() - 1 : 0;
    const int c1 = gen->GetCol1() > 0 ? gen->GetCol1() - 1 : 0;
    const int r0 = gen->GetRow0() < static_cast<int>(mRows - 1) ? gen->GetRow0() + 1 : mRows - 1;
    const int c0 = gen->GetCol0() < static_cast<int>(mCols - 1) ? gen->GetCol0() + 1 : mCols - 1;

    const int indBaseTop = r1 * mCols;
    const int indBaseBottom = r0 * mCols;

    const int halfRows = mRows / 2;
    const int halfCols = mCols / 2;

    // NOTE for simplicity corner cells are overlapping and sometimes checked twice.
    // This can be optimized, but it's probably not worth it for now.


    // BOTTOM of the map
    if(r0 > halfRows)
    {
        // LEFT of the map
        if(c0 < halfCols)
        {
            // check right (top to bottom)
            for(int r = r1; r <= r0; ++r)
            {
                if(mCells[r * mCols + c0].walkable)
                    return Cell2D(r, c0);
            }

            // check top (right to left)
            for(int c = c0; c >= c1; --c)
            {
                if(mCells[indBaseTop + c].walkable)
                    return Cell2D(r1, c);
            }

            // check bottom (right to left)
            for(int c = c0; c >= c1; --c)
            {
                if(mCells[indBaseBottom + c].walkable)
                    return Cell2D(r0, c);
            }

            // check left (top to bottom)
            for(int r = r1; r <= r0; ++r)
            {
                if(mCells[r * mCols + c1].walkable)
                    return Cell2D(r, c1);
            }
        }
        // RIGHT of the map
        else
        {
            // check left (top to bottom)
            for(int r = r1; r <= r0; ++r)
            {
                if(mCells[r * mCols + c1].walkable)
                    return Cell2D(r, c1);
            }

            // check top (left to right)
            for(int c = c1; c <= c0; ++c)
            {
                if(mCells[indBaseTop + c].walkable)
                    return Cell2D(r1, c);
            }

            // check right (top to bottom)
            for(int r = r1; r <= r0; ++r)
            {
                if(mCells[r * mCols + c0].walkable)
                    return Cell2D(r, c0);
            }

            // check bottom (left to right)
            for(int c = c1; c <= c0; ++c)
            {
                if(mCells[indBaseBottom + c].walkable)
                    return Cell2D(r0, c);
            }
        }
    }
    // TOP of the map
    else
    {
        // LEFT of the map
        if(c0 < halfCols)
        {
            // check right (bottom to top)
            for(int r = r0; r >= r1; --r)
            {
                if(mCells[r * mCols + c0].walkable)
                    return Cell2D(r, c0);
            }

            // check bottom (right to left)
            for(int c = c0; c >= c1; --c)
            {
                if(mCells[indBaseBottom + c].walkable)
                    return Cell2D(r0, c);
            }

            // check top (right to left)
            for(int c = c0; c >= c1; --c)
            {
                if(mCells[indBaseTop + c].walkable)
                    return Cell2D(r1, c);
            }

            // check left (bottom to top)
            for(int r = r0; r >= r1; --r)
            {
                if(mCells[r * mCols + c1].walkable)
                    return Cell2D(r, c1);
            }
        }
        // RIGHT of the map
        else
        {
            // check left (bottom to top)
            for(int r = r0; r >= r1; --r)
            {
                if(mCells[r * mCols + c1].walkable)
                    return Cell2D(r, c1);
            }

            // check bottom (left to right)
            for(int c = c1; c <= c0; ++c)
            {
                if(mCells[indBaseBottom + c].walkable)
                    return Cell2D(r0, c);
            }

            // check top (left to right)
            for(int c = c1; c <= c0; ++c)
            {
                if(mCells[indBaseTop + c].walkable)
                    return Cell2D(r1, c);
            }

            // check right (bottom to top)
            for(int r = r0; r >= r1; --r)
            {
                if(mCells[r * mCols + c0].walkable)
                    return Cell2D(r, c0);
            }
        }
    }

    // failed to find a spot
    return Cell2D(-1, -1);
}

void GameMap::StartCreateUnit(GameObjectTypeId ut, GameObject * gen, const Cell2D & dest, Player * player)
{
    const int ind = dest.row * mCols + dest.col;
    GameMapCell & gcell = mCells[ind];

    // make player pay
    const auto & costs = GetObjectData(ut).GetCosts();

    player->SumResource(Player::Stat::ENERGY, -costs[RES_ENERGY]);
    player->SumResource(Player::Stat::MATERIAL, -costs[RES_MATERIAL1]);
    player->SumResource(Player::Stat::DIAMONDS, -costs[RES_DIAMONDS]);
    player->SumResource(Player::Stat::BLOBS, -costs[RES_BLOBS]);

    // mark cell as changing
    gcell.changing = true;
}

void GameMap::CreateUnit(GameObjectTypeId ut, GameObject * gen, const Cell2D & dest, Player * player)
{
    const unsigned int r = static_cast<unsigned int>(dest.row);
    const unsigned int c = static_cast<unsigned int>(dest.col);

    const int ind = r * mCols + c;
    GameMapCell & gcell = mCells[ind];

    const PlayerFaction faction = player->GetFaction();
    const ObjectData & data = GetObjectData(ut);

    Unit * unit = new Unit(data);
    unit->SetFaction(faction);
    unit->SetCell(&mCells[ind]);

    // links to other objects
    unit->SetGameMap(this);
    unit->SetScreen(mScreenGame);

    // update cell
    gcell.objTop = unit;
    gcell.walkable = false;
    gcell.changing = false;

    mIsoMap->GetLayer(OBJECTS2)->AddObject(unit->GetIsoObject(), r, c);

    // store unit in map list and in registry
    mObjects.push_back(unit);
    mObjectsSet.insert(unit);

    // update player
    player->AddUnit(unit);

    // update visibility map
    AddPlayerObjVisibility(unit, player);

    ApplyLocalVisibility();
}

bool GameMap::CanUnitMove(const Cell2D & start, const Cell2D & end, Player * player) const
{
    const unsigned int r0 = static_cast<unsigned int>(start.row);
    const unsigned int c0 = static_cast<unsigned int>(start.col);

    // start out of bounds
    if(!(r0 < mRows && c0 < mCols))
        return false;

    const unsigned int r1 = static_cast<unsigned int>(end.row);
    const unsigned int c1 = static_cast<unsigned int>(end.col);

    // end out of bounds
    if(!(r1 < mRows && c1 < mCols))
        return false;

    const int diffR = abs(end.row - start.row);
    const int diffC = abs(end.col - start.col);

    // end too far - units can only move to next cell
    if(diffR > 1 || diffC > 1)
        return false;

    const int ind0 = r0 * mCols + c0;
    const GameMapCell & gcell0 = mCells[ind0];
    const Unit * unit0 = gcell0.GetUnit();

    // start has no units
    if(nullptr == unit0)
        return false;

    // trying to move an enemy unit
    if(unit0->GetFaction() != player->GetFaction())
        return false;

    const int ind1 = r1 * mCols + c1;
    const GameMapCell & gcell1 = mCells[ind1];

    // end not a walkable cell
    if(!gcell1.walkable)
        return false;

    const Unit * unit1 = gcell1.GetUnit();

    // fail if destination is full or has different level units
    if(unit1 != nullptr)
        return false;

    // all good
    return true;
}

bool GameMap::MoveUnit(ObjectPath * path)
{
    GameObject * obj = path->GetObject();

    const int ind = obj->GetRow0() * mCols + obj->GetCol0();

    // object is not in its cell !?
    if(mCells[ind].objTop != obj)
        return false;

    // start path
    const bool started = path->Start();

    if(started)
        mPaths.emplace_back(path);

    return started;
}

bool GameMap::AbortMove(GameObject * obj)
{
    for(auto path : mPaths)
    {
        if(path->GetObject() == obj)
        {
            path->Abort();
            return true;
        }
    }

    return false;
}

Cell2D GameMap::GetCloseMoveTarget(const Cell2D & start, const Cell2D & end) const
{
    // get all walkable cells around end
    const int rowTL = end.row - 1 > 0 ? end.row - 1 : 0;
    const int colTL = end.col - 1 > 0 ? end.col - 1 : 0;
    const int rowBR = end.row + 1 < static_cast<int>(mRows - 1) ? end.row + 1 : mRows - 1;
    const int colBR = end.col + 1 < static_cast<int>(mCols - 1) ? end.col + 1 : mCols - 1;

    std::vector<Cell2D> walkalbes;
    const int maxWalkables = 8;
    walkalbes.reserve(maxWalkables);

    for(int r = rowTL; r <= rowBR; ++r)
    {
        const int indBase = r * mCols;

        for(int c = colTL; c <= colBR; ++c)
        {
            const int ind = indBase + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    return GetClosestCell(start, walkalbes);
}

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const
{
    const Cell2D tl(target->GetRow1(), target->GetCol1());
    const Cell2D br(target->GetRow0(), target->GetCol0());

    return GetAdjacentMoveTarget(start, tl, br);
}

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & target) const
{
    return GetAdjacentMoveTarget(start, target, target);
}

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const
{
    // get all walkable cells around target
    const int tRows = targetBR.row - targetTL.row + 1;
    const int tCols = targetBR.col - targetTL.col + 1;

    const int rowTL = targetTL.row - 1 > 0 ? targetTL.row - 1 : 0;
    const int colTL = targetTL.col - 1 > 0 ? targetTL.col - 1 : 0;
    const int rowBR = targetBR.row + 1 < static_cast<int>(mRows - 1) ? targetBR.row + 1 : mRows - 1;
    const int colBR = targetBR.col + 1 < static_cast<int>(mCols - 1) ? targetBR.col + 1 : mCols - 1;

    std::vector<Cell2D> walkalbes;
    const int maxWalkables = (tCols + 2 + tRows) * 2;
    walkalbes.reserve(maxWalkables);

    for(int r = rowTL; r <= rowBR; ++r)
    {
        const int indBase = r * mCols;

        for(int c = colTL; c <= colBR; ++c)
        {
            const int ind = indBase + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    return GetClosestCell(start, walkalbes);
}

Cell2D GameMap::GetOrthoAdjacentMoveTarget(const Cell2D & start, const GameObject * target) const
{
    const Cell2D tl(target->GetRow1(), target->GetCol1());
    const Cell2D br(target->GetRow0(), target->GetCol0());

    return GetOrthoAdjacentMoveTarget(start, tl, br);
}

Cell2D GameMap::GetOrthoAdjacentMoveTarget(const Cell2D & start, const Cell2D & target) const
{
    return GetOrthoAdjacentMoveTarget(start, target, target);
}

Cell2D GameMap::GetOrthoAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL, const Cell2D & targetBR) const
{
    const int tRows = targetBR.row - targetTL.row + 1;
    const int tCols = targetBR.col - targetTL.col + 1;

    std::vector<Cell2D> walkalbes;
    const int maxWalkables = (tRows + tCols) * 2;
    walkalbes.reserve(maxWalkables);

    // TOP cells
    if(targetTL.row > 0)
    {
        const int r = targetTL.row - 1;
        const int indBase = r * mCols;

        for(int c = targetTL.col; c <= targetBR.col; ++c)
        {
            const int ind = indBase + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // BOTTOM cells
    if(targetBR.row < (static_cast<int>(mRows) - 1))
    {
        const int r = targetBR.row + 1;
        const int indBase = r * mCols;

        for(int c = targetTL.col; c <= targetBR.col; ++c)
        {
            const int ind = indBase + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // LEFT cells
    if(targetTL.col > 0)
    {
        const int c = targetTL.col - 1;

        for(int r = targetTL.row; r <= targetBR.row; ++r)
        {
            const int ind = (r * mCols) + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // RIGHT cells
    if(targetBR.col < (static_cast<int>(mCols) - 1))
    {
        const int c = targetBR.col + 1;

        for(int r = targetTL.row; r <= targetBR.row; ++r)
        {
            const int ind = (r * mCols) + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    return GetClosestCell(start, walkalbes);
}

bool GameMap::FindAttackPosition(const Unit * u, const GameObject * target, Cell2D & pos)
{
    const int dist = ceilf(u->GetRangeAttack() * 0.5f);

    if(FindAttackPosition(u, target, dist, pos))
        return true;

    // fallback
    return FindAttackPosition(u, target, dist + 1, pos);
}

bool GameMap::FindAttackPosition(const Unit * u, const GameObject * target, int dist, Cell2D & pos)
{
    const int unitR0 = u->GetRow0();
    const int unitC0 = u->GetCol0();
    const Cell2D unitCell(unitR0, unitC0);
    const int targetR0 = target->GetRow0();
    const int targetC0 = target->GetCol0();

    // FIND CLOSEST VERTEX OF TARGET
    int targetR = targetR0;
    int targetC = targetC0;

    if(target->GetRows() > 1 || target->GetCols() > 1)
    {
        const int targetR1 = target->GetRow1();
        const int targetC1 = target->GetCol1();

        const int VERTS = 4;
        const Cell2D targetV[VERTS] =
        {
            {targetR0, targetC0},
            {targetR0, targetC1},
            {targetR1, targetC0},
            {targetR1, targetC1}
        };

        int bestInd = 0;
        int minDist = mRows + mCols;

        for(int i = 0; i < VERTS; ++i)
        {
            const int distV = ApproxDistance(unitCell, targetV[i]);

            if(distV < minDist)
            {
                bestInd = i;
                minDist = distV;
            }
        }

        targetR = targetV[bestInd].row;
        targetC = targetV[bestInd].col;
    }

    // FIND ATTACK POSITION
    const int tlR = (targetR - dist) >= 0 ? (targetR - dist) : 0;
    const int tlC = (targetC - dist) >= 0 ? (targetC - dist) : 0;
    const int brR = (targetR + dist) < (mRows - 1) ? targetR + dist : (mRows - 1);
    const int brC = (targetC + dist) < (mCols - 1) ? targetC + dist : (mCols - 1);

    int minDist = mRows + mCols;

    // TOP
    {
        const int r = tlR;

        for(int c = tlC; c <= brC; ++c)
        {
            const Cell2D dest(r, c);
            const int dist = ApproxDistance(unitCell, dest);

            if(dist < minDist)
            {
                pos = dest;
                minDist = dist;
            }
        }
    }

    // BOTTOM
    {
        const int r = brR;

        for(int c = tlC; c <= brC; ++c)
        {
            const Cell2D dest(r, c);
            const int dist = ApproxDistance(unitCell, dest);

            if(dist < minDist)
            {
                pos = dest;
                minDist = dist;
            }
        }
    }

    // LEFT
    {
        const int c = tlC;

        for(int r = tlR + 1; r < brR; ++r)
        {
            const Cell2D dest(r, c);
            const int dist = ApproxDistance(unitCell, dest);

            if(dist < minDist)
            {
                pos = dest;
                minDist = dist;
            }
        }
    }

    // RIGHT
    {
        const int c = brC;

        for(int r = tlR + 1; r < brR; ++r)
        {
            const Cell2D dest(r, c);
            const int dist = ApproxDistance(unitCell, dest);

            if(dist < minDist)
            {
                pos = dest;
                minDist = dist;
            }
        }
    }

    return (pos.row != -1);
}

bool GameMap::MoveObjectDown(GameObject * obj)
{
    const unsigned int ind = (obj->GetRow0() * mCols) + obj->GetCol0();
    GameMapCell & gc = mCells[ind];

    if(obj != gc.objTop)
        return false;

    if(gc.objBottom)
        return false;

    // move object
    gc.objBottom = gc.objTop;
    gc.objTop = nullptr;

    return true;
}

bool GameMap::MoveObjectUp(GameObject * obj)
{
    const unsigned int ind = (obj->GetRow0() * mCols) + obj->GetCol0();
    GameMapCell & gc = mCells[ind];

    if(obj != gc.objBottom)
        return false;

    if(gc.objTop)
        return false;

    // move object
    gc.objTop = gc.objBottom;
    gc.objBottom = nullptr;

    return true;
}

int GameMap::ApproxDistance(const Cell2D & c1, const Cell2D & c2) const
{
    return std::abs(c1.row - c2.row) + std::abs(c1.col - c2.col);
}

int GameMap::ApproxDistance(const GameObject * obj1, const GameObject * obj2) const
{
    return std::abs(obj1->GetRow0() - obj2->GetRow0()) +
           std::abs(obj1->GetCol0() - obj2->GetCol0());
}


bool GameMap::FindClosestCellConnectedToObject(const GameObject * obj, const Cell2D start, Cell2D & end)
{
    end.row = -1;
    end.col = -1;

    const PlayerFaction faction = obj->GetFaction();

    // object has no faction -> no cell connected
    if(faction == NO_FACTION)
        return false;

    const Player * player = mGame->GetPlayerByFaction(faction);

    // FIND CONNECTED (SAME FACTION) CELLS
    std::vector<unsigned int> todo;
    std::unordered_set<unsigned int> done;

    const Cell2D objCell(obj->GetRow0(), obj->GetCol0());
    const unsigned int indObj = objCell.row * mCols + objCell.col;

    todo.push_back(indObj);

    end = objCell;
    unsigned int closestDist = ApproxDistance(start, objCell);

    while(!todo.empty())
    {
        unsigned int currInd = todo.back();
        todo.pop_back();

        const GameMapCell & currCell = mCells[currInd];
        const Cell2D cc(currCell.row, currCell.col);

        const unsigned int dist = ApproxDistance(start, cc);

        if(dist < closestDist)
        {
            closestDist = dist;
            end = cc;
        }

        // add TOP
        unsigned int r = currCell.row - 1;

        if(r < mRows)
        {
            const unsigned int ind = currInd - mCols;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add BOTTOM
        r = currCell.row + 1;

        if(r < mRows)
        {
            unsigned int ind = currInd + mCols;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add LEFT
        unsigned int c = currCell.col - 1;

        if(c < mCols)
        {
            const unsigned int ind = currInd - 1;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add RIGHT
        c = currCell.col + 1;

        if(c < mCols)
        {
            const unsigned int ind = currInd + 1;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add current to done
        done.insert(currInd);
    }

    return true;
}

bool GameMap::FindClosestLinkedCell(PlayerFaction faction, const Cell2D start, Cell2D & linked)
{
    const int maxDist = mRows * mCols;
    int minDist = maxDist;

    const Player * player = mGame->GetPlayerByFaction(faction);

    const int r0 = start.row;
    const int c0 = start.col;
    const int maxRadiusR = start.row < (mRows / 2) ? (mRows - r0) : (r0 + 1);
    const int maxRadiusC = start.col < (mCols / 2) ? (mCols - c0) : (c0 + 1);
    const int maxRadius = maxRadiusR > maxRadiusC ? maxRadiusR : maxRadiusC;
    int radius = 1;

    linked.row = -1;
    linked.col = -1;

    while(radius < maxRadius)
    {
        const int tlR = (r0 - radius) >= 0 ? (r0 - radius) : 0;
        const int tlC = (c0 - radius) >= 0 ? (c0 - radius) : 0;
        const int brR = (r0 + radius) < (mRows - 1) ? r0 + radius : (mRows - 1);
        const int brC = (c0 + radius) < (mCols - 1) ? c0 + radius : (mCols - 1);

        // TOP
        {
            const int r = tlR;
            const unsigned int ind0 = r * mCols;

            for(int c = tlC; c <= brC; ++c)
            {
                const unsigned int ind = ind0 + c;
                const GameMapCell & cell = mCells[ind];

                if(cell.owner == player && cell.linked)
                {
                    const Cell2D dest(r, c);
                    const int dist = ApproxDistance(start, dest);

                    if(dist < minDist)
                    {
                        linked = dest;
                        minDist = dist;
                    }
                }
            }
        }

        // BOTTOM
        {
            const int r = brR;
            const unsigned int ind0 = r * mCols;

            for(int c = tlC; c <= brC; ++c)
            {
                const unsigned int ind = ind0 + c;
                const GameMapCell & cell = mCells[ind];

                if(cell.owner == player && cell.linked)
                {
                    const Cell2D dest(r, c);
                    const int dist = ApproxDistance(start, dest);

                    if(dist < minDist)
                    {
                        linked = dest;
                        minDist = dist;
                    }
                }
            }
        }

        // LEFT
        {
            const int c = tlC;

            for(int r = tlR + 1; r < brR; ++r)
            {
                const unsigned int ind = r * mCols + c;
                const GameMapCell & cell = mCells[ind];

                if(cell.owner == player && cell.linked)
                {
                    const Cell2D dest(r, c);
                    const int dist = ApproxDistance(start, dest);

                    if(dist < minDist)
                    {
                        linked = dest;
                        minDist = dist;
                    }
                }
            }
        }

        // RIGHT
        {
            const int c = brC;

            for(int r = tlR + 1; r < brR; ++r)
            {
                const unsigned int ind = r * mCols + c;
                const GameMapCell & cell = mCells[ind];

                if(cell.owner == player && cell.linked)
                {
                    const Cell2D dest(r, c);
                    const int dist = ApproxDistance(start, dest);

                    if(dist < minDist)
                    {
                        linked = dest;
                        minDist = dist;
                    }
                }
            }
        }

        if(linked.row != -1)
            return true;

        ++radius;
    }

    return false;
}

void GameMap::OnNewTurn(PlayerFaction faction)
{
    // notify all objects
    for(GameObject * obj : mObjects)
        obj->OnNewTurn(faction);

    // notify all generators
    for(CollectableGenerator * cg : mCollGen)
        cg->OnNewTurn();
}

void GameMap::Update(float delta)
{
    // -- game objects --
    auto itObj = mObjects.begin();

    while(itObj != mObjects.end())
    {
        GameObject * obj = *itObj;

        obj->Update(delta);

        if(obj->IsDestroyed())
        {
            GameObject * obj = *itObj;

            mScreenGame->ClearObjectAction(obj);

            DestroyObjectPaths(obj);

            Player * p = mGame->GetPlayerByFaction(obj->GetFaction());

            if(p != nullptr && p->IsAI())
                p->GetAI()->HandleObjectDestroyed(obj);

            // erase object from vector and set
            itObj = mObjects.erase(itObj);
            mObjectsSet.erase(obj);

            DestroyObject(obj);
        }
        else
            ++itObj;
    }

    // add new objects to map
    UpdateObjectsToAdd();

    // paths
    UpdateObjectPaths(delta);

    // conquer paths
    UpdateConquerPaths(delta);

    // wall building paths
    UpdateWallBuildPaths(delta);
}

// ==================== PRIVATE METHODS ====================

void GameMap::ClearCell(GameMapCell & gcell)
{
    // destroy any generator
    auto it = std::find_if(mCollGen.begin(), mCollGen.end(), [gcell](CollectableGenerator * gen)
    {
        return gen->GetRow() == gcell.row && gen->GetCol() == gcell.col;
    });


    if(it != mCollGen.end())
    {
        delete *it;
        mCollGen.erase(it);
    }

    gcell.currType = EMPTY;
}

int GameMap::DefineCellType(unsigned int ind, const GameMapCell & cell)
{
    // if cell is not visible it's always Fog Of War
    if(!mGame->GetLocalPlayer()->IsCellVisible(ind))
        return FOG_OF_WAR;

    // scene cell
    if(SCENE_ROCKS == cell.currType || DIAMONDS_SOURCE == cell.currType ||
       BLOBS_SOURCE == cell.currType || TREES1 == cell.currType)
        return cell.currType;

    const PlayerFaction ownerFaction = cell.owner ? cell.owner->GetFaction() : NO_FACTION;

    int type = EMPTY;

    switch(ownerFaction)
    {
        case FACTION_1:
            if(cell.linked)
                type = F1_CONNECTED;
            else
                type = F1;
        break;

        case FACTION_2:
            if(cell.linked)
                type = F2_CONNECTED;
            else
                type = F2;
        break;

        case FACTION_3:
            if(cell.linked)
                type = F3_CONNECTED;
            else
                type = F3;
        break;

        // no owner
        default:
        {
            if(0 == cell.influencer)
                type = F1_INFLUENCED;
            else if(1 == cell.influencer)
                type = F2_INFLUENCED;
            else if(2 == cell.influencer)
                type = F3_INFLUENCED;
            // no influence
            else
                type = cell.basicType;
        }
        break;
    }

    return type;
}

void GameMap::UpdateLinkedCells(Player * player)
{
    const GameObject * b = player->GetBase();

    if(nullptr == b)
        return;

    const std::vector<Structure *> & structures = player->GetStructures();
    std::unordered_map<GameObject *, bool> objsLink;
    std::unordered_map<int, bool> cells;

    // CLEAR ALL LINKED STATUS
    for(int r = 0; r < mRows; ++r)
    {
        const int ind0 = r * mCols;

        for(int c = 0; c < mCols; ++c)
        {
            const int ind = ind0 + c;
            const GameMapCell & cell = mCells[ind];

            if(cell.owner == player)
                cells.emplace(ind, false);
        }
    }

    // reset all objects linked flag and remove visibility
    for(GameObject * obj : structures)
    {
        DelPlayerObjVisibility(obj, player);

        objsLink.insert({obj, false});
    }

    // FIND LINKED CELLS
    std::vector<unsigned int> todo;
    std::unordered_set<unsigned int> done;

    const unsigned int indHome = b->GetRow0() * mCols + b->GetCol0();
    todo.push_back(indHome);

    while(!todo.empty())
    {
        unsigned int currInd = todo.back();
        todo.pop_back();

        const GameMapCell & currCell = mCells[currInd];
        cells[currInd] = true;

        // mark object as linked
        if(currCell.objTop != nullptr)
            objsLink[currCell.objTop] = true;
        else if(currCell.objBottom != nullptr)
            objsLink[currCell.objBottom] = true;

        // add TOP
        unsigned int r = currCell.row - 1;

        if(r < mRows)
        {
            const unsigned int ind = currInd - mCols;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add BOTTOM
        r = currCell.row + 1;

        if(r < mRows)
        {
            unsigned int ind = currInd + mCols;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add LEFT
        unsigned int c = currCell.col - 1;

        if(c < mCols)
        {
            const unsigned int ind = currInd - 1;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add RIGHT
        c = currCell.col + 1;

        if(c < mCols)
        {
            const unsigned int ind = currInd + 1;

            if(mCells[ind].owner == player && done.find(ind) == done.end())
                todo.push_back(ind);
        }

        // add current to done
        done.insert(currInd);
    }

    // UPDATE LINK STATUS CELLS
    for(const auto & it : cells)
    {
        GameMapCell & cell = mCells[it.first];

        // linked status of cell changed
        if(cell.linked != it.second)
        {
            cell.linked = it.second;

            if(cell.linked)
                mControlMap->AddControlPointsForCell(cell.row, cell.col, cell.owner->GetFaction());
        }
    }

    // UPDATE LINK STATUS OBJECTS
    bool radarLinked = false;

    for(GameObject * obj : structures)
    {
        if(obj->IsLinked() != objsLink[obj])
        {
            obj->SetLinked(objsLink[obj]);

            if(obj->IsLinked())
            {
                // add control points of new linked object
                mControlMap->AddControlPointsForObject(obj);

                // mark linked radar
                if(obj->GetObjectType() == GameObject::TYPE_RADAR_STATION)
                    radarLinked = true;
            }
        }
    }

    // enable minimap for local player if a radar is linked
    if(player->IsLocal())
        mScreenGame->SetMiniMapEnabled(radarLinked);

    // UPDATE INFLUENCE
    for(unsigned int ind : done)
    {
        const int row = ind / mCols;
        const int col = ind % mCols;

        UpdateInfluencedCells(row, col);
    }

    // UPDATE ALL CELLS IMAGE
    const unsigned int totCells = mRows * mCols;

    for(unsigned int ind = 0; ind < totCells; ++ind)
    {
        const GameMapCell & cell = mCells[ind];

        if(cell.owner == player || cell.influencer != NO_FACTION)
            UpdateCellType(ind, cell);
    }

    // UPDATE OBJECTS VISIBILITY
    for(GameObject * obj : structures)
        AddPlayerObjVisibility(obj, player);

   ApplyLocalVisibility();
}

void GameMap::UpdateInfluencedCells(int row, int col)
{
    const unsigned int ind0 = row * mCols + col;
    const GameMapCell & gcell = mCells[ind0];

    const PlayerFaction faction = gcell.owner->GetFaction();

    const unsigned int r0 = (row > 0) ? row - 1 : row;
    const unsigned int c0 = (col > 0) ? col - 1 : col;

    const unsigned int r1Inc = row + 2;
    const unsigned int r1 = (r1Inc < mRows) ? r1Inc : mRows;

    const unsigned int c1Inc = col + 2;
    const unsigned int c1 = (c1Inc < mCols) ? c1Inc : mCols;

    for(unsigned int r = r0; r < r1; ++r)
    {
        const unsigned int indBase = r * mCols;

        for(unsigned int c = c0; c < c1; ++c)
        {
            const unsigned int ind = indBase + c;
            GameMapCell & gc = mCells[ind];

            // not walkable empty cell
            if(!gc.walkable && nullptr == gc.objTop)
                continue;

            // update map of influence
            gc.influencers[faction] = true;

            // count active influencers to see if there's only one
            int influencers = 0;

            for(auto it : gc.influencers)
            {
                if(it.second)
                    ++influencers;
            }

            if(influencers == 1)
                gc.influencer = faction;
            else
                gc.influencer = NO_FACTION;

            UpdateCellType(ind, gc);
        }
    }
}

void GameMap::UpdateVisibility(Player * player, bool init)
{
    // no visibility for AI
    if(player->IsAI())
        return ;

    // update cells
    const unsigned int totCells = mRows * mCols;

    for(unsigned int ind = 0; ind < totCells; ++ind)
    {
        const GameMapCell & cell = mCells[ind];
        UpdateCellType(ind, cell);
    }

    // update objects
    if(init)
    {
        for(GameObject * go : mObjects)
            InitObjectVisibility(player, go);
    }
    else
    {
        for(GameObject * go : mObjects)
            ApplyVisibilityToObject(player, go);
    }

    // update control areas
    mControlMap->UpdateVisualAreas();
}

bool GameMap::MoveObjToCell(GameObject * obj, int row, int col)
{
    // TODO support objects covering more than 1 cell
    const int ind0 = obj->GetRow0() * mCols + obj->GetCol0();

    // object is not in its cell !?
    if(mCells[ind0].objTop != obj)
        return false;

    // update minimap
    MiniMap * mm = mScreenGame->GetMiniMap();
    mm->MoveElement(obj->GetRow0(), obj->GetCol0(), row, col);

    // move object in iso map
    IsoLayer * layer = obj->GetIsoObject()->GetLayer();
    layer->MoveObject(obj->GetRow0(), obj->GetCol0(), row, col, false);

    // remove object from current cell
    mCells[ind0].objTop = nullptr;
    mCells[ind0].walkable = true;

    // add object to new cell
    const int ind1 = row * mCols + col;

    obj->SetCell(&mCells[ind1]);

    mCells[ind1].objTop = obj;
    mCells[ind1].walkable = false;
    mCells[ind1].walkTarget = false;

    return true;
}

Cell2D GameMap::GetClosestCell(const Cell2D & start, const std::vector<Cell2D> & targets) const
{
    // failed to find any walkable
    if(targets.empty())
        return Cell2D(-1, -1);

    // get closest cell
    int minInd = 0;
    int minDist = std::abs(start.row - targets[minInd].row) +
                  std::abs(start.col - targets[minInd].col);

    for(unsigned int i = 1; i < targets.size(); ++i)
    {
        const int dist = std::abs(start.row - targets[i].row) +
                         std::abs(start.col - targets[i].col);

        if(dist < minDist)
        {
            minDist = dist;
            minInd = i;
        }
    }

    return targets[minInd];
}

void GameMap::AddObjectToMap(const ObjectToAdd & o2a)
{
    // add object to map containers
    mObjects.push_back(o2a.obj);
    mObjectsSet.insert(o2a.obj);

    // generic cells update
    for(unsigned int r = o2a.r1; r <= o2a.r0; ++r)
    {
        const unsigned int indBase = r * mCols;

        for(unsigned int c = o2a.c1; c <= o2a.c0; ++c)
        {
            const unsigned int ind = indBase + c;

            GameMapCell & cell = mCells[ind];

            cell.walkable = o2a.obj->CanBeCollected();
            cell.objTop = o2a.obj;
            cell.owner = o2a.owner;

            // update cell image
            UpdateCellType(ind, cell);
        }
    }

    // handle owned object
    if(o2a.owner != nullptr)
    {
        // register objects to Player
        if(o2a.obj->GetObjectCategory() == GameObject::CAT_RES_GENERATOR)
            o2a.owner->AddResourceGenerator(static_cast<ResourceGenerator *>(o2a.obj));
        if(o2a.obj->IsStructure())
            o2a.owner->AddStructure(static_cast<Structure *>(o2a.obj));

        // update control points
        if(o2a.obj->IsLinked())
            mControlMap->AddControlPointsForObject(o2a.obj);

        // update visibility map
        // NOTE only for human player for now
        Player * localPlayer = mGame->GetLocalPlayer();

        if(o2a.owner == localPlayer)
            AddPlayerObjVisibility(o2a.obj, localPlayer);
    }

    // create object in iso map
    mIsoMap->GetLayer(o2a.layer)->AddObject(o2a.obj->GetIsoObject(), o2a.r0, o2a.c0);

    // apply visibility
    ApplyLocalVisibilityToObject(o2a.obj);
}

void GameMap::DestroyObject(GameObject * obj)
{
    Player * owner = mGame->GetPlayerByFaction(obj->GetFaction());

    if(owner != nullptr)
    {
        Player * localPlayer = mGame->GetLocalPlayer();

        // owner is local Player
        if(owner == localPlayer)
        {
            // clear selection if object is selected
            if(owner->GetSelectedObject() == obj)
                mScreenGame->ClearSelection(owner);

            // update visibility map
            // NOTE only local player for now
            DelPlayerObjVisibility(obj, localPlayer);
        }

        // remove unit from player
        if(obj->GetObjectCategory() == GameObject::CAT_UNIT)
            owner->RemoveUnit(static_cast<Unit *>(obj));
        // remove structure
        else if(obj->IsStructure())
        {
            owner->RemoveStructure(static_cast<Structure *>(obj));

            // remove resource generator
            if(obj->GetObjectCategory() == GameObject::CAT_RES_GENERATOR)
                owner->RemoveResourceGenerator(static_cast<ResourceGenerator *>(obj));
        }
    }

    // generic cells update
    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        const unsigned int indBase = r * mCols;

        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            const unsigned int ind = indBase + c;

            GameMapCell & cell = mCells[ind];

            cell.walkable = true;
            cell.objTop = nullptr;

            // update cell image
            UpdateCellType(ind, cell);
        }
    }

    // remove iso object from layer
    IsoObject * isoObj = obj->GetIsoObject();
    IsoLayer * layer = isoObj->GetLayer();
    layer->ClearObject(isoObj);

    // remove object from mini map
    mScreenGame->GetMiniMap()->RemoveElement(obj->GetRow0(), obj->GetCol0());

    // finally delete the object
    delete obj;
}

void  GameMap::AddVisibilityToCell(Player * player, int ind)
{
    player->AddVisibility(ind);
}

void GameMap::DelVisibilityToCell(Player * player, int ind)
{
    player->RemVisibility(ind);
}

void GameMap::AddPlayerObjVisibility(GameObject * obj, Player * player)
{
    using namespace std::placeholders;

    // player is bounded to the function object so I don't need to propagate it as param
    PropagatePlayerObjVisibility(obj,
                                 std::bind(&GameMap::AddVisibilityToCell, this, player, _1));
}

void GameMap::DelPlayerObjVisibility(GameObject * obj, Player * player)
{
    using namespace std::placeholders;

    // player is bounded to the function object so I don't need to propagate it as param
    PropagatePlayerObjVisibility(obj,
                                 std::bind(&GameMap::DelVisibilityToCell, this, player, _1));
}

void GameMap::PropagatePlayerObjVisibility(GameObject * obj, std::function<void(int)> visFun)
{
    const Cell2D cell1(obj->GetRow1(), obj->GetCol1());
    const Cell2D cell0(obj->GetRow0(), obj->GetCol0());

    const int objVisLvl = obj->GetVisibilityLevel();

    PropagatePlayerVisibility(cell1, cell0, objVisLvl, visFun);
}

void GameMap::AddPlayerCellVisibility(const GameMapCell & cell, Player * player)
{
    using namespace std::placeholders;

    // player is bounded to the function object so I don't need to propagate it as param
    PropagatePlayerCellVisibility(cell,
                                  std::bind(&GameMap::AddVisibilityToCell, this, player, _1));
}

void GameMap::DelPlayerCellVisibility(const GameMapCell & cell, Player * player)
{
    using namespace std::placeholders;

    // player is bounded to the function object so I don't need to propagate it as param
    PropagatePlayerCellVisibility(cell,
                                  std::bind(&GameMap::DelVisibilityToCell, this, player, _1));
}

void GameMap::PropagatePlayerCellVisibility(const GameMapCell & cell, std::function<void(int)> visFun)
{
    const Cell2D c(cell.row, cell.col);

    // TODO get visibility from player or base
    const int visLvl = 0;

    PropagatePlayerVisibility(c, c, visLvl, visFun);
}

void GameMap::PropagatePlayerVisibility(const Cell2D & cell1, const Cell2D & cell0,
                                        int visLevel, std::function<void(int)> visFun)
{
    const int objRows = 1 + cell0.row - cell1.row;
    const int objCols = 1 + cell0.col - cell1.col;

    const int visHalfLen0 = 2;
    const int visLenInc = 1;
    const int visExtSide = visHalfLen0 + visLenInc * visLevel;

    const int visLenMax = objRows + visExtSide * 2;

    const int visSideCols = (visLenMax - 1) / 2;

    // object columns
    int minCol = cell1.col;
    int maxCol = cell0.col;

    if(1 == objCols && visLevel % 2 == 1)
    {
        if(minCol > 0)
            minCol -= 1;

        if(maxCol < static_cast<int>(mCols - 1))
            maxCol += 1;
    }

    int minRow = cell1.row - visExtSide;
    int maxRow = cell0.row + visExtSide;

    for(int c = minCol; c <= maxCol; ++c)
    {
        for(int r = minRow; r <= maxRow; ++r)
        {
            if(r < 0 || r >= static_cast<int>(mRows))
                continue;

            const int ind = r * mCols + c;

            visFun(ind);
        }
    }

    // left columns
    if(minCol - 1 > -1)
    {
        int minColL = minCol - visSideCols;
        int maxColL = minCol - 1;

        if(minColL < 0)
            minColL = 0;

        int minRowL = minRow;
        int maxRowL = maxRow;

        for(int c = maxColL; c >= minColL; --c)
        {
            ++minRowL;
            --maxRowL;

            for(int r = minRowL; r <= maxRowL; ++r)
            {
                if(r < 0 || r >= static_cast<int>(mRows))
                    continue;

                const int ind = r * mCols + c;

                visFun(ind);
            }
        }
    }

    // right columns
    if(maxCol + 1 < static_cast<int>(mCols))
    {
        int minColR = maxCol + 1;
        int maxColR = maxCol + visSideCols;

        if(maxColR >= static_cast<int>(mCols))
            maxColR = mCols - 1;

        int minRowR = minRow;
        int maxRowR = maxRow;

        for(int c = minColR; c <= maxColR; ++c)
        {
            ++minRowR;
            --maxRowR;

            for(int r = minRowR; r <= maxRowR; ++r)
            {
                if(r < 0 || r >= static_cast<int>(mRows))
                    continue;

                const int ind = r * mCols + c;

                visFun(ind);
            }
        }
    }
}

void GameMap::DestroyObjectPaths(GameObject * obj)
{
    // ObjectPaths
    auto itPath = mPaths.begin();

    while(itPath != mPaths.end())
    {
        ObjectPath * path = *itPath;

        if(path->GetObject() == obj)
        {
            path->InstantAbort();
            delete path;
            itPath = mPaths.erase(itPath);
            break;
        }
        else
            ++itPath;
    }

    // other paths are only for units
    if(obj->GetObjectCategory() != GameObject::CAT_UNIT)
        return ;

    auto unit = static_cast<Unit *>(obj);

    // ConquerPaths
    auto itCP = mConquerPaths.begin();

    while(itCP != mConquerPaths.end())
    {
        ConquerPath * path = *itCP;

        if(path->GetUnit() == unit)
        {
            path->InstantAbort();
            delete path;
            itCP = mConquerPaths.erase(itCP);
            break;
        }
        else
            ++itCP;
    }

    // WallBuildPaths
    auto itWBP = mWallBuildPaths.begin();

    while(itWBP != mWallBuildPaths.end())
    {
        WallBuildPath * path = *itWBP;

        if(path->GetUnit() == unit)
        {
            path->InstantAbort();
            delete path;
            itWBP = mWallBuildPaths.erase(itWBP);
            break;
        }
        else
            ++itWBP;
    }
}

void GameMap::UpdateObjectPaths(float delta)
{
    auto itPath = mPaths.begin();

    while(itPath != mPaths.end())
    {
        ObjectPath * path = *itPath;

        path->Update(delta);

        const ObjectPath::PathState state = path->GetState();

        if(state == ObjectPath::PathState::COMPLETED || state == ObjectPath::PathState::ABORTED)
        {
            delete path;
            itPath = mPaths.erase(itPath);
        }
        else if(state == ObjectPath::PathState::FAILED)
        {
            // TODO try to recover from failed path
            delete path;
            itPath = mPaths.erase(itPath);
        }
        else
            ++itPath;
    }
}

void GameMap::UpdateConquerPaths(float delta)
{
    auto itCP = mConquerPaths.begin();

    while(itCP != mConquerPaths.end())
    {
        ConquerPath * path = *itCP;

        path->Update(delta);

        const ConquerPath::ConquerState state = path->GetState();

        if(state == ConquerPath::ConquerState::COMPLETED || state == ConquerPath::ConquerState::ABORTED)
        {
            delete path;
            itCP = mConquerPaths.erase(itCP);
        }
        else if(state == ConquerPath::ConquerState::FAILED)
        {
            // TODO try to recover from failed path
            delete path;
            itCP = mConquerPaths.erase(itCP);
        }
        else
            ++itCP;
    }
}

void GameMap::UpdateWallBuildPaths(float delta)
{
    auto it = mWallBuildPaths.begin();

    while(it != mWallBuildPaths.end())
    {
        WallBuildPath * path = *it;

        path->Update(delta);

        const WallBuildPath::BuildState state = path->GetState();

        if(state == WallBuildPath::BuildState::COMPLETED || state == WallBuildPath::BuildState::ABORTED)
        {
            delete path;
            it = mWallBuildPaths.erase(it);
        }
        else if(state == WallBuildPath::BuildState::FAILED)
        {
            // TODO try to recover from failed path
            delete path;
            it = mWallBuildPaths.erase(it);
        }
        else
            ++it;
    }
}

void GameMap::UpdateObjectsToAdd()
{
    while(!mObjectsToAdd.empty())
    {
        AddObjectToMap(mObjectsToAdd.back());

        mObjectsToAdd.pop_back();
    }
}

void GameMap::UpdateWalls(const Cell2D & center)
{
    const int rows = static_cast<int>(mRows);
    const int cols = static_cast<int>(mCols);

    // cell out of map !?
    if(center.row < 0 || center.col < 0 || center.row >= rows || center.col >= cols)
        return ;

    UpdateWall(center);

    // update north
    if(center.row > 0)
        UpdateWall({center.row - 1, center.col});

    // update south
    if(center.row + 1 < rows)
        UpdateWall({center.row + 1, center.col});

    // update west
    if(center.col > 0)
        UpdateWall({center.row, center.col - 1});

    // update east
    if(center.col + 1 < cols)
        UpdateWall({center.row, center.col + 1});
}

void GameMap::UpdateWall(const Cell2D & cell)
{
    GameObject * obj = GetCell(cell.row, cell.col).objTop;

    // no wall or gate here
    if(nullptr == obj || (obj->GetObjectType() != GameObject::TYPE_WALL && obj->GetObjectType() != GameObject::TYPE_WALL_GATE))
        return ;

    const GameObject * objN = (cell.row - 1 >= 0) ? GetCell(cell.row - 1, cell.col).objTop : nullptr;
    const bool wallN = objN && (objN->GetObjectType() == GameObject::TYPE_WALL ||
                                objN->GetObjectType() == GameObject::TYPE_DEFENSIVE_TOWER ||
                                objN->GetObjectType() == GameObject::TYPE_WALL_GATE);

    const GameObject * objS = (cell.row + 1 < static_cast<int>(mRows)) ? GetCell(cell.row + 1, cell.col).objTop : nullptr;
    const bool wallS = objS && (objS->GetObjectType() == GameObject::TYPE_WALL ||
                                objS->GetObjectType() == GameObject::TYPE_DEFENSIVE_TOWER ||
                                objS->GetObjectType() == GameObject::TYPE_WALL_GATE);

    const GameObject * objW = (cell.col - 1 >= 0) ? GetCell(cell.row, cell.col - 1).objTop : nullptr;
    const bool wallW = objW && (objW->GetObjectType() == GameObject::TYPE_WALL ||
                                objW->GetObjectType() == GameObject::TYPE_DEFENSIVE_TOWER ||
                                objW->GetObjectType() == GameObject::TYPE_WALL_GATE);

    const GameObject * objE = (cell.col + 1 < static_cast<int>(mCols)) ? GetCell(cell.row, cell.col + 1).objTop : nullptr;
    const bool wallE = objE && (objE->GetObjectType() == GameObject::TYPE_WALL ||
                                objE->GetObjectType() == GameObject::TYPE_DEFENSIVE_TOWER ||
                                objE->GetObjectType() == GameObject::TYPE_WALL_GATE);

    enum Flags
    {
        NORTH   = 0x1,
        SOUTH   = 0x2,
        WEST    = 0x4,
        EAST    = 0x8
    };

    enum Configurations
    {
        HORIZ_1 = WEST,
        HORIZ_2 = EAST,
        HORIZ_3 = WEST + EAST,

        VERT_1 = NORTH,
        VERT_2 = SOUTH,
        VERT_3 = NORTH + SOUTH,

        TL = EAST + SOUTH,
        TR = WEST + SOUTH,
        BL = EAST+ NORTH,
        BR = WEST + NORTH,

        INTN = NORTH + WEST + EAST,
        INTS = SOUTH + WEST + EAST,
        INTW = NORTH + SOUTH + WEST,
        INTE = NORTH + SOUTH + EAST,

        CROSS = NORTH + SOUTH + WEST + EAST
    };

    const int conf = NORTH * static_cast<int>(wallN) +
                     SOUTH * static_cast<int>(wallS) +
                     WEST * static_cast<int>(wallW) +
                     EAST * static_cast<int>(wallE);

    if(obj->GetObjectType() == GameObject::TYPE_WALL)
    {
        auto wall = static_cast<Wall *>(obj);

        switch(conf)
        {
            case HORIZ_1:
            case HORIZ_2:
            case HORIZ_3:
                wall->SetObjectVariant(Wall::HORIZ);
            break;

            case VERT_1:
            case VERT_2:
            case VERT_3:
                wall->SetObjectVariant(Wall::VERT);
            break;

            case TL:
                wall->SetObjectVariant(Wall::TL);
            break;
            case TR:
                wall->SetObjectVariant(Wall::TR);
            break;
            case BL:
                wall->SetObjectVariant(Wall::BL);
            break;
            case BR:
                wall->SetObjectVariant(Wall::BR);
            break;

            case INTN:
                wall->SetObjectVariant(Wall::INTN);
            break;
            case INTS:
                wall->SetObjectVariant(Wall::INTS);
            break;
            case INTW:
                wall->SetObjectVariant(Wall::INTW);
            break;
            case INTE:
                wall->SetObjectVariant(Wall::INTE);
            break;

            case CROSS:
                wall->SetObjectVariant(Wall::CROSS);
            break;

            default:
                // do nothing for any other case
            break;
        }
    }
    else if(obj->GetObjectType() == GameObject::TYPE_WALL_GATE)
    {
        auto gate = static_cast<WallGate *>(obj);

        switch(conf)
        {
            case HORIZ_1:
            case HORIZ_2:
            case HORIZ_3:
                gate->SetObjectVariant(WallGate::HORIZ);
            break;

            case VERT_1:
            case VERT_2:
            case VERT_3:
                gate->SetObjectVariant(WallGate::VERT);
            break;

            default:
                // do nothing for any other case
            break;
        }
    }
}

const ObjectData & GameMap::GetObjectData(GameObjectTypeId t) const
{
    const ObjectsDataRegistry * objReg = mGame->GetObjectsRegistry();
    return objReg->GetObjectData(t);
}

} // namespace game
