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
#include "GameObjectTools/Laser.h"
#include "GameObjectTools/WeaponData.h"
#include "GameObjects/Barracks.h"
#include "GameObjects/Base.h"
#include "GameObjects/Blobs.h"
#include "GameObjects/BlobsGenerator.h"
#include "GameObjects/Bunker.h"
#include "GameObjects/CityBlock.h"
#include "GameObjects/CityGroup.h"
#include "GameObjects/DefensiveTower.h"
#include "GameObjects/Diamonds.h"
#include "GameObjects/DiamondsGenerator.h"
#include "GameObjects/Hospital.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjects/MiniUnitsGroup.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectInitData.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/PracticeTarget.h"
#include "GameObjects/RadarStation.h"
#include "GameObjects/RadarTower.h"
#include "GameObjects/ResearchCenter.h"
#include "GameObjects/ResourceGenerator.h"
#include "GameObjects/ResourceStorage.h"
#include "GameObjects/SceneObject.h"
#include "GameObjects/SpawningTower.h"
#include "GameObjects/Temple.h"
#include "GameObjects/TradingPost.h"
#include "GameObjects/Trees.h"
#include "GameObjects/Unit.h"
#include "GameObjects/Wall.h"
#include "GameObjects/WallGate.h"
#include "Screens/ScreenGame.h"
#include "Widgets/MiniMap.h"

#include <sgl/ai/Pathfinder.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/StringManager.h>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace
{
const float timeAutoAttackDelay = 0.50f;
}

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
    , mSM(sgl::utilities::StringManager::Instance())
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

    // PATHFINDER
    mPathfinder = new sgl::ai::Pathfinder;
}

GameMap::~GameMap()
{
    delete mPathfinder;

    delete mControlMap;

    for(auto obj : mObjects)
        delete obj;

    for(auto g : mMiniUnitsGroups)
        delete g;

    for(auto g : mCities)
        delete g;

    for(auto cg : mCollGen)
        delete cg;

    for(auto op : mPaths)
        delete op;

    for(auto cp : mConquerPaths)
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

    // finish init of pathfinder
    mPathfinder->SetMap(this);
}

