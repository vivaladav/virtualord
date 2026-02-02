#include "GameObjects/ObjectsDataRegistry.h"

#include "GameData.h"

#include <cassert>

namespace game
{

ObjectsDataRegistry::ObjectsDataRegistry()
{
    InitObjectData();

    InitWeaponData();
}


const ObjectData & ObjectsDataRegistry::GetObjectData(GameObjectTypeId type) const
{
    auto res = mDataObjects.find(type);

    if(res != mDataObjects.end())
        return res->second;
    else
        return ObjectData::NullObj;
}

const WeaponData & ObjectsDataRegistry::GetWeaponData(WeaponType type) const
{
    auto res = mDataWeapons.find(type);

    if(res != mDataWeapons.end())
        return res->second;
    else
        return WeaponData::NullObj;
}

void ObjectsDataRegistry::InitObjectData()
{
    mDataObjects.emplace(ObjectData::TYPE_BARRACKS,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4},
                     {OBJ_ATT_REGENERATION, 5}, {OBJ_ATT_RESISTANCE, 4},
                     {OBJ_ATT_SHIELD, 3}, {OBJ_ATT_CONSTRUCTION, 5}, {OBJ_ATT_HEALTH, 5} },
                   { 450, 450, 0, 0 },
                   { ID_STRUCT_BARRACKS_F1, ID_STRUCT_BARRACKS_F2,
                     ID_STRUCT_BARRACKS_F3, ID_STRUCT_BARRACKS}, SpriteFileStructures,
                   ObjectData::TYPE_BARRACKS, ObjectData::CAT_STRUCTURE,
                   OCS_BARRACKS, OCAT_GENERIC, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_CITY_BLOCK,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 7}, {OBJ_ATT_RESISTANCE, 5} }, {},
                   { ID_CITY_TL1_F1, ID_CITY_TL1_F2, ID_CITY_TL1_F3, ID_CITY_TL1_NF },
                   SpriteFileCity, ObjectData::TYPE_CITY_BLOCK, ObjectData::CAT_SCENE_OBJ,
                   OCO_SCENE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_BASE,
        ObjectData({ {OBJ_ATT_ENERGY, 6}, {OBJ_ATT_VIEW_RANGE, 6}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 6}, {OBJ_ATT_SHIELD, 6}, {OBJ_ATT_CONSTRUCTION, 6},
                     {OBJ_ATT_HEALTH, 8} }, { 1000, 1000, 0, 0 },
                   { ID_STRUCT_BASE_L1_F1, ID_STRUCT_BASE_L1_F2, ID_STRUCT_BASE_L1_F3,
                     ID_STRUCT_BASE_L1}, SpriteFileStructures, ObjectData::TYPE_BASE,
                   ObjectData::CAT_STRUCTURE, OCS_BASE, OCAT_GENERIC, WeaponData::TYPE_NULL, 3, 3));

    mDataObjects.emplace(ObjectData::TYPE_BASE_SPOT,
        ObjectData({}, {}, { ID_STRUCT_BASE_L1 }, SpriteFileStructures,
                   ObjectData::TYPE_BASE_SPOT, ObjectData::CAT_STRUCTURE,
                   OCS_BASE, OCAT_GENERIC, WeaponData::TYPE_NULL, 3, 3));

    mDataObjects.emplace(ObjectData::TYPE_BLOBS,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 1} }, {}, { BLOBS_1 },
                   SpriteCollectiblesFile, ObjectData::TYPE_BLOBS, ObjectData::CAT_COLLECTABLE,
                   OCO_COLLECTABLE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_BUNKER,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 3}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 5}, {OBJ_ATT_SHIELD, 5}, {OBJ_ATT_HEALTH, 7} },
                   { 150, 200, 0, 0 },
                   { ID_STRUCT_BUNKER_F1, ID_STRUCT_BUNKER_F2, ID_STRUCT_BUNKER_F3, ID_STRUCT_BUNKER },
                   SpriteFileStructures, ObjectData::TYPE_BUNKER, ObjectData::CAT_STRUCTURE,
                   OCS_TOWER, OCAT_DEFENSE, WeaponData::TYPE_LASER4, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_DEFENSIVE_TOWER,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 5}, {OBJ_ATT_SHIELD, 5}, {OBJ_ATT_HEALTH, 7} },
                   { 250, 300, 0, 0 }, { ID_STRUCT_DTOWER_L1_F1, ID_STRUCT_DTOWER_L1_F2,
                   ID_STRUCT_DTOWER_L1_F3, ID_STRUCT_DTOWER_L1 }, SpriteFileStructures,
                   ObjectData::TYPE_DEFENSIVE_TOWER, ObjectData::CAT_STRUCTURE,
                   OCS_TOWER, OCAT_DEFENSE, WeaponData::TYPE_LASER5, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_DIAMONDS,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 1} }, {}, { DIAMONDS_1 },
                   SpriteCollectiblesFile, ObjectData::TYPE_DIAMONDS, ObjectData::CAT_COLLECTABLE,
                   OCO_COLLECTABLE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_HOSPITAL,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALING_RANGE, 5},
                     {OBJ_ATT_HEALING_POWER, 6}, {OBJ_ATT_CONSTRUCTION, 4}, {OBJ_ATT_HEALTH, 5} },
                   { 1000, 500, 0, 0 },
                   { ID_STRUCT_HOSPITAL_F1, ID_STRUCT_HOSPITAL_F2,
                     ID_STRUCT_HOSPITAL_F3, ID_STRUCT_HOSPITAL}, SpriteFileStructures,
                   ObjectData::TYPE_HOSPITAL, ObjectData::CAT_STRUCTURE,
                   OCS_HOSPITAL, OCAT_GENERIC, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_LOOTBOX,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 1}}, {}, { IND_LOOTBOX },
                   SpriteCollectiblesFile, ObjectData::TYPE_LOOTBOX, ObjectData::CAT_COLLECTABLE,
                   OCO_COLLECTABLE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_LOOTBOX2,
        ObjectData({ {OBJ_ATT_ENERGY, 2}, {OBJ_ATT_HEALTH, 1}}, {}, { IND_LOOTBOX_L2 },
                   SpriteCollectiblesFile, ObjectData::TYPE_LOOTBOX2, ObjectData::CAT_COLLECTABLE,
                   OCO_COLLECTABLE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_MINI_UNIT1,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 5}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_SPEED, 8},
                     {OBJ_ATT_HEALTH, 5} }, { 20, 20, 0, 0 },
                   { SID_MUNIT_01_1X_F1, SID_MUNIT_01_1X_F2, SID_MUNIT_01_1X_F3 }, SpriteFileMiniUnits,
                   ObjectData::TYPE_MINI_UNIT1, ObjectData::CAT_MINI_UNIT,
                   OCMU_SOLDIER, OCAT_MINI_UNIT, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_MINI_UNIT2,
        ObjectData({ {OBJ_ATT_ENERGY, 2}, {OBJ_ATT_VIEW_RANGE, 6}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 2},  {OBJ_ATT_SPEED, 9},
                     {OBJ_ATT_HEALTH, 4} }, { 15, 15, 0, 0 },
                   { SID_MUNIT_02_1X_F1, SID_MUNIT_02_1X_F2, SID_MUNIT_02_1X_F3 }, SpriteFileMiniUnits,
                   ObjectData::TYPE_MINI_UNIT2, ObjectData::CAT_MINI_UNIT,
                   OCMU_SOLDIER, OCAT_MINI_UNIT, WeaponData::TYPE_LASER2, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_MOUNTAINS,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 9}, {OBJ_ATT_RESISTANCE, 7} }, {},
                   { ID_SCENE_MOUNTAIN_1 }, SpriteFileSceneElements, ObjectData::TYPE_MOUNTAINS,
                   ObjectData::CAT_SCENE_OBJ, OCO_SCENE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_PRACTICE_TARGET,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 8}, {OBJ_ATT_SHIELD, 8}, {OBJ_ATT_HEALTH, 9} },
                   { 200, 350, 0, 0 },
                   { ID_PRACTICE_TARGET }, SpriteFileStructures,
                   ObjectData::TYPE_PRACTICE_TARGET, ObjectData::CAT_STRUCTURE,
                   OCS_TARGET, OCAT_GENERIC, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RADAR_STATION,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 8}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 5}, {OBJ_ATT_SHIELD, 3}, {OBJ_ATT_HEALTH, 4} },
                   { 250, 200, 0, 0 },
                   { ID_STRUCT_RADAR_F1, ID_STRUCT_RADAR_F2, ID_STRUCT_RADAR_F3,
                     ID_STRUCT_RADAR }, SpriteFileStructures,
                   ObjectData::TYPE_RADAR_STATION, ObjectData::CAT_STRUCTURE,
                   OCS_RADAR, OCAT_TECHNOLOGY, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_RADAR_TOWER,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 6}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 4}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALTH, 3} },
                   { 150, 150, 0, 0 },
                   { ID_STRUCT_RADAR_TOWER_F1, ID_STRUCT_RADAR_TOWER_F2,
                     ID_STRUCT_RADAR_TOWER_F3, ID_STRUCT_RADAR_TOWER }, SpriteFileStructures,
                   ObjectData::TYPE_RADAR_TOWER, ObjectData::CAT_STRUCTURE,
                   OCS_RADAR, OCAT_TECHNOLOGY, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RESEARCH_CENTER,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALTH, 4} },
                   { 1000, 500, 100, 100 },
                   { ID_STRUCT_RESEARCH_CENTER_F1, ID_STRUCT_RESEARCH_CENTER_F2,
                     ID_STRUCT_RESEARCH_CENTER_F3, ID_STRUCT_RESEARCH_CENTER }, SpriteFileStructures,
                   ObjectData::TYPE_RESEARCH_CENTER, ObjectData::CAT_STRUCTURE,
                   OCS_RESEARCH, OCAT_TECHNOLOGY, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_RES_GEN_ENERGY,
        ObjectData({ {OBJ_ATT_ENERGY, 9}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 1}, {OBJ_ATT_HEALTH, 5} },
                   { 0, 0, 0, 0 },
                   { ID_STRUCT_GEN_ENERGY_F1, ID_STRUCT_GEN_ENERGY_F2,
                     ID_STRUCT_GEN_ENERGY_F3, ID_STRUCT_GEN_ENERGY }, SpriteFileStructures,
                   ObjectData::TYPE_RES_GEN_ENERGY, ObjectData::CAT_RES_GENERATOR,
                   OCS_GENERATOR, OCAT_RESOURCES, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALTH, 4} },
                   { 250, 250, 0, 0 },
                   { ID_STRUCT_SOLAR_PANEL_F1, ID_STRUCT_SOLAR_PANEL_F2,
                     ID_STRUCT_SOLAR_PANEL_F3, ID_STRUCT_SOLAR_PANEL }, SpriteFileStructures,
                   ObjectData::TYPE_RES_GEN_ENERGY_SOLAR, ObjectData::CAT_RES_GENERATOR,
                   OCS_GENERATOR, OCAT_RESOURCES, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RES_GEN_MATERIAL,
        ObjectData({ {OBJ_ATT_ENERGY, 9}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 1}, {OBJ_ATT_HEALTH, 5} },
                   { 0, 0, 0, 0 },
                   { ID_STRUCT_GEN_MATERIAL_F1, ID_STRUCT_GEN_MATERIAL_F2,
                     ID_STRUCT_GEN_MATERIAL_F3, ID_STRUCT_GEN_MATERIAL}, SpriteFileStructures,
                   ObjectData::TYPE_RES_GEN_MATERIAL, ObjectData::CAT_RES_GENERATOR,
                   OCS_GENERATOR, OCAT_RESOURCES, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALTH, 4} },
                   { 250, 250, 10, 10 },
                   { ID_MATERIAL_EXTRACTOR_F1, ID_MATERIAL_EXTRACTOR_F2,
                     ID_MATERIAL_EXTRACTOR_F3, ID_MATERIAL_EXTRACTOR }, SpriteFileStructures,
                   ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT, ObjectData::CAT_RES_GENERATOR,
                   OCS_GENERATOR, OCAT_RESOURCES, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_RES_STORAGE_BLOBS,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 4}, {OBJ_ATT_STORAGE, 3},
                     {OBJ_ATT_HEALTH, 4} },
                   { 200, 200, 0, 0 },
                   { ID_STRUCT_STORAGE_BLOBS_F1, ID_STRUCT_STORAGE_BLOBS_F2,
                     ID_STRUCT_STORAGE_BLOBS_F3, ID_STRUCT_STORAGE_BLOBS }, SpriteFileStructures,
                   ObjectData::TYPE_RES_STORAGE_BLOBS, ObjectData::CAT_RES_STORAGE,
                   OCS_STORAGE, OCAT_RESOURCES, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RES_STORAGE_DIAMONDS,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 4}, {OBJ_ATT_STORAGE, 3},
                     {OBJ_ATT_HEALTH, 4} },
                   { 200, 200, 0, 0 },
                   { ID_STRUCT_STORAGE_DIAMONDS_F1, ID_STRUCT_STORAGE_DIAMONDS_F2,
                     ID_STRUCT_STORAGE_DIAMONDS_F3, ID_STRUCT_STORAGE_DIAMONDS }, SpriteFileStructures,
                   ObjectData::TYPE_RES_STORAGE_DIAMONDS, ObjectData::CAT_RES_STORAGE,
                   OCS_STORAGE, OCAT_RESOURCES, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RES_STORAGE_ENERGY,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 4}, {OBJ_ATT_STORAGE, 7},
                     {OBJ_ATT_HEALTH, 5} },
                   { 100, 100, 0, 0 },
                   { ID_STRUCT_STORAGE_ENERGY_F1, ID_STRUCT_STORAGE_ENERGY_F2,
                     ID_STRUCT_STORAGE_ENERGY_F3, ID_STRUCT_STORAGE_ENERGY }, SpriteFileStructures,
                   ObjectData::TYPE_RES_STORAGE_ENERGY, ObjectData::CAT_RES_STORAGE,
                   OCS_STORAGE, OCAT_RESOURCES, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_RES_STORAGE_MATERIAL,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 2},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 4}, {OBJ_ATT_STORAGE, 6},
                     {OBJ_ATT_HEALTH, 4} },
                   { 100, 100, 0, 0 },
                   { ID_STRUCT_STORAGE_MATERIAL_F1, ID_STRUCT_STORAGE_MATERIAL_F2,
                     ID_STRUCT_STORAGE_MATERIAL_F3, ID_STRUCT_STORAGE_MATERIAL }, SpriteFileStructures,
                   ObjectData::TYPE_RES_STORAGE_MATERIAL, ObjectData::CAT_RES_STORAGE,
                   OCS_STORAGE, OCAT_RESOURCES, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_ROCKS,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 6}, {OBJ_ATT_RESISTANCE, 6} }, {},
                   { ROCKS_ROW_1 }, SpriteRocksFile, ObjectData::TYPE_ROCKS, ObjectData::CAT_SCENE_OBJ,
                   OCO_SCENE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_SPAWN_TOWER,
        ObjectData({ {OBJ_ATT_ENERGY, 6}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 4}, {OBJ_ATT_SHIELD, 5}, {OBJ_ATT_HEALTH, 6},
                     {OBJ_ATT_SPAWNING, 6} },
                   { 750, 500, 50, 50 },
                   { ID_STRUCT_SPAWN_TOWER_F1, ID_STRUCT_SPAWN_TOWER_F2, ID_STRUCT_SPAWN_TOWER_F3,
                     ID_STRUCT_SPAWN_TOWER }, SpriteFileStructures,
                   ObjectData::TYPE_SPAWN_TOWER, ObjectData::CAT_STRUCTURE,
                   OCS_TOWER, OCAT_GENERIC, WeaponData::TYPE_NULL, 2, 2));

    mDataObjects.emplace(ObjectData::TYPE_TEMPLE,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 1},
                   {OBJ_ATT_RESISTANCE, 1}, {OBJ_ATT_SHIELD, 1}, {OBJ_ATT_HEALTH, 3} },
                   { 0, 0, 0, 0 },
                   { ID_STRUCT_TEMPLE_F1, ID_STRUCT_TEMPLE_F2, ID_STRUCT_TEMPLE_F3,
                     ID_STRUCT_TEMPLE}, SpriteFileStructures,
                   ObjectData::TYPE_TEMPLE, ObjectData::CAT_STRUCTURE,
                   OCS_RELICS, OCAT_GENERIC, WeaponData::TYPE_NULL, 3, 3));

    mDataObjects.emplace(ObjectData::TYPE_TRADING_POST,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_HEALTH, 5} },
                   { 1000, 500, 0, 0 },
                   { ID_STRUCT_TRADING_POST_F1, ID_STRUCT_TRADING_POST_F2,
                     ID_STRUCT_TRADING_POST_F3, ID_STRUCT_TRADING_POST}, SpriteFileStructures,
                   ObjectData::TYPE_TRADING_POST, ObjectData::CAT_STRUCTURE,
                   OCS_TRADING, OCAT_GENERIC, WeaponData::TYPE_NULL, 3, 3));

    mDataObjects.emplace(ObjectData::TYPE_TREES,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_HEALTH, 1}, {OBJ_ATT_RESISTANCE, 1} }, {},
                   { TREE1_1T_1 }, SpriteFileTrees, ObjectData::TYPE_TREES, ObjectData::CAT_SCENE_OBJ,
                   OCO_SCENE, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_MEDIC1,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 5}, {OBJ_ATT_REGENERATION, 6},
                     {OBJ_ATT_RESISTANCE, 4}, {OBJ_ATT_SHIELD, 4},  {OBJ_ATT_SPEED, 2},
                     {OBJ_ATT_HEALING_RANGE, 3}, {OBJ_ATT_HEALING_POWER, 5}, {OBJ_ATT_HEALTH, 6} } ,
                   { 500, 500, 0, 0 },
                   { SPR_UNIT_05_F1, SPR_UNIT_05_F2, SPR_UNIT_05_F3}, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_MEDIC1, ObjectData::CAT_UNIT,
                   OCU_MEDIC, OCAT_UNIT, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_SCOUT1,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 4}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 2}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_SPEED, 5},
                     {OBJ_ATT_CONSTRUCTION, 2}, {OBJ_ATT_CONQUEST, 5}, {OBJ_ATT_HEALTH, 6} },
                   { 300, 300, 0, 0 },
                   { SPR_UNIT_04_F1, SPR_UNIT_04_F2, SPR_UNIT_04_F3}, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_SCOUT1, ObjectData::CAT_UNIT,
                   OCU_SOLDIER, OCAT_UNIT, WeaponData::TYPE_LASER1, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_SOLDIER1,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 5}, {OBJ_ATT_REGENERATION, 3},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_SPEED, 4},
                     {OBJ_ATT_CONQUEST, 3}, {OBJ_ATT_HEALTH, 4} },
                   { 200, 200, 0, 0 },
                   { SPR_UNIT_02_F1, SPR_UNIT_02_F2, SPR_UNIT_02_F3 }, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_SOLDIER1, ObjectData::CAT_UNIT,
                   OCU_SOLDIER, OCAT_UNIT, WeaponData::TYPE_LASER2, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_SOLDIER2,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 5}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 5}, {OBJ_ATT_SHIELD, 6}, {OBJ_ATT_SPEED, 3},
                     {OBJ_ATT_CONQUEST, 2}, {OBJ_ATT_HEALTH, 5} },
                   { 400, 400, 0, 0 }, { SPR_UNIT_03_F1, SPR_UNIT_03_F2, SPR_UNIT_03_F3 }, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_SOLDIER2, ObjectData::CAT_UNIT, OCU_SOLDIER, OCAT_UNIT,
                   WeaponData::TYPE_LASER3, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_SPAWNER1,
        ObjectData({ {OBJ_ATT_ENERGY, 5}, {OBJ_ATT_VIEW_RANGE, 5}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 3},  {OBJ_ATT_SPEED, 4},
                     {OBJ_ATT_SPAWNING, 1}, {OBJ_ATT_HEALTH, 5} },
                   { 200, 200, 5, 5 },
                   { SPR_UNIT_06_F1, SPR_UNIT_06_F2, SPR_UNIT_06_F3 }, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_SPAWNER1, ObjectData::CAT_UNIT,
                   OCU_SPAWNER, OCAT_UNIT, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_SPAWNER2,
        ObjectData({ {OBJ_ATT_ENERGY, 6}, {OBJ_ATT_VIEW_RANGE, 6}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 4},  {OBJ_ATT_SPEED, 3},
                     {OBJ_ATT_SPAWNING, 3}, {OBJ_ATT_HEALTH, 6} },
                   { 300, 300, 25, 25 },
                   { SPR_UNIT_07_F1, SPR_UNIT_07_F2, SPR_UNIT_07_F3 }, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_SPAWNER2, ObjectData::CAT_UNIT,
                   OCU_SPAWNER, OCAT_UNIT, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_UNIT_WORKER1,
        ObjectData({ {OBJ_ATT_ENERGY, 4}, {OBJ_ATT_VIEW_RANGE, 6}, {OBJ_ATT_REGENERATION, 4},
                     {OBJ_ATT_RESISTANCE, 3}, {OBJ_ATT_SHIELD, 2}, {OBJ_ATT_SPEED, 4},
                     {OBJ_ATT_CONSTRUCTION, 5}, {OBJ_ATT_CONQUEST, 3}, {OBJ_ATT_HEALTH, 5} } ,
                   { 150, 150, 0, 0 },
                   { SPR_UNIT_01_F1, SPR_UNIT_01_F2, SPR_UNIT_01_F3 }, SpriteFileUnits,
                   ObjectData::TYPE_UNIT_WORKER1, ObjectData::CAT_UNIT,
                   OCU_WORKER, OCAT_UNIT, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_WALL,
        ObjectData({ {OBJ_ATT_ENERGY, 1}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 5},
                     {OBJ_ATT_RESISTANCE, 6}, {OBJ_ATT_SHIELD, 6}, {OBJ_ATT_HEALTH, 7} },
                   { 100, 100, 0, 0 },
                   { WALL_L1_F1_HORIZ, WALL_L1_F2_HORIZ, WALL_L1_F3_HORIZ },
                   SpriteFileWalls, ObjectData::TYPE_WALL, ObjectData::CAT_STRUCTURE,
                   OCS_WALL, OCAT_DEFENSE, WeaponData::TYPE_NULL, 1, 1));

    mDataObjects.emplace(ObjectData::TYPE_WALL_GATE,
        ObjectData({ {OBJ_ATT_ENERGY, 3}, {OBJ_ATT_VIEW_RANGE, 2}, {OBJ_ATT_REGENERATION, 6},
                     {OBJ_ATT_RESISTANCE, 5}, {OBJ_ATT_SHIELD, 5}, {OBJ_ATT_HEALTH, 7} },
                   { 100, 100, 0, 0 },
                   { WALL_GATE_L1_F1_HORIZ_CLOSED, WALL_GATE_L1_F2_HORIZ_CLOSED,
                     WALL_GATE_L1_F3_HORIZ_CLOSED }, SpriteFileWalls,
                   ObjectData::TYPE_WALL_GATE, ObjectData::CAT_STRUCTURE,
                   OCS_WALL, OCAT_DEFENSE, WeaponData::TYPE_NULL, 1, 1));

