#include "GameObjects/ObjectsDataRegistry.h"

#include "GameData.h"
#include "GameObject.h"

namespace game
{

ObjectsDataRegistry::ObjectsDataRegistry()
{
    InitObjectData();
    InitFactionData();
}


const ObjectBasicData & ObjectsDataRegistry::GetObjectData(GameObjectTypeId type) const
{
    auto res = mData.find(type);

    if(res != mData.end())
        return res->second;
    else
        return ObjectBasicData::NullObj;
}

const ObjectFactionData & ObjectsDataRegistry::GetFactionData(PlayerFaction f, GameObjectTypeId type) const
{
    auto res = mFactionData[f].find(type);

    if(res != mFactionData[f].end())
        return res->second;
    else
        return ObjectFactionData::NullObj;
}

void ObjectsDataRegistry::InitObjectData()
{
    mData.emplace(GameObject::TYPE_BARRACKS,
                  ObjectBasicData(GameObject::TYPE_BARRACKS, OCS_BARRACKS, OCAT_GENERIC,
                                  SpriteFileStructures, ID_STRUCT_BARRACKS, 2, 2));

    mData.emplace(GameObject::TYPE_BASE,
                  ObjectBasicData(GameObject::TYPE_BASE, OCS_BASE, OCAT_GENERIC,
                                  SpriteFileStructures, ID_STRUCT_BASE_L1, 3, 3));

    mData.emplace(GameObject::TYPE_BASE_SPOT,
                  ObjectBasicData(GameObject::TYPE_BASE_SPOT, OCS_BASE, OCAT_GENERIC,
                                  SpriteFileStructures, ID_STRUCT_BASE_L1, 3, 3));

    mData.emplace(GameObject::TYPE_BLOBS,
                  ObjectBasicData(GameObject::TYPE_BLOBS, OCO_COLLECTABLE, OCAT_UNDEFINED,
                                  SpriteCollectiblesFile, BLOBS_1, 1, 1));

    mData.emplace(GameObject::TYPE_DIAMONDS,
                  ObjectBasicData(GameObject::TYPE_DIAMONDS, OCO_COLLECTABLE, OCAT_UNDEFINED,
                                  SpriteCollectiblesFile, DIAMONDS_1, 1, 1));

    mData.emplace(GameObject::TYPE_HOSPITAL,
                  ObjectBasicData(GameObject::TYPE_HOSPITAL, OCS_HOSPITAL, OCAT_GENERIC,
                                  SpriteFileStructures, ID_STRUCT_HOSPITAL, 2, 2));

    mData.emplace(GameObject::TYPE_DEFENSIVE_TOWER,
                  ObjectBasicData(GameObject::TYPE_DEFENSIVE_TOWER, OCS_TOWER, OCAT_DEFENSE,
                                  SpriteFileStructures, ID_STRUCT_DTOWER_L1, 1, 1));

    mData.emplace(GameObject::TYPE_LOOTBOX,
                  ObjectBasicData(GameObject::TYPE_LOOTBOX, OCO_COLLECTABLE, OCAT_UNDEFINED,
                                  SpriteCollectiblesFile, IND_LOOTBOX, 1, 1));

    mData.emplace(GameObject::TYPE_MOUNTAINS,
                  ObjectBasicData(GameObject::TYPE_MOUNTAINS, OCO_SCENE, OCAT_UNDEFINED,
                                  SpriteFileSceneElements, ID_SCENE_MOUNTAIN_1, 2, 2));

    mData.emplace(GameObject::TYPE_PRACTICE_TARGET,
                  ObjectBasicData(GameObject::TYPE_PRACTICE_TARGET, OCS_TARGET, OCAT_GENERIC,
                                  SpriteFileStructures, ID_PRACTICE_TARGET, 1, 1));

    mData.emplace(GameObject::TYPE_RADAR_STATION,
                  ObjectBasicData(GameObject::TYPE_RADAR_STATION, OCS_RADAR, OCAT_TECHNOLOGY,
                                  SpriteFileStructures, ID_STRUCT_RADAR, 2, 2));

    mData.emplace(GameObject::TYPE_RADAR_TOWER,
                  ObjectBasicData(GameObject::TYPE_RADAR_STATION, OCS_RADAR, OCAT_TECHNOLOGY,
                                  SpriteFileStructures, ID_STRUCT_RADAR_TOWER, 1, 1));

    mData.emplace(GameObject::TYPE_RES_GEN_ENERGY,
                  ObjectBasicData(GameObject::TYPE_RES_GEN_ENERGY, OCS_GENERATOR, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_GEN_ENERGY, 2, 2));

    mData.emplace(GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
                  ObjectBasicData(GameObject::TYPE_RES_GEN_ENERGY_SOLAR, OCS_GENERATOR, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_SOLAR_PANEL, 1, 1));

    mData.emplace(GameObject::TYPE_RES_GEN_MATERIAL,
                  ObjectBasicData(GameObject::TYPE_RES_GEN_MATERIAL, OCS_GENERATOR, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_GEN_MATERIAL, 2, 2));

    mData.emplace(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT,
                  ObjectBasicData(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT, OCS_GENERATOR, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_MATERIAL_EXTRACTOR, 2, 2));

    mData.emplace(GameObject::TYPE_RES_STORAGE_BLOBS,
                  ObjectBasicData(GameObject::TYPE_RES_STORAGE_BLOBS, OCS_STORAGE, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_STORAGE_BLOBS, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_DIAMONDS,
                  ObjectBasicData(GameObject::TYPE_RES_STORAGE_DIAMONDS, OCS_STORAGE, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_STORAGE_DIAMONDS, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_ENERGY,
                  ObjectBasicData(GameObject::TYPE_RES_STORAGE_ENERGY, OCS_STORAGE, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_STORAGE_ENERGY, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_MATERIAL,
                  ObjectBasicData(GameObject::TYPE_RES_STORAGE_MATERIAL, OCS_STORAGE, OCAT_RESOURCES,
                                  SpriteFileStructures, ID_STRUCT_STORAGE_MATERIAL, 1, 1));

    mData.emplace(GameObject::TYPE_ROCKS,
                  ObjectBasicData(GameObject::TYPE_ROCKS, OCO_SCENE, OCAT_UNDEFINED,
                                  SpriteRocksFile, ROCKS_ROW_1, 1, 1));

    mData.emplace(GameObject::TYPE_TEMPLE,
                  ObjectBasicData(GameObject::TYPE_TEMPLE, OCS_RELICS, OCAT_GENERIC,
                                  SpriteFileStructures, ID_STRUCT_TEMPLE, 3, 3));

    mData.emplace(GameObject::TYPE_TREES,
                  ObjectBasicData(GameObject::TYPE_TREES, OCO_SCENE, OCAT_UNDEFINED,
                                  SpriteFileTrees, TREE1_1T_1, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_MEDIC1,
                  ObjectBasicData(GameObject::TYPE_UNIT_MEDIC1, OCU_MEDIC, OCAT_UNIT,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SCOUT1,
                  ObjectBasicData(GameObject::TYPE_UNIT_SCOUT1, OCU_SOLDIER, OCAT_UNIT,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SOLDIER1,
                  ObjectBasicData(GameObject::TYPE_UNIT_SOLDIER1, OCU_SOLDIER, OCAT_UNIT,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SOLDIER2,
                  ObjectBasicData(GameObject::TYPE_UNIT_SOLDIER2, OCU_SOLDIER, OCAT_UNIT,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_WORKER1,
                  ObjectBasicData(GameObject::TYPE_UNIT_WORKER1, OCU_WORKER, OCAT_UNIT,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_WALL,
                  ObjectBasicData(GameObject::TYPE_WALL, OCS_WALL, OCAT_DEFENSE,
                                  nullptr, 0, 1, 1));

    mData.emplace(GameObject::TYPE_WALL_GATE,
                  ObjectBasicData(GameObject::TYPE_WALL_GATE, OCS_WALL, OCAT_DEFENSE,
                                  nullptr, 0, 1, 1));
}

void ObjectsDataRegistry::InitFactionData()
{
    // ===== FACTION 1 =====
    std::unordered_map<GameObjectTypeId, ObjectFactionData> & data1 = mFactionData[FACTION_1];

    data1.emplace(GameObject::TYPE_BARRACKS,
                  ObjectFactionData(std::vector<int>{ 5, 4, 0, 0, 0, 5, 4, 3 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BARRACKS_F1));

    data1.emplace(GameObject::TYPE_BASE,
                  ObjectFactionData(std::vector<int>{ 6, 6, 5, 4, 5, 5, 6, 6 },
                                    std::vector<int>{ 1000, 1000, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BASE_L1_F1));

    data1.emplace(GameObject::TYPE_DEFENSIVE_TOWER,
                  ObjectFactionData(std::vector<int>{ 5, 4, 4, 4, 5, 5, 5, 5 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_DTOWER_L1_F1));

    data1.emplace(GameObject::TYPE_HOSPITAL,
                  ObjectFactionData(std::vector<int>{ 6, 4, 0, 0, 0, 5, 3, 2 },
                                    std::vector<int>{ 1000, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_HOSPITAL_F1));

    data1.emplace(GameObject::TYPE_PRACTICE_TARGET,
                  ObjectFactionData(std::vector<int>{ 8, 2, 0, 0, 0, 5, 8, 8 },
                                    std::vector<int>{ 50, 50, 0, 0 },
                                    SpriteFileStructures, ID_PRACTICE_TARGET));

    data1.emplace(GameObject::TYPE_RADAR_STATION,
                  ObjectFactionData(std::vector<int>{ 4, 8, 0, 0, 0, 4, 5, 3 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_F1));

    data1.emplace(GameObject::TYPE_RADAR_TOWER,
                  ObjectFactionData(std::vector<int>{ 3, 6, 0, 0, 0, 3, 4, 2 },
                                    std::vector<int>{ 100, 50, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_TOWER_F1));

    data1.emplace(GameObject::TYPE_RES_GEN_ENERGY,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_ENERGY_F1));

    data1.emplace(GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_SOLAR_PANEL_F1));

    data1.emplace(GameObject::TYPE_RES_GEN_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_MATERIAL_F1));

    data1.emplace(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 10, 10 },
                                    SpriteFileStructures, ID_MATERIAL_EXTRACTOR_F1));

    data1.emplace(GameObject::TYPE_RES_STORAGE_BLOBS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_BLOBS_F1));

    data1.emplace(GameObject::TYPE_RES_STORAGE_DIAMONDS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_DIAMONDS_F1));

    data1.emplace(GameObject::TYPE_RES_STORAGE_ENERGY,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_ENERGY_F1));

    data1.emplace(GameObject::TYPE_RES_STORAGE_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_MATERIAL_F1));

    data1.emplace(GameObject::TYPE_TEMPLE,
                  ObjectFactionData(std::vector<int>{ 8, 4, 0, 0, 0, 1, 1, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_TEMPLE_F1));

    data1.emplace(GameObject::TYPE_UNIT_MEDIC1,
                  ObjectFactionData(std::vector<int>{ 6, 5, 1, 1, 1, 6, 4, 4, 2, 1, 1, 6 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_05_F1));

    data1.emplace(GameObject::TYPE_UNIT_SCOUT1,
                  ObjectFactionData(std::vector<int>{ 4, 4, 1, 2, 2, 4, 2, 2, 5, 1, 3, 1 },
                                    std::vector<int>{ 300, 300, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_04_F1));

    data1.emplace(GameObject::TYPE_UNIT_SOLDIER1,
                  ObjectFactionData(std::vector<int>{ 4, 5, 4, 5, 4, 3, 3, 2, 4, 1, 3, 1 },
                                    std::vector<int>{ 200, 200, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_02_F1));

    data1.emplace(GameObject::TYPE_UNIT_SOLDIER2,
                  ObjectFactionData(std::vector<int>{ 6, 5, 5, 4, 5, 4, 5, 6, 3, 1, 2, 2 },
                                    std::vector<int>{ 400, 400, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_03_F1));

    data1.emplace(GameObject::TYPE_UNIT_WORKER1,
                  ObjectFactionData(std::vector<int>{ 6, 6, 1, 1, 2, 4, 3, 2, 4, 5, 5, 1 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_01_F1));

    data1.emplace(GameObject::TYPE_WALL,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_L1_F1_HORIZ));

    data1.emplace(GameObject::TYPE_WALL_GATE,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_GATE_L1_F1_HORIZ_CLOSED));

    // ===== FACTION 2 =====
    std::unordered_map<GameObjectTypeId, ObjectFactionData> & data2 = mFactionData[FACTION_2];

    data2.emplace(GameObject::TYPE_BARRACKS,
                  ObjectFactionData(std::vector<int>{ 5, 4, 0, 0, 0, 5, 4, 3 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BARRACKS_F2));

    data2.emplace(GameObject::TYPE_BASE,
                  ObjectFactionData(std::vector<int>{ 6, 6, 5, 4, 5, 5, 6, 6 },
                                    std::vector<int>{ 1000, 1000, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BASE_L1_F2));

    data2.emplace(GameObject::TYPE_DEFENSIVE_TOWER,
                  ObjectFactionData(std::vector<int>{ 5, 4, 4, 4, 5, 5, 5, 5 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_DTOWER_L1_F2));

    data2.emplace(GameObject::TYPE_HOSPITAL,
                  ObjectFactionData(std::vector<int>{ 6, 4, 0, 0, 0, 5, 3, 2 },
                                    std::vector<int>{ 1000, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_HOSPITAL_F2));

    data2.emplace(GameObject::TYPE_PRACTICE_TARGET,
                  ObjectFactionData(std::vector<int>{ 8, 2, 0, 0, 0, 5, 8, 8 },
                                    std::vector<int>{ 50, 50, 0, 0 },
                                    SpriteFileStructures, ID_PRACTICE_TARGET));

    data2.emplace(GameObject::TYPE_RADAR_STATION,
                  ObjectFactionData(std::vector<int>{ 4, 8, 0, 0, 0, 4, 5, 3 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_F2));

    data2.emplace(GameObject::TYPE_RADAR_TOWER,
                  ObjectFactionData(std::vector<int>{ 3, 6, 0, 0, 0, 3, 4, 2 },
                                    std::vector<int>{ 100, 50, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_TOWER_F2));

    data2.emplace(GameObject::TYPE_RES_GEN_ENERGY,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_ENERGY_F2));

    data2.emplace(GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_SOLAR_PANEL_F2));

    data2.emplace(GameObject::TYPE_RES_GEN_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_MATERIAL_F2));

    data2.emplace(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 10, 10 },
                                    SpriteFileStructures, ID_MATERIAL_EXTRACTOR_F2));

    data2.emplace(GameObject::TYPE_RES_STORAGE_BLOBS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_BLOBS_F2));

    data2.emplace(GameObject::TYPE_RES_STORAGE_DIAMONDS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_DIAMONDS_F2));

    data2.emplace(GameObject::TYPE_RES_STORAGE_ENERGY,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_ENERGY_F2));

    data2.emplace(GameObject::TYPE_RES_STORAGE_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_MATERIAL_F2));

    data2.emplace(GameObject::TYPE_TEMPLE,
                  ObjectFactionData(std::vector<int>{ 8, 4, 0, 0, 0, 1, 1, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_TEMPLE_F2));

    data2.emplace(GameObject::TYPE_UNIT_MEDIC1,
                  ObjectFactionData(std::vector<int>{ 6, 5, 1, 1, 1, 6, 4, 4, 2, 1, 1, 6 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_05_F2));

    data2.emplace(GameObject::TYPE_UNIT_SCOUT1,
                  ObjectFactionData(std::vector<int>{ 4, 4, 1, 2, 2, 4, 2, 2, 5, 1, 3, 1 },
                                    std::vector<int>{ 300, 300, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_04_F2));

    data2.emplace(GameObject::TYPE_UNIT_SOLDIER1,
                  ObjectFactionData(std::vector<int>{ 4, 5, 4, 5, 4, 3, 3, 2, 4, 1, 3, 1 },
                                    std::vector<int>{ 200, 200, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_02_F2));

    data2.emplace(GameObject::TYPE_UNIT_SOLDIER2,
                  ObjectFactionData(std::vector<int>{ 6, 5, 5, 4, 5, 4, 5, 6, 3, 1, 2, 2 },
                                    std::vector<int>{ 400, 400, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_03_F2));

    data2.emplace(GameObject::TYPE_UNIT_WORKER1,
                  ObjectFactionData(std::vector<int>{ 6, 6, 1, 1, 2, 4, 3, 2, 4, 5, 5, 1 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_01_F2));

    data2.emplace(GameObject::TYPE_WALL,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_L1_F2_HORIZ));

    data2.emplace(GameObject::TYPE_WALL_GATE,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_GATE_L1_F2_HORIZ_CLOSED));

    // ===== FACTION 3 =====
    std::unordered_map<GameObjectTypeId, ObjectFactionData> & data3 = mFactionData[FACTION_3];

    data3.emplace(GameObject::TYPE_BARRACKS,
                  ObjectFactionData(std::vector<int>{ 5, 4, 0, 0, 0, 5, 4, 3 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BARRACKS_F3));

    data3.emplace(GameObject::TYPE_BASE,
                  ObjectFactionData(std::vector<int>{ 6, 6, 5, 4, 5, 5, 6, 6 },
                                    std::vector<int>{ 1000, 1000, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_BASE_L1_F3));

    data3.emplace(GameObject::TYPE_DEFENSIVE_TOWER,
                  ObjectFactionData(std::vector<int>{ 5, 4, 4, 4, 5, 5, 5, 5 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_DTOWER_L1_F3));

    data3.emplace(GameObject::TYPE_HOSPITAL,
                  ObjectFactionData(std::vector<int>{ 6, 4, 0, 0, 0, 5, 3, 2 },
                                    std::vector<int>{ 1000, 500, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_HOSPITAL_F3));

    data3.emplace(GameObject::TYPE_PRACTICE_TARGET,
                  ObjectFactionData(std::vector<int>{ 8, 2, 0, 0, 0, 5, 8, 8 },
                                    std::vector<int>{ 50, 50, 0, 0 },
                                    SpriteFileStructures, ID_PRACTICE_TARGET));

    data3.emplace(GameObject::TYPE_RADAR_STATION,
                  ObjectFactionData(std::vector<int>{ 4, 8, 0, 0, 0, 4, 5, 3 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_F3));

    data3.emplace(GameObject::TYPE_RADAR_TOWER,
                  ObjectFactionData(std::vector<int>{ 3, 6, 0, 0, 0, 3, 4, 2 },
                                    std::vector<int>{ 100, 50, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_RADAR_TOWER_F3));

    data3.emplace(GameObject::TYPE_RES_GEN_ENERGY,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_ENERGY_F3));

    data3.emplace(GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_SOLAR_PANEL_F3));

    data3.emplace(GameObject::TYPE_RES_GEN_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 9, 4, 0, 0, 0, 3, 2, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_GEN_MATERIAL_F3));

    data3.emplace(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT,
                  ObjectFactionData(std::vector<int>{ 4, 2, 0, 0, 0, 2, 2, 2 },
                                    std::vector<int>{ 150, 150, 10, 10 },
                                    SpriteFileStructures, ID_MATERIAL_EXTRACTOR_F3));

    data3.emplace(GameObject::TYPE_RES_STORAGE_BLOBS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_BLOBS_F3));

    data3.emplace(GameObject::TYPE_RES_STORAGE_DIAMONDS,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_DIAMONDS_F3));

    data3.emplace(GameObject::TYPE_RES_STORAGE_ENERGY,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_ENERGY_F3));

    data3.emplace(GameObject::TYPE_RES_STORAGE_MATERIAL,
                  ObjectFactionData(std::vector<int>{ 3, 2, 0, 0, 0, 2, 3, 4 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_STORAGE_MATERIAL_F3));

    data3.emplace(GameObject::TYPE_TEMPLE,
                  ObjectFactionData(std::vector<int>{ 8, 4, 0, 0, 0, 1, 1, 1 },
                                    std::vector<int>{ 0, 0, 0, 0 },
                                    SpriteFileStructures, ID_STRUCT_TEMPLE_F3));

    data3.emplace(GameObject::TYPE_UNIT_MEDIC1,
                  ObjectFactionData(std::vector<int>{ 6, 5, 1, 1, 1, 6, 4, 4, 2, 1, 1, 6 },
                                    std::vector<int>{ 500, 500, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_05_F3));

    data3.emplace(GameObject::TYPE_UNIT_SCOUT1,
                  ObjectFactionData(std::vector<int>{ 4, 4, 1, 2, 2, 4, 2, 2, 5, 1, 3, 1 },
                                    std::vector<int>{ 300, 300, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_04_F3));

    data3.emplace(GameObject::TYPE_UNIT_SOLDIER1,
                  ObjectFactionData(std::vector<int>{ 4, 5, 4, 5, 4, 3, 3, 2, 4, 1, 3, 1 },
                                    std::vector<int>{ 200, 200, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_02_F3));

    data3.emplace(GameObject::TYPE_UNIT_SOLDIER2,
                  ObjectFactionData(std::vector<int>{ 6, 5, 5, 4, 5, 4, 5, 6, 3, 1, 2, 2 },
                                    std::vector<int>{ 400, 400, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_03_F3));

    data3.emplace(GameObject::TYPE_UNIT_WORKER1,
                  ObjectFactionData(std::vector<int>{ 6, 6, 1, 1, 2, 4, 3, 2, 4, 5, 5, 1 },
                                    std::vector<int>{ 150, 150, 0, 0 },
                                    SpriteFileUnits, SPR_UNIT_01_F3));

    data3.emplace(GameObject::TYPE_WALL,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_L1_F3_HORIZ));

    data3.emplace(GameObject::TYPE_WALL_GATE,
                  ObjectFactionData(std::vector<int>{ 5, 2, 0, 0, 0, 5, 6, 6 },
                                    std::vector<int>{ 100, 100, 0, 0 },
                                    SpriteFileWalls, WALL_GATE_L1_F3_HORIZ_CLOSED));
}

} // namespace game