void GameMap::CreateCollectableGenerator(unsigned int r, unsigned int c, ResourceType type)
{
    CollectableGenerator * gen = nullptr;

    if(RES_DIAMONDS == type)
        gen = new DiamondsGenerator(mGame, this);
    else if(RES_BLOBS == type)
        gen = new BlobsGenerator(mGame, this);
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

void GameMap::UpdateLocalObjectVisibility(GameObject * go)
{
    Player * p = mGame->GetLocalPlayer();

    DelPlayerObjVisibility(go, p);

    AddPlayerObjVisibility(go, p);

    ApplyLocalVisibility();
}

void GameMap::CreateObjectFromFile(unsigned int layerId, GameObjectTypeId type, GameObjectVariantId variant,
                                   unsigned int faction, unsigned int r0, unsigned int c0)
{
    const auto pf = static_cast<PlayerFaction>(faction);

    if(ObjectData::TYPE_UNIT_MEDIC1 == type || ObjectData::TYPE_UNIT_SCOUT1 == type ||
       ObjectData::TYPE_UNIT_SOLDIER1 == type || ObjectData::TYPE_UNIT_SOLDIER2 == type ||
       ObjectData::TYPE_UNIT_SPAWNER1 == type || ObjectData::TYPE_UNIT_SPAWNER2 == type ||
       ObjectData::TYPE_UNIT_WORKER1 == type)
    {
        Player * owner =  mGame->GetPlayerByFaction(pf);

        if(nullptr == owner)
            return ;

        const Cell2D dest(r0, c0);

        CreateUnit(type, dest, owner);
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

    // CONVERT BASE SPOT TO BASE
    if(ObjectData::TYPE_BASE_SPOT == type)
    {
        o2a.owner = mGame->GetPlayerByIndex(variant);
        type = ObjectData::TYPE_BASE;
    }

    // data to pass to new object
    const ObjectInitData initData(mGame, this, mScreenGame->GetParticlesManager(),
                                  o2a.owner, mScreenGame);

    if(ObjectData::TYPE_MOUNTAINS == type ||
       ObjectData::TYPE_ROCKS == type)
        o2a.obj = new SceneObject(data, initData, variant);
    else if(ObjectData::TYPE_RES_GEN_ENERGY == type ||
       ObjectData::TYPE_RES_GEN_MATERIAL == type ||
       ObjectData::TYPE_RES_GEN_ENERGY_SOLAR == type ||
       ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT == type)
        o2a.obj = new ResourceGenerator(data, initData);
    else if(ObjectData::TYPE_RES_STORAGE_BLOBS == type ||
            ObjectData::TYPE_RES_STORAGE_DIAMONDS == type ||
            ObjectData::TYPE_RES_STORAGE_ENERGY == type ||
            ObjectData::TYPE_RES_STORAGE_MATERIAL == type)
        o2a.obj = new ResourceStorage(data, initData);
    else if(ObjectData::TYPE_DIAMONDS == type)
        o2a.obj = new Diamonds(data, initData);
    else if(ObjectData::TYPE_BLOBS == type)
        o2a.obj  = new Blobs(data, initData);
    else if(ObjectData::TYPE_TREES == type)
        o2a.obj  = new Trees(data, initData, variant);
    else if(ObjectData::TYPE_RADAR_STATION == type)
        o2a.obj = new RadarStation(data, initData);
    else if(ObjectData::TYPE_RADAR_TOWER == type)
        o2a.obj = new RadarTower(data, initData);
    else if(ObjectData::TYPE_BARRACKS == type)
        o2a.obj = new Barracks(data, initData);
    else if(ObjectData::TYPE_CITY_BLOCK == type)
        o2a.obj = new CityBlock(data, initData, variant);
    else if(ObjectData::TYPE_RESEARCH_CENTER == type)
        o2a.obj = new ResearchCenter(data, initData);
    else if(ObjectData::TYPE_HOSPITAL == type)
        o2a.obj = new Hospital(data, initData);
    else if(ObjectData::TYPE_DEFENSIVE_TOWER == type)
        o2a.obj = new DefensiveTower(data, initData);
    else if(ObjectData::TYPE_BUNKER == type)
        o2a.obj = new Bunker(data, initData);
    else if(ObjectData::TYPE_SPAWN_TOWER == type)
        o2a.obj = new SpawningTower(data, initData);
    else if(ObjectData::TYPE_TRADING_POST == type)
        o2a.obj = new TradingPost(data, initData);
    else if(ObjectData::TYPE_WALL == type)
        o2a.obj = new Wall(data, initData, variant);
    else if(ObjectData::TYPE_WALL_GATE == type)
        o2a.obj = new WallGate(data, initData, variant);
    else if(ObjectData::TYPE_LOOTBOX == type || ObjectData::TYPE_LOOTBOX2 == type)
        o2a.obj = new LootBox(data, initData);
    else if(ObjectData::TYPE_TEMPLE == type)
        o2a.obj = new Temple(data, initData);
    else if(ObjectData::TYPE_PRACTICE_TARGET == type)
        o2a.obj = new PracticeTarget(data, initData);
    else if(ObjectData::TYPE_BASE == type)
    {
        const ObjectData & dataBase = GetObjectData(type);
        auto b = new Base(dataBase, initData);
        o2a.obj = b;

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

        o2a.owner->SetBase(b);
        o2a.owner->SumCells(rows * cols);
    }
    // this should never happen
    else
    {
        std::cerr << "[ERR] GameMap::CreateObject - unknown obj type: " << type << std::endl;
        return nullptr;
    }

    // set object properties
    o2a.obj->SetCell(&mCells[ind0]);

    // weapon
    AssignWeaponToObject(data.GetWeapon(), o2a.obj);

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

void GameMap::InitCities()
{
    std::vector<GameObject *> blockTodo;

    for(auto o : mObjects)
    {
        // skip other objects
        if(o->GetObjectType() != ObjectData::TYPE_CITY_BLOCK)
            continue;

        // block already done
        if(o->GetGroup() != nullptr)
            continue;

        auto g = new CityGroup(this);
        mCities.emplace_back(g);

        blockTodo.emplace_back(o);

        while(!blockTodo.empty())
        {
            auto b = blockTodo.back();
            blockTodo.pop_back();

            // block already done
            if(b->GetGroup() != nullptr)
                continue;

            // set city group
            b->SetGroup(g);

            // add neighbors
            const int r0 = b->GetRow0();
            const int c0 = b->GetCol0();
            const int rows = b->GetRows();
            const int cols = b->GetCols();

            bool border = false;

            // TOP
            if(r0 > rows)
            {
                const int r = r0 - rows;
                const int ind0 = r * mCols;

                // TL
                if(c0 > cols)
                {
                    const int c = c0 - cols;
                    const int ind = ind0 + c;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }

                // T
                {
                    const int ind = ind0 + c0;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }

                // TR
                if(c0 + cols < mCols)
                {
                    const int c = c0 + cols;
                    const int ind = ind0 + c;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }
            }

            // LEFT
            if(c0 > cols)
            {
                const int c = c0 - cols;
                const int ind = (r0 * mCols) + c;

                auto co = mCells[ind].objTop;

                if(co == nullptr)
                    border = true;
                else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                         co->GetGroup() == nullptr)
                    blockTodo.emplace_back(co);
            }

            // RIGHT
            if(c0 + cols < mCols)
            {
                const int c = c0 + cols;
                const int ind = (r0 * mCols) + c;

                auto co = mCells[ind].objTop;

                if(co == nullptr)
                    border = true;
                else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                         co->GetGroup() == nullptr)
                    blockTodo.emplace_back(co);
            }

            // BOTTOM
            if(r0 + rows < mRows)
            {
                const int r = r0 + rows;
                const int ind0 = r * mCols;

                // TL
                if(c0 > cols)
                {
                    const int c = c0 - cols;
                    const int ind = ind0 + c;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }

                // T
                {
                    const int ind = ind0 + c0;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }

                // TR
                if(c0 + cols < mCols)
                {
                    const int c = c0 + cols;
                    const int ind = ind0 + c;

                    auto co = mCells[ind].objTop;

                    if(co == nullptr)
                        border = true;
                    else if (co->GetObjectType() == ObjectData::TYPE_CITY_BLOCK &&
                             co->GetGroup() == nullptr)
                        blockTodo.emplace_back(co);
                }

                static_cast<CityBlock *>(b)->SetBorder(border);
            }
        }
    }
}

void GameMap::RegisterEnemyKill(GameObject * killer)
{
    // TODO assign experience points based on kill maybe
    const int experienceKill = 100;
    killer->SumExperience(experienceKill);

    ++mEnemiesKilled[killer->GetFaction()];
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

bool GameMap::HasResourcesToConquerCell(Unit * unit)
{
    // check if unit has enough energy
    if(!unit->HasEnergyForActionStep(CONQUER_CELL))
        return false;

    // check if player has enough resources
    Player * player = mGame->GetPlayerByFaction(unit->GetFaction());

    return player->HasEnough(Player::Stat::MATERIAL, COST_CONQUEST_CELL);
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

    return true;
}

void GameMap::StartConquerCell(const Cell2D & cell, Player * player)
{
    const int ind = cell.row * mCols + cell.col;
    GameMapCell & gcell = mCells[ind];

    // take player's energy
    player->SumResource(Player::Stat::MATERIAL, -COST_CONQUEST_CELL);
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

    // propagate effects of conquest
    UpdateInfluencedCells(cell.row, cell.col);

    UpdateLinkedCells(player);

    // update visibility map
    AddPlayerCellVisibility(gcell, player);

    if(stolen)
    {
        DelPlayerCellVisibility(gcell, prevOwner);

        UpdateLinkedCells(prevOwner);
    }

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
    {
        unit->ShowWarning(mSM->GetCString("WARN_CELL_NOT_VALID"), 3.f);
        return false;
    }

    // check costs
    const auto & costs = data.GetCosts();
    const bool costOk = player->HasEnough(Player::ENERGY, costs[RES_ENERGY]) &&
                        player->HasEnough(Player::MATERIAL, costs[RES_MATERIAL1]) &&
                        player->HasEnough(Player::DIAMONDS, costs[RES_DIAMONDS]) &&
                        player->HasEnough(Player::BLOBS, costs[RES_BLOBS]);

    if(!costOk)
    {
        unit->ShowWarning(mSM->GetCString("WARN_LACK_RES"), 2.f);
        return false;
    }

    // check unit's energy
    if(!unit->HasEnergyForActionStep(BUILD_STRUCTURE))
    {
        unit->ShowWarning(mSM->GetCString("WARN_NO_ENE"), 2.f);
        return false;
    }

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

            // already occupied
            if(!gcell.walkable)
            {
                unit->ShowWarning(mSM->GetCString("WARN_CELL_NOT_VALID"), 3.f);
                return false;
            }
        }
    }

    return true;
}

void GameMap::StartBuildStructure(const Cell2D & cell, Player * player, GameObjectTypeId st)
{
    const ObjectData & data = GetObjectData(st);

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

    GameObject * obj = CreateObject(REGULAR_OBJECTS, st, 0, player->GetFaction(), cell.row, cell.col, true);

    // update player
    const int numCells = obj->GetRows() * obj->GetCols();
    player->SumCells(numCells);

    // propagate effects of conquest
    for(int r = obj->GetRow1(); r <= obj->GetRow0(); ++r)
    {
        for(int c = obj->GetCol1(); c <= obj->GetCol0(); ++c)
        {
            UpdateInfluencedCells(r, c);
        }
    }

    UpdateLinkedCells(player);

    // update surrounding walls if building an object that can connect
    if(obj->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER || obj->GetObjectType() == ObjectData::TYPE_WALL_GATE)
        UpdateWalls(cell);

    // update visibility map
    AddPlayerCellVisibility(gcell, player);

    if(stolen)
        DelPlayerCellVisibility(gcell, prevOwner);

    ApplyLocalVisibility();
}