#ifdef DEV_MODE
    for(auto it : mDataObjects)
        assert(it.first == it.second.GetType());
#endif
}

void ObjectsDataRegistry::InitWeaponData()
{
    mDataWeapons.emplace(WeaponData::TYPE_LASER1,
        WeaponData(WeaponData::TYPE_LASER1, WEAPONC_LASER, 2, 0.4f,
                   { {OBJ_ATT_ATTACK_ACCURACY, 6}, {OBJ_ATT_ATTACK_POWER, 1}, {OBJ_ATT_ATTACK_RANGE, 5} },
                   { {ATT_AIMED_SHOT, 20}, {ATT_BURST_SHOT, 5}, {ATT_QUICK_SHOT, 10} }));

    mDataWeapons.emplace(WeaponData::TYPE_LASER2,
        WeaponData(WeaponData::TYPE_LASER2, WEAPONC_LASER, 3, 0.4f,
                   { {OBJ_ATT_ATTACK_ACCURACY, 4}, {OBJ_ATT_ATTACK_POWER, 2}, {OBJ_ATT_ATTACK_RANGE, 4} },
                   { {ATT_AIMED_SHOT, 25}, {ATT_BURST_SHOT, 10}, {ATT_QUICK_SHOT, 15} }));

    mDataWeapons.emplace(WeaponData::TYPE_LASER3,
        WeaponData(WeaponData::TYPE_LASER3, WEAPONC_LASER, 3, 0.4f,
                   { {OBJ_ATT_ATTACK_ACCURACY, 5}, {OBJ_ATT_ATTACK_POWER, 3}, {OBJ_ATT_ATTACK_RANGE, 3} },
                   { {ATT_AIMED_SHOT, 30}, {ATT_BURST_SHOT, 15}, {ATT_QUICK_SHOT, 20} }));

    mDataWeapons.emplace(WeaponData::TYPE_LASER4,
        WeaponData(WeaponData::TYPE_LASER4, WEAPONC_LASER, 4, 0.5f,
                   { {OBJ_ATT_ATTACK_ACCURACY, 5}, {OBJ_ATT_ATTACK_POWER, 4}, {OBJ_ATT_ATTACK_RANGE, 4} },
                   { {ATT_AIMED_SHOT, 35}, {ATT_BURST_SHOT, 20}, {ATT_QUICK_SHOT, 25} }));

    mDataWeapons.emplace(WeaponData::TYPE_LASER5,
        WeaponData(WeaponData::TYPE_LASER5, WEAPONC_LASER, 4, 0.5f,
                   { {OBJ_ATT_ATTACK_ACCURACY, 6}, {OBJ_ATT_ATTACK_POWER, 5}, {OBJ_ATT_ATTACK_RANGE, 5} },
                   { {ATT_AIMED_SHOT, 40}, {ATT_BURST_SHOT, 25}, {ATT_QUICK_SHOT, 30} }));

#ifdef DEV_MODE
    for(auto it : mDataWeapons)
        assert(it.first == it.second.GetType());
#endif
}

} // namespace game