bool GameMap::HasResourcesToBuildWall(Unit * unit, unsigned int level)
{
    // check if unit has enough energy
    if(!unit->HasEnergyForActionStep(BUILD_WALL))
        return false;

    // check if player has enough resources
    const int costMat = Wall::GetCostMaterial(level);
    const int costEne = Wall::GetCostEnergy(level);

    Player * player = mGame->GetPlayerByFaction(unit->GetFaction());

    return player->HasEnough(Player::Stat::MATERIAL, costMat)  &&
           player->HasEnough(Player::Stat::ENERGY, costEne);
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

    // cell is full
    if(!gcell.walkable)
        return false;

    return true;
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

    // propagate effects of conquest
    UpdateInfluencedCells(cell.row, cell.col);

    // add object wall
    CreateObject(REGULAR_OBJECTS, ObjectData::TYPE_WALL, planned,
                 player->GetFaction(), cell.row, cell.col, true);

    UpdateLinkedCells(player);

    // update minimap
    if(IsCellVisibleToLocalPlayer(cell.row, cell.col))
    {
        const ObjectData & data = GetObjectData(ObjectData::TYPE_WALL);

        const PlayerFaction faction = player->GetFaction();
        const auto type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);
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

    // unit can't conquer
    if(!unit->CanConquer())
    {
        unit->ShowWarning(mSM->GetCString("WARN_CANT_CONQUER"), 2.f);
        return false;
    }

    // unit doesn't have enough energy
    if(!unit->HasEnergyForActionStep(CONQUER_STRUCTURE))
    {
        unit->ShowWarning(mSM->GetCString("WARN_NO_ENE"), 2.f);
        return false;
    }

    const int ind1 = r1 * mCols + c1;
    GameMapCell & gcell1 = mCells[ind1];

    // end is empty
    if(nullptr == gcell1.objTop)
        return false;

    // target object can't be conquered
    if(!gcell1.objTop->CanBeConquered())
    {
       unit->ShowWarning(mSM->GetCString("WARN_CANT_BE_CONQUERED"), 3.f);
        return false;
    }

    // player already owns the structure
    if(gcell1.objTop->GetFaction() == player->GetFaction())
        return false;

    return true;
}

void GameMap::StartConquerStructure(const Cell2D & end, Player * player)
{
    // take player's energy
    player->SumResource(Player::Stat::ENERGY, -COST_CONQUEST_RES_GEN);
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

            UpdateInfluencedCells(r, c);
        }
    }

    Player * prevOwner = mGame->GetPlayerByFaction(obj->GetFaction());

    // assign owner to object
    obj->SetOwner(player);

    // update player
    const int numCells = obj->GetRows() * obj->GetCols();
    player->SumCells(numCells);

    // update tracking of structures and resource generators
    if(obj->IsStructure())
    {
        auto st = static_cast<Structure *>(obj);

        player->AddStructure(st);

        if(prevOwner)
            prevOwner->RemoveStructure(st);
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
                p->SetResource(Player::Stat::MONEY, money.GetValue() * mult);
            }
            break;

            case Temple::EXP_REW_MAX_RES_ENE_MAT:
            {
                p->SetResource(Player::Stat::ENERGY, energy.GetMax());
                p->SetResource(Player::Stat::MATERIAL, material.GetMax());
            }
            break;

            case Temple::EXP_REW_MAX_RES_BLO_DIA:
            {
                p->SetResource(Player::Stat::BLOBS, blobs.GetMax());
                p->SetResource(Player::Stat::DIAMONDS, diamonds.GetMax());
            }
            break;

            case Temple::EXP_REW_MAX_RESOURCES:
            {
                p->SetResource(Player::Stat::ENERGY, energy.GetMax());
                p->SetResource(Player::Stat::MATERIAL, material.GetMax());
                p->SetResource(Player::Stat::BLOBS, blobs.GetMax());
                p->SetResource(Player::Stat::DIAMONDS, diamonds.GetMax());
            }
            break;

            // INCREASE PRODUCTION
            case Temple::EXP_REW_INC_ENERGY_PRODUCTION:
            {
                const float mult = 2.f;

                for(GameObject * o : mObjects)
                {
                    if(o->GetFaction() == faction &&
                       o->GetObjectType() == ObjectData::TYPE_RES_GEN_ENERGY)
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
                       o->GetObjectType() == ObjectData::TYPE_RES_GEN_MATERIAL)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

            // MAXIMIZE COLLECTIBLES
            case Temple::EXP_REW_MAX_BLOBS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == ObjectData::TYPE_BLOBS)
                        static_cast<Blobs *>(o)->MaximizeUnits();
                }
            }
            break;

            case Temple::EXP_REW_MAX_DIAMONDS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == ObjectData::TYPE_DIAMONDS)
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
                       o->GetObjectType() == ObjectData::TYPE_RES_GEN_ENERGY)
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
                       o->GetObjectType() == ObjectData::TYPE_RES_GEN_MATERIAL)
                        static_cast<ResourceGenerator *>(o)->ScaleOutput(mult);
                }
            }
            break;

                // MINIMIZE COLLECTIBLES
            case Temple::EXP_PUN_MIN_BLOBS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == ObjectData::TYPE_BLOBS)
                        static_cast<Blobs *>(o)->MinimizeUnits();
                }
            }
            break;

            case Temple::EXP_PUN_MIN_DIAMONDS:
            {
                for(GameObject * o : mObjects)
                {
                    if(o->GetObjectType() == ObjectData::TYPE_DIAMONDS)
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
    // already has enough units
    if(player->GetNumUnits() == player->GetMaxUnits())
    {
        gen->ShowWarning(mSM->GetCString("WARN_TM_UNITS"), 2.f);
        return false;
    }

    // check if player has enough resources
    const auto & costs = GetObjectData(ut).GetCosts();

    if(!player->HasEnough(Player::Stat::ENERGY, costs[RES_ENERGY]) ||
       !player->HasEnough(Player::Stat::MATERIAL, costs[RES_MATERIAL1]) ||
       !player->HasEnough(Player::Stat::DIAMONDS, costs[RES_DIAMONDS]) ||
       !player->HasEnough(Player::Stat::BLOBS, costs[RES_BLOBS]))
    {
        gen->ShowWarning(mSM->GetCString("WARN_LACK_RES"), 2.f);
        return false;
    }

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
    gen->ShowWarning(mSM->GetCString("WARN_CANT_CELL"), 3.f);
    return false;
}

Cell2D GameMap::GetNewUnitDestination(GameObject * gen) const
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

void GameMap::StartCreateUnit(GameObjectTypeId ut, GameObject * gen,
                              const Cell2D & dest, Player * player)
{
    const int ind = dest.row * mCols + dest.col;
    GameMapCell & gcell = mCells[ind];

    // make player pay
    const auto & costs = GetObjectData(ut).GetCosts();

    player->SumResource(Player::Stat::ENERGY, -costs[RES_ENERGY]);
    player->SumResource(Player::Stat::MATERIAL, -costs[RES_MATERIAL1]);
    player->SumResource(Player::Stat::DIAMONDS, -costs[RES_DIAMONDS]);
    player->SumResource(Player::Stat::BLOBS, -costs[RES_BLOBS]);
}

Unit * GameMap::CreateUnit(GameObjectTypeId ut, const Cell2D & dest, Player * player)
{
    const unsigned int r = static_cast<unsigned int>(dest.row);
    const unsigned int c = static_cast<unsigned int>(dest.col);

    const int ind = r * mCols + c;
    GameMapCell & gcell = mCells[ind];

    const ObjectData & data = GetObjectData(ut);

    // data to pass to new object
    const ObjectInitData initData(mGame, this, mScreenGame->GetParticlesManager(),
                                  player, mScreenGame);

    Unit * unit = new Unit(data, initData);
    unit->SetCell(&mCells[ind]);

    // weapon
    AssignWeaponToObject(data.GetWeapon(), unit);

    // update cell
    gcell.objTop = unit;
    gcell.walkable = false;

    mIsoMap->GetLayer(REGULAR_OBJECTS)->AddObject(unit->GetIsoObject(), r, c);

    // store unit in map list and in registry
    mObjects.push_back(unit);
    mObjectsSet.insert(unit);

    // update player
    player->AddUnit(unit);

    // update visibility map
    AddPlayerObjVisibility(unit, player);
    ApplyLocalVisibility();

    return unit;
}

bool GameMap::CanCreateMiniUnit(GameObjectTypeId ut, GameObject * gen, int elements, Player * player)
{
    // check if player has enough resources
    const auto & costs = GetObjectData(ut).GetCosts();

    if(!player->HasEnough(Player::Stat::ENERGY, costs[RES_ENERGY] * elements) ||
       !player->HasEnough(Player::Stat::MATERIAL, costs[RES_MATERIAL1] * elements) ||
       !player->HasEnough(Player::Stat::DIAMONDS, costs[RES_DIAMONDS] * elements) ||
       !player->HasEnough(Player::Stat::BLOBS, costs[RES_BLOBS] * elements))
    {
        gen->ShowWarning(mSM->GetCString("WARN_LACK_RES"), 2.f);
        return false;
    }

    if(!gen->HasEnergyForActionStep(GameObjectActionType::SPAWN))
    {
        gen->ShowWarning(mSM->GetCString("WARN_NO_ENE"), 2.f);
        return false;
    }

    return true;
}

GameObject * GameMap::CreateMiniUnit(GameObjectTypeId ut, GameObject * gen, const Cell2D & dest,
                                     int elements, Player * player)
{
    const int ind = dest.row * mCols + dest.col;
    GameMapCell & gcell = mCells[ind];

    const ObjectData & data = GetObjectData(ut);

    // data to pass to new object
    const ObjectInitData initData(mGame, this, mScreenGame->GetParticlesManager(),
                                  player, mScreenGame);

    // pay costs
    const auto & costs = data.GetCosts();

    player->SumResource(Player::Stat::ENERGY, -costs[RES_ENERGY] * elements);
    player->SumResource(Player::Stat::MATERIAL, -costs[RES_MATERIAL1] * elements);
    player->SumResource(Player::Stat::DIAMONDS, -costs[RES_DIAMONDS] * elements);
    player->SumResource(Player::Stat::BLOBS, -costs[RES_BLOBS] * elements);

    // create object
    auto mu = new MiniUnit(data, initData, elements);
    mu->SetCell(&mCells[ind]);

    // weapon
    AssignWeaponToObject(data.GetWeapon(), mu);

    // update cell
    gcell.objTop = mu;
    gcell.walkable = false;

    mIsoMap->GetLayer(REGULAR_OBJECTS)->AddObject(mu->GetIsoObject(), dest.row, dest.col);

    // store unit in map list and in registry
    mObjects.push_back(mu);
    mObjectsSet.insert(mu);

    // update visibility map
    AddPlayerObjVisibility(mu, player);
    ApplyLocalVisibility();

    return mu;
}

Cell2D GameMap::GetNewMiniUnitDestination(const Cell2D & genCell) const
{
    // TODO quick code that will need to be improved later
    const int maxDist = 2;

    for(int d = 1; d <= maxDist; ++d)
    {
        const int r0 = (d < genCell.row) ? genCell.row - d : 0;
        const int r1uc = genCell.row + d;
        const int r1 = r1uc < mRows ? r1uc + 1 : mRows;

        for(int r = r0; r < r1; ++r)
        {
            const int c0 = (d < genCell.col) ? genCell.col - d : 0;
            const int c1uc = genCell.col + d;
            const int c1 = c1uc < mCols ? c1uc + 1 : mCols;

            for(int c = c0; c < c1; ++c)
            {
                if(mCells[r * mCols + c].walkable)
                    return Cell2D(r, c);
            }
        }
    }

    return Cell2D(-1, -1);
}

MiniUnitsGroup * GameMap::CreateMiniUnitsGroup(PlayerFaction faction)
{
    auto group = new MiniUnitsGroup(faction);

    mMiniUnitsGroups.emplace_back(group);

    return group;
}

void GameMap::DamageArea(const Cell2D & srcBR, const Cell2D & srcTL, int radius, float maxDamage)
{
    const bool fatal = false;

    for(int rad = 1; rad <= radius; ++rad)
    {
        const float damage = std::roundf(maxDamage / rad);

        // ALONG COLS FROM LEFT TO RIGHT
        const int c0UC = srcTL.col - rad;
        const int c0 = (c0UC > 0) ? c0UC : 0;
        const int c1UC = srcBR.col + rad;
        const int c1 = (c1UC <= mCols) ? c1UC : mCols;

        // TOP
        const int tRow = srcTL.row - rad;

        if(tRow >= 0)
        {
            const int ind0 = tRow * mCols;

            for(int c = c0; c < c1; ++c)
            {
                const int ind = ind0 + c;

                if(mCells[ind].objTop != nullptr && !mCells[ind].objTop->IsDestroyed())
                    mCells[ind].objTop->Hit(damage, nullptr, fatal);

                if(mCells[ind].objBottom != nullptr && !mCells[ind].objBottom->IsDestroyed())
                    mCells[ind].objBottom->Hit(damage, nullptr, fatal);
            }
        }

        // BOTTOM
        const int bRow = srcBR.row + rad;

        if(bRow < mRows)
        {
            const int ind0 = bRow * mCols;

            for(int c = c0; c < c1; ++c)
            {
                const int ind = ind0 + c;

                if(mCells[ind].objTop != nullptr && !mCells[ind].objTop->IsDestroyed())
                    mCells[ind].objTop->Hit(damage, nullptr, fatal);

                if(mCells[ind].objBottom != nullptr && !mCells[ind].objBottom->IsDestroyed())
                    mCells[ind].objBottom->Hit(damage, nullptr, fatal);
            }
        }

        // ALONG ROWS FROM TOP TO BOTTOM
        const int r0UC = srcTL.row - rad + 1;
        const int r0 = (r0UC > 0) ? r0UC : 0;
        const int r1UC = srcBR.row + rad;
        const int r1 = (r1UC <= mRows) ? r1UC : mRows;

        // LEFT
        const int lCol = srcTL.col - rad;

        if(lCol >= 0)
        {
            for(int r = r0; r < r1; ++r)
            {
                const int ind = r * mCols + lCol;

                if(mCells[ind].objTop != nullptr && !mCells[ind].objTop->IsDestroyed())
                    mCells[ind].objTop->Hit(damage, nullptr, fatal);

                if(mCells[ind].objBottom != nullptr && !mCells[ind].objBottom->IsDestroyed())
                    mCells[ind].objBottom->Hit(damage, nullptr, fatal);
            }
        }

        // RIGHT
        const int rCol = srcBR.col + rad;

        if(rCol < mCols)
        {
            for(int r = r0; r < r1; ++r)
            {
                const int ind = r * mCols + rCol;

                if(mCells[ind].objTop != nullptr && !mCells[ind].objTop->IsDestroyed())
                    mCells[ind].objTop->Hit(damage, nullptr, fatal);

                if(mCells[ind].objBottom != nullptr && !mCells[ind].objBottom->IsDestroyed())
                    mCells[ind].objBottom->Hit(damage, nullptr, fatal);
            }
        }
    }
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
    if(path->Start())
    {
        mPathsToAdd.emplace_back(path);
        return true;
    }
    else
        return false;
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

Cell2D GameMap::GetAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL,
                                      const Cell2D & targetBR) const
{
    const int tRows = targetBR.row - targetTL.row + 1;
    const int tCols = targetBR.col - targetTL.col + 1;

    std::vector<Cell2D> walkalbes;
    const int maxWalkables = (tCols + tRows + 2 ) * 2;
    walkalbes.reserve(maxWalkables);

    const int colL = targetTL.col > 0 ? targetTL.col - 1 : 0;
    const int colR = (targetBR.col + 2) < mCols ? targetBR.col + 2 : mCols;
    const int rowB = targetBR.row + 1;

    // TOP CELLS
    if(targetTL.row > 0)
    {
        const int r = targetTL.row - 1;
        const int ind0 = r * mCols;

        for(int c = colL; c < colR; ++c)
        {
            const int ind = ind0 + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // RIGHT CELLS
    if((targetBR.col + 1) < mCols)
    {
        const int c = targetBR.col + 1;

        for(int r = targetTL.row; r < rowB; ++r)
        {
            const int ind = (r * mCols) + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // BOTTOM CELLS
    if((targetBR.row + 1) < mRows)
    {
        const int r = targetBR.row + 1;
        const int ind0 = r * mCols;

        for(int c = colL; c < colR; ++c)
        {
            const int ind = ind0 + c;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, c);
        }
    }

    // LEFT CELLS
    if(targetTL.col > 0)
    {
        for(int r = targetTL.row; r < rowB; ++r)
        {
            const int ind = (r * mCols) + colL;

            if(mCells[ind].walkable)
                walkalbes.emplace_back(r, colL);
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

Cell2D GameMap::GetOrthoAdjacentMoveTarget(const Cell2D & start, const Cell2D & targetTL,
                                           const Cell2D & targetBR) const
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
    const int dist = ceilf(u->GetWeapon()->GetRange() * 0.5f);

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
    const int R1 = obj1->GetRows() > 1 ? (obj1->GetRow0() + obj1->GetRow1()) / 2 : obj1->GetRow0();
    const int C1 = obj1->GetCols() > 1 ? (obj1->GetCol0() + obj1->GetCol1()) / 2 : obj1->GetCol0();

    const int R2 = obj2->GetRows() > 1 ? (obj2->GetRow0() + obj2->GetRow1()) / 2 : obj2->GetRow0();
    const int C2 = obj2->GetCols() > 1 ? (obj2->GetCol0() + obj2->GetCol1()) / 2 : obj2->GetCol0();

    return std::abs(R2 - R1) + std::abs(C2 - C1);
}

int GameMap::Distance(const GameObject * obj1, const GameObject * obj2) const
{
    int distR = mRows;
    int distC = mCols;

    const int obj1RowBR = obj1->GetRow0();
    const int obj1ColBR = obj1->GetCol0();
    const int obj1RowTL = obj1->GetRow1();
    const int obj1ColTL = obj1->GetCol1();

    const int obj2RowBR = obj2->GetRow0();
    const int obj2ColBR = obj2->GetCol0();
    const int obj2RowTL = obj2->GetRow1();
    const int obj2ColTL = obj2->GetCol1();

    // BR -> BR
    const int distR1 = std::abs(obj1RowBR - obj2RowBR);
    const int distC1 = std::abs(obj1ColBR - obj2ColBR);

    if(distR1 < distR)
        distR = distR1;

    if(distC1 < distC)
        distC = distC1;


    // BR -> TL
    const int distR2 = std::abs(obj1RowBR - obj2RowTL);
    const int distC2 = std::abs(obj1ColBR - obj2ColTL);

    if(distR2 < distR)
        distR = distR2;

    if(distC2 < distC)
        distC = distC2;

    // TL -> BR
    const int distR3 = std::abs(obj1RowTL - obj2RowBR);
    const int distC3 = std::abs(obj1ColTL - obj2ColBR);

    if(distR3 < distR)
        distR = distR3;

    if(distC3 < distC)
        distC = distC3;

    // TL -> TL
    const int distR4 = std::abs(obj1RowTL - obj2RowTL);
    const int distC4 = std::abs(obj1ColTL - obj2ColTL);

    if(distR4 < distR)
        distR = distR4;

    if(distC4 < distC)
        distC = distC4;

    return distR + distC;
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

bool GameMap::FindFreeArea(const Cell2D & start, int rows, int cols, int maxRadius, Cell2D & target)
{
    const int r0 = start.row;
    const int c0 = start.col;

    const int maxDist = mRows * mCols;
    int minDist = maxDist;

    int radius = 1;

    target.row = -1;
    target.col = -1;

    while(radius < maxRadius)
    {
        const int tlR = (r0 - radius) > 0 ? (r0 - radius) : 0;
        const int tlC = (c0 - radius) > 0 ? (c0 - radius) : 0;
        const int brR = (r0 + radius) < (mRows - 1) ? r0 + radius : (mRows - 1);
        const int brC = (c0 + radius) < (mCols - 1) ? c0 + radius : (mCols - 1);

        // TOP
        {
            const int r = tlR;

            for(int c = tlC; c <= brC; ++c)
            {
                if(IsAreaFree(r, c, rows, cols))
                {
                    const Cell2D t(r, c);
                    const int dist = ApproxDistance(start, t);

                    if(dist < minDist)
                    {
                        minDist = dist;
                        target = t;
                    }
                }
            }
        }

        // BOTTOM
        {
            const int r = brR;

            for(int c = tlC; c <= brC; ++c)
            {
                if(IsAreaFree(r, c, rows, cols))
                {
                    const Cell2D t(r, c);
                    const int dist = ApproxDistance(start, t);

                    if(dist < minDist)
                    {
                        minDist = dist;
                        target = t;
                    }
                }
            }
        }

        // LEFT
        {
            const int c = tlC;

            for(int r = tlR + 1; r < brR; ++r)
            {
                if(IsAreaFree(r, c, rows, cols))
                {
                    const Cell2D t(r, c);
                    const int dist = ApproxDistance(start, t);

                    if(dist < minDist)
                    {
                        minDist = dist;
                        target = t;
                    }
                }
            }
        }

        // RIGHT
        {
            const int c = brC;

            for(int r = tlR + 1; r < brR; ++r)
            {
                if(IsAreaFree(r, c, rows, cols))
                {
                    const Cell2D t(r, c);
                    const int dist = ApproxDistance(start, t);

                    if(dist < minDist)
                    {
                        minDist = dist;
                        target = t;
                    }
                }
            }
        }

        if(target.row != -1)
            return true;

        ++radius;
    }

    return false;
}

bool GameMap::IsAreaFree(int brR, int brC, int rows, int cols)
{
    const int tlR = brR - rows + 1;
    const int tlC = brC - cols + 1;

    // area goes outside map
    if(rows > brR || cols > brC || tlR < 0 || tlC < 0)
        return false;

    for(int r = tlR; r <= brR; ++r)
    {
        const int ind0 = r * mCols;

        for(int c = tlC; c <= brC; ++c)
        {
            const int ind = ind0 + c;
            const GameMapCell & cell = mCells[ind];

            if(cell.objTop != nullptr || cell.objBottom != nullptr)
                return false;
        }
    }

    return true;
}

void GameMap::OnNewTurn(PlayerFaction faction)
{
    // notify all objects
    for(GameObject * obj : mObjects)
        obj->OnNewTurn(faction);

    // notify all generators
    for(CollectableGenerator * cg : mCollGen)
        cg->OnNewTurn();

    // select groups of mini units to move
    DeleteEmptyMiniUnitsGroups();
    InitMiniUnitsGroupsReadyToMove(faction);
}

int GameMap::GetFactionMoneyPerTurn(PlayerFaction faction)
{
    const int perc = mControlMap->GetPercentageControlledByFaction(faction);
    const int maxMoney = 1000 / 100;

    return maxMoney * perc;
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

            // apply damage to surrounding area based on onject's energy and size
            const int damageRadius = obj->GetRows();
            const float maxDamage = obj->GetEnergy();
            DamageArea(Cell2D(obj->GetRow0(), obj->GetCol0()), Cell2D(obj->GetRow1(), obj->GetCol1()),
                       damageRadius, maxDamage);

            DestroyObjectPaths(obj);

            Player * p = mGame->GetPlayerByFaction(obj->GetFaction());

            if(p != nullptr && p->IsAI())
                p->GetAI()->HandleObjectDestroyed(obj);

            // erase object from vector and set
            itObj = mObjects.erase(itObj);
            mObjectsSet.erase(obj);

            DestroyObject(obj);

            auto player = sgl::media::AudioManager::Instance()->GetPlayer();
            player->PlaySound("game/explosion-01.ogg");
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

    // mini units attacking
    UpdateMiniUnitsAttacking(delta);

    // structures attacking
    UpdateStructuresAttacking(delta);
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
        int r = currCell.row - 1;

        if(r >= 0)
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
        int c = currCell.col - 1;

        if(c >= 0)
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
        // linked status changed
        if(obj->IsLinked() != objsLink[obj])
        {
            obj->SetLinked(objsLink[obj]);

            if(obj->IsLinked())
            {
                // add control points of new linked object
                mControlMap->AddControlPointsForObject(obj);

                // mark linked radar
                if(obj->GetObjectType() == ObjectData::TYPE_RADAR_STATION)
                    radarLinked = true;
            }
        }
        // linked status unchanged
        else
        {
            if(obj->IsLinked() && obj->GetObjectType() == ObjectData::TYPE_RADAR_STATION)
                radarLinked = true;
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

    obj->OnPositionChanged();

    // remove object from current cell
    mCells[ind0].objTop = nullptr;
    mCells[ind0].walkable = true;

    // add object to new cell
    const int ind1 = row * mCols + col;

    obj->SetCell(&mCells[ind1]);

    mCells[ind1].objTop = obj;
    mCells[ind1].walkable = false;

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

    o2a.obj->OnPositionChanged();

    // apply visibility
    ApplyLocalVisibilityToObject(o2a.obj);
}

void GameMap::DestroyObject(GameObject * obj)
{
    mScreenGame->OnObjectDestroyed(obj);

    Player * owner = mGame->GetPlayerByFaction(obj->GetFaction());

    if(owner != nullptr)
    {
        Player * localPlayer = mGame->GetLocalPlayer();

        // owner is local Player
        if(owner == localPlayer)
        {
            // update visibility map
            // NOTE only local player for now
            DelPlayerObjVisibility(obj, localPlayer);
        }

        // remove unit from player
        if(obj->GetObjectCategory() == ObjectData::CAT_UNIT)
            owner->RemoveUnit(static_cast<Unit *>(obj));
        // remove structure
        else if(obj->IsStructure())
            owner->RemoveStructure(static_cast<Structure *>(obj));
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

            if(cell.objTop == obj)
                cell.objTop = nullptr;
            else if(cell.objBottom == obj)
                cell.objBottom = nullptr;

            // update cell image
            UpdateCellType(ind, cell);
        }
    }

    // remove iso object from layer
    IsoObject * isoObj = obj->GetIsoObject();
    IsoLayer * layer = isoObj->GetLayer();
    layer->ClearObject(isoObj);

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
    if(obj->GetObjectCategory() != ObjectData::CAT_UNIT)
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
    // merge object actions to do with object actions list
    mPaths.insert(mPaths.end(), mPathsToAdd.begin(), mPathsToAdd.end());
    mPathsToAdd.clear();

    // now process all paths
    auto itPath = mPaths.begin();

    while(itPath != mPaths.end())
    {
        ObjectPath * path = *itPath;

        path->Update(delta);

        if(path->IsTerminated())
        {
            // AUTO MOVING MINI UNITS -> continue if possible
            if(path->GetObject()->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
                ContinueMiniUnitGroupMove(path);

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
    if(nullptr == obj || (obj->GetObjectType() != ObjectData::TYPE_WALL &&
                          obj->GetObjectType() != ObjectData::TYPE_WALL_GATE))
        return ;

    const GameObject * objN = (cell.row - 1 >= 0) ? GetCell(cell.row - 1, cell.col).objTop : nullptr;
    const bool wallN = objN && (objN->GetObjectType() == ObjectData::TYPE_WALL ||
                                objN->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER ||
                                objN->GetObjectType() == ObjectData::TYPE_WALL_GATE);

    const GameObject * objS = (cell.row + 1 < static_cast<int>(mRows)) ?
                              GetCell(cell.row + 1, cell.col).objTop : nullptr;
    const bool wallS = objS && (objS->GetObjectType() == ObjectData::TYPE_WALL ||
                                objS->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER ||
                                objS->GetObjectType() == ObjectData::TYPE_WALL_GATE);

    const GameObject * objW = (cell.col - 1 >= 0) ? GetCell(cell.row, cell.col - 1).objTop : nullptr;
    const bool wallW = objW && (objW->GetObjectType() == ObjectData::TYPE_WALL ||
                                objW->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER ||
                                objW->GetObjectType() == ObjectData::TYPE_WALL_GATE);

    const GameObject * objE = (cell.col + 1 < static_cast<int>(mCols)) ?
                              GetCell(cell.row, cell.col + 1).objTop : nullptr;
    const bool wallE = objE && (objE->GetObjectType() == ObjectData::TYPE_WALL ||
                                objE->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER ||
                                objE->GetObjectType() == ObjectData::TYPE_WALL_GATE);

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

    if(obj->GetObjectType() == ObjectData::TYPE_WALL)
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
    else if(obj->GetObjectType() == ObjectData::TYPE_WALL_GATE)
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

const WeaponData & GameMap::GetWeaponData(WeaponType t) const
{
    const ObjectsDataRegistry * objReg = mGame->GetObjectsRegistry();
    return objReg->GetWeaponData(t);
}

void GameMap::AssignWeaponToObject(WeaponType wt, GameObject * obj)
{
    if(wt == WeaponData::TYPE_NULL)
        return ;

    auto pm = mScreenGame->GetParticlesManager();

    const WeaponData & wData = GetWeaponData(wt);
    Weapon * weapon = nullptr;

    if(wData.GetClass() == WEAPONC_LASER)
        weapon = new Laser(wData, obj, mGame, this, pm);

    obj->SetWeapon(weapon);
}

void GameMap::DeleteEmptyMiniUnitsGroups()
{
    auto it = mMiniUnitsGroups.begin();

    while(it != mMiniUnitsGroups.end())
    {
        auto group = *it;

        // empty group -> destroy
        if(group->IsEmpty())
        {
            delete group;

            it = mMiniUnitsGroups.erase(it);
        }
        else
            ++it;
    }
}

void GameMap::InitMiniUnitsGroupsReadyToMove(PlayerFaction faction)
{
    // populate list of groups to move
    for(auto g : mMiniUnitsGroups)
    {
        if(g->GetFaction() == faction && g->HasPathSet())
            mMiniUnitsGroupsToMove.emplace_back(g);
    }

    // start to move
    if(!mMiniUnitsGroupsToMove.empty())
    {
        mScreenGame->SetLocalTurnStage(TURN_STAGE_MINI_UNITS_MOVE);

        SetNextMiniUnitsGroupToMove();
    }
    // no mini units need to move -> check for attack
    else
        InitMiniUnitsReadyToAttack(faction);
}

void GameMap::SetNextMiniUnitsGroupToMove()
{
    while(!mMiniUnitsGroupsToMove.empty())
    {
        if(StartMiniUnitGroupMove())
            return ;
        // init failed -> clear group from the list and try next
        else
            ClearMiniUnitsGroupMoveFailed();
    }
}

bool GameMap::StartMiniUnitGroupMove()
{
    auto group = mMiniUnitsGroupsToMove.back();

    const Cell2D & target = group->GetPathTarget();

    // find shortest path to destination checking all MiniUnits in group
    std::vector<unsigned int> path;
    GameObject * obj = nullptr;

    group->DoForAll([this, target, &obj, &path](GameObject * o)
    {
        // skip mini units that have already reached target
        if(!static_cast<MiniUnit *>(o)->IsMoving())
            return ;

        // flag all mini units that need to move this turn
        o->SetActiveAction(GameObjectActionType::MOVE);

        const auto p = mPathfinder->MakePath(o->GetRow0(), o->GetCol0(), target.row, target.col,
                                             sgl::ai::Pathfinder::INCLUDE_START);

        if(path.empty() || (!p.empty() && p.size() < path.size()))
        {
            obj = o;
            path = std::move(p);
        }
    });

    // can't find a valid path to target -> cancel it
    if(path.empty())
        return false;

    // path found -> start the move of the leader (first) mini unit
    auto op = new ObjectPath(obj, mIsoMap, this, mScreenGame);
    op->SetPath(path);

    if(!MoveUnit(op))
    {
        delete op;
        return false;
    }
    else
        return true;
}

void GameMap::ContinueMiniUnitGroupMove(const ObjectPath * prevOP)
{
    const unsigned int lastStep = prevOP->GetLastStepDone();

    // previous mini unit didn't move at all -> error
    if(0 == lastStep)
    {
        ClearMiniUnitsGroupMoveFailed();
        SetNextMiniUnitsGroupToMove();

        return ;
    }

    auto prevMU = static_cast<MiniUnit *>(prevOP->GetObject());
    auto group = mMiniUnitsGroupsToMove.back();

    // mark mini unit moved for this turn
    prevMU->SetCurrentAction(GameObjectActionType::IDLE);
    prevMU->SetActiveAction(GameObjectActionType::IDLE);

    Cell2D target;

    // previous MiniUnit reached group target
    if(prevMU->GetRow0() == group->GetPathTarget().row &&
       prevMU->GetCol0() == group->GetPathTarget().col)
    {
        // mark mini unit done
        prevMU->SetMoving(false);
        prevMU->setTargetReached();

        // move group target back to path[n-1] and clear if this fails (it shouldn't)
        if(!group->PopPathTargetBack(mCols))
        {
            ClearMiniUnitsGroupMoveFailed();
            SetNextMiniUnitsGroupToMove();

            return ;
        }

        // next target is new target
        target = group->GetPathTarget();
    }
    // group target not reached yet
    else
    {
        // next target is step before previous mini unit
        const std::vector<unsigned int> & prevPath = prevOP->GetPath();
        const unsigned int targetInd = prevPath[lastStep - 1];
        target.row = targetInd / mCols;
        target.col = targetInd % mCols;
    }

    std::vector<unsigned int> path;
    GameObject * obj = nullptr;
    int done = 0;
    int moved = 0;

    group->DoForAll([this, target, &obj, &path, &done, &moved](GameObject * o)
    {
        // already moved this turn
        if(o->GetActiveAction() == GameObjectActionType::IDLE)
        {
            ++moved;

            // target reached
            if(!static_cast<MiniUnit *>(o)->IsMoving())
                ++done;

            return ;
        }

        const auto p = mPathfinder->MakePath(o->GetRow0(), o->GetCol0(), target.row, target.col,
                                             sgl::ai::Pathfinder::INCLUDE_START);

        if(path.empty() || (!p.empty() && p.size() < path.size()))
        {
            obj = o;
            path = std::move(p);
        }
    });

    // moved all mini units of group for this turn
    if(moved == group->GetNumObjects())
    {
        ClearMiniUnitsGroupMoveCompleted(done == moved);
        SetNextMiniUnitsGroupToMove();

        return ;
    }

    // can't find a valid path to target -> failed
    if(path.empty())
    {
        ClearMiniUnitsGroupMoveFailed();
        SetNextMiniUnitsGroupToMove();

        return ;
    }

    group->SetPath(std::move(path));

    // path found -> start move
    auto op = new ObjectPath(obj, mIsoMap, this, mScreenGame);
    op->SetPath(path);

    if(!MoveUnit(op))
    {
        delete op;
        ClearMiniUnitsGroupMoveFailed();
        SetNextMiniUnitsGroupToMove();
    }
}

void GameMap::ClearMiniUnitsGroupMoveCompleted(bool finished)
{
    auto group = mMiniUnitsGroupsToMove.back();

    // finished completely -> clear group path
    if(finished)
        group->ClearPath();
    // all moved for this turn, but not all reached target yet -> update group path
    else
    {
        std::vector<unsigned int> path;
        const Cell2D & target = group->GetPathTarget();

        group->DoForAll([this, target, &path](GameObject * o)
        {
            const auto p = mPathfinder->MakePath(o->GetRow0(), o->GetCol0(), target.row, target.col,
                                                 sgl::ai::Pathfinder::NO_OPTION);

            if(path.empty() || (!p.empty() && p.size() < path.size()))
                path = std::move(p);
        });

        if(!path.empty())
            group->SetPath(std::move(path));
    }

    ClearMovingMiniUnitsGroup();
}

// NOTE this assumes that the current group moving is last one in the list
void GameMap::ClearMiniUnitsGroupMoveFailed()
{
    // list empty -> nothing to do
    if(mMiniUnitsGroupsToMove.empty())
        return ;

    auto group = mMiniUnitsGroupsToMove.back();

    group->ClearPath();

    // reset mini units action
    group->DoForAll([](GameObject * o)
    {
        static_cast<MiniUnit *>(o)->SetMoving(false);
        o->SetActiveAction(GameObjectActionType::IDLE);
        o->SetCurrentAction(GameObjectActionType::IDLE);
    });

    ClearMovingMiniUnitsGroup();
}

void GameMap::ClearMovingMiniUnitsGroup()
{
    const PlayerFaction faction = mMiniUnitsGroupsToMove.back()->GetFaction();

    // clear element from list
    mMiniUnitsGroupsToMove.pop_back();

    // move done -> next step, make mini units with weapon attack
    if(mMiniUnitsGroupsToMove.empty())
        InitMiniUnitsReadyToAttack(faction);
}

void GameMap::InitMiniUnitsReadyToAttack(PlayerFaction faction)
{
    // populate list of groups to move
    for(auto g : mMiniUnitsGroups)
    {
        if(g->GetFaction() == faction && g->CanAttack())
        {
            g->DoForAll([this](GameObject * o)
            {
                if(o->HasEnemyInRange())
                {
                    o->SetActiveAction(GameObjectActionType::ATTACK);
                    mMiniUnitsAttacking.emplace_back(static_cast<MiniUnit *>(o));
                }
            });
        }
    }

    mTimerAutoAttacking = 0.f;

    // nothing to do -> end here
    if(mMiniUnitsAttacking.empty())
        InitStructuresReadyToAttack();
    else
        mScreenGame->SetLocalTurnStage(TURN_STAGE_MINI_UNITS_ATTACK);
}

void GameMap::UpdateMiniUnitsAttacking(float delta)
{
    // empty queue -> nothing to do
    if(mMiniUnitsAttacking.empty())
        return ;

    mTimerAutoAttacking -= delta;

    // attack delay -> exit
    if(mTimerAutoAttacking > 0.f)
        return ;

    MiniUnit * mu = mMiniUnitsAttacking.back();

    // mini unit still attacking -> nothing to do
    if(mu->GetWeapon()->HasTarget())
        return ;

    // unit not processed yet -> try to find a target
    if(mu->GetCurrentAction() == GameObjectActionType::IDLE)
        mu->FindAndSetEnemyTarget();

    // target found -> start attack
    if(mu->GetWeapon()->HasTarget())
    {
        mu->SetCurrentAction(GameObjectActionType::ATTACK);
        return ;
    }

    // no target -> clear mini unit from queue
    mu->SetActiveAction(GameObjectActionType::IDLE);
    mu->SetCurrentAction(GameObjectActionType::IDLE);

    mMiniUnitsAttacking.pop_back();

    // reset auto-attack timer to delay next attacker
    mTimerAutoAttacking = timeAutoAttackDelay;

    // no more mini units to check -> finished
    if(mMiniUnitsAttacking.empty())
        InitStructuresReadyToAttack();
}

void GameMap::InitStructuresReadyToAttack()
{
    Player * player = mScreenGame->GetActivePlayer();

    const std::vector<Structure *> & structures = player->GetStructures();

    for(auto s : structures)
    {
        if(s->IsLinked() && s->HasEnemyInRange())
            mStructuresAttacking.emplace_back(s);
    }

    mTimerAutoAttacking = 0.f;

    // nothing to do -> end here
    if(mStructuresAttacking.empty())
        mScreenGame->OnAutomaticMovesFinished();
    else
        mScreenGame->SetLocalTurnStage(TURN_STAGE_TOWERS_ATTACK);
}

void GameMap::UpdateStructuresAttacking(float delta)
{
    // empty queue -> nothing to do
    if(mStructuresAttacking.empty())
        return ;

    mTimerAutoAttacking -= delta;

    // attack delay -> exit
    if(mTimerAutoAttacking > 0.f)
        return ;

    GameObject * obj = mStructuresAttacking.back();

    // object still attacking -> nothing to do
    if(obj->GetWeapon()->HasTarget())
        return ;

    // object not processed yet -> try to find a target
    if(obj->HasEnergyForActionStep(GameObjectActionType::ATTACK))
        obj->FindAndSetEnemyTarget();

    // target found -> start attack
    if(obj->GetWeapon()->HasTarget())
    {
        obj->SetCurrentAction(GameObjectActionType::ATTACK);
        return ;
    }

    // no target -> clear mini unit from queue
    obj->SetActiveAction(GameObjectActionType::IDLE);
    obj->SetCurrentAction(GameObjectActionType::IDLE);

    mStructuresAttacking.pop_back();

    // reset auto-attack timer to delay next attacker
    mTimerAutoAttacking = timeAutoAttackDelay;

    // nothing to do -> end here
    if(mStructuresAttacking.empty())
        mScreenGame->OnAutomaticMovesFinished();
}

} // namespace game
