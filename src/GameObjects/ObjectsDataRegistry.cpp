#include "GameObjects/ObjectsDataRegistry.h"

#include "GameData.h"
#include "GameObject.h"

namespace game
{

ObjectsDataRegistry::ObjectsDataRegistry()
{
    InitObjectData();
}


const ObjectData & ObjectsDataRegistry::GetObjectData(GameObjectTypeId type) const
{
    auto res = mData.find(type);

    if(res != mData.end())
        return res->second;
    else
        return ObjectData::NullObj;
}

void ObjectsDataRegistry::InitObjectData()
{
    mData.emplace(GameObject::TYPE_BARRACKS,
                  ObjectData({ 5, 4, 0, 0, 0, 5, 4, 3, 0, 0, 0, 0 }, { 500, 500, 0, 0 },
                             { ID_STRUCT_BARRACKS_F1, ID_STRUCT_BARRACKS_F2,
                               ID_STRUCT_BARRACKS_F3, ID_STRUCT_BARRACKS},  SpriteFileStructures,
                             GameObject::TYPE_BARRACKS, OCS_BARRACKS, OCAT_GENERIC, 2, 2));

    mData.emplace(GameObject::TYPE_BASE,
                  ObjectData({ 6, 6, 5, 4, 5, 5, 6, 6, 0, 0, 0, 0 }, { 1000, 1000, 0, 0 },
                             { ID_STRUCT_BASE_L1_F1, ID_STRUCT_BASE_L1_F2,
                              ID_STRUCT_BASE_L1_F3, ID_STRUCT_BASE_L1}, SpriteFileStructures,
                             GameObject::TYPE_BASE, OCS_BASE, OCAT_GENERIC, 3, 3));

    mData.emplace(GameObject::TYPE_BASE_SPOT,
                  ObjectData({}, {}, { ID_STRUCT_BASE_L1 }, SpriteFileStructures,
                             GameObject::TYPE_BASE_SPOT, OCS_BASE, OCAT_GENERIC, 3, 3));

    mData.emplace(GameObject::TYPE_BLOBS,
                  ObjectData({}, {}, { BLOBS_1 }, SpriteCollectiblesFile,
                             GameObject::TYPE_BLOBS, OCO_COLLECTABLE, OCAT_UNDEFINED, 1, 1));

    mData.emplace(GameObject::TYPE_BUNKER,
                  ObjectData({ 4, 3, 3, 4, 3, 4, 4, 4, 0, 0, 0, 0 }, { 150, 200, 0, 0 },
                             { ID_STRUCT_BUNKER_F1, ID_STRUCT_BUNKER_F2,
                              ID_STRUCT_BUNKER_F3, ID_STRUCT_BUNKER }, SpriteFileStructures,
                             GameObject::TYPE_BUNKER, OCS_TOWER, OCAT_DEFENSE, 1, 1));

    mData.emplace(GameObject::TYPE_DEFENSIVE_TOWER,
                  ObjectData({ 5, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0, 0 }, { 250, 300, 0, 0 },
                             { ID_STRUCT_DTOWER_L1_F1, ID_STRUCT_DTOWER_L1_F2,
                               ID_STRUCT_DTOWER_L1_F3, ID_STRUCT_DTOWER_L1 }, SpriteFileStructures,
                             GameObject::TYPE_DEFENSIVE_TOWER, OCS_TOWER, OCAT_DEFENSE, 1, 1));

    mData.emplace(GameObject::TYPE_DIAMONDS,
                  ObjectData({}, {}, { DIAMONDS_1 }, SpriteCollectiblesFile,
                             GameObject::TYPE_DIAMONDS, OCO_COLLECTABLE, OCAT_UNDEFINED, 1, 1));

    mData.emplace(GameObject::TYPE_HOSPITAL,
                  ObjectData({ 6, 4, 0, 0, 0, 5, 3, 2, 0, 0, 0, 5 }, { 1000, 500, 0, 0 },
                             { ID_STRUCT_HOSPITAL_F1, ID_STRUCT_HOSPITAL_F2,
                               ID_STRUCT_HOSPITAL_F3, ID_STRUCT_HOSPITAL}, SpriteFileStructures,
                             GameObject::TYPE_HOSPITAL, OCS_HOSPITAL, OCAT_GENERIC, 2, 2));

    mData.emplace(GameObject::TYPE_LOOTBOX,
                  ObjectData({}, {}, { IND_LOOTBOX }, SpriteCollectiblesFile,
                             GameObject::TYPE_LOOTBOX, OCO_COLLECTABLE, OCAT_UNDEFINED, 1, 1));

    mData.emplace(GameObject::TYPE_MOUNTAINS,
                  ObjectData({}, {}, { ID_SCENE_MOUNTAIN_1 }, SpriteFileSceneElements,
                             GameObject::TYPE_MOUNTAINS, OCO_SCENE, OCAT_UNDEFINED, 2, 2));

    mData.emplace(GameObject::TYPE_PRACTICE_TARGET,
                  ObjectData({ 8, 2, 0, 0, 0, 5, 8, 8, 0, 0, 0, 0 }, { 100, 250, 0, 0 },
                             { ID_PRACTICE_TARGET }, SpriteFileStructures,
                             GameObject::TYPE_PRACTICE_TARGET, OCS_TARGET, OCAT_GENERIC, 1, 1));

    mData.emplace(GameObject::TYPE_RADAR_STATION,
                  ObjectData({ 4, 8, 0, 0, 0, 4, 5, 3, 0, 0, 0, 0 }, { 250, 150, 0, 0 },
                             { ID_STRUCT_RADAR_F1, ID_STRUCT_RADAR_F2, ID_STRUCT_RADAR_F3,
                               ID_STRUCT_RADAR }, SpriteFileStructures, GameObject::TYPE_RADAR_STATION,
                              OCS_RADAR, OCAT_TECHNOLOGY, 2, 2));

    mData.emplace(GameObject::TYPE_RADAR_TOWER,
                  ObjectData({ 3, 6, 0, 0, 0, 3, 4, 2, 0, 0, 0, 0 }, { 150, 100, 0, 0 },
                             { ID_STRUCT_RADAR_TOWER_F1, ID_STRUCT_RADAR_TOWER_F2,
                               ID_STRUCT_RADAR_TOWER_F3, ID_STRUCT_RADAR_TOWER }, SpriteFileStructures,
                             GameObject::TYPE_RADAR_STATION, OCS_RADAR, OCAT_TECHNOLOGY, 1, 1));

    mData.emplace(GameObject::TYPE_RESEARCH_CENTER,
                  ObjectData({ 4, 4, 0, 0, 0, 4, 3, 2, 0, 0, 0, 0 }, { 1000, 500, 100, 100 },
                             { ID_STRUCT_RESEARCH_CENTER_F1, ID_STRUCT_RESEARCH_CENTER_F2,
                              ID_STRUCT_RESEARCH_CENTER_F3, ID_STRUCT_RESEARCH_CENTER }, SpriteFileStructures,
                             GameObject::TYPE_RESEARCH_CENTER, OCS_RESEARCH, OCAT_TECHNOLOGY, 2, 2));

    mData.emplace(GameObject::TYPE_RES_GEN_ENERGY,
                  ObjectData({ 9, 4, 0, 0, 0, 3, 2, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                             { ID_STRUCT_GEN_ENERGY_F1, ID_STRUCT_GEN_ENERGY_F2,
                               ID_STRUCT_GEN_ENERGY_F3, ID_STRUCT_GEN_ENERGY }, SpriteFileStructures,
                              GameObject::TYPE_RES_GEN_ENERGY, OCS_GENERATOR, OCAT_RESOURCES, 2, 2));

    mData.emplace(GameObject::TYPE_RES_GEN_ENERGY_SOLAR,
                  ObjectData({ 4, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0 }, { 250, 250, 0, 0 },
                             { ID_STRUCT_SOLAR_PANEL_F1, ID_STRUCT_SOLAR_PANEL_F2,
                               ID_STRUCT_SOLAR_PANEL_F3, ID_STRUCT_SOLAR_PANEL }, SpriteFileStructures,
                             GameObject::TYPE_RES_GEN_ENERGY_SOLAR, OCS_GENERATOR, OCAT_RESOURCES, 1, 1));

    mData.emplace(GameObject::TYPE_RES_GEN_MATERIAL,
                  ObjectData({ 9, 4, 0, 0, 0, 3, 2, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                             { ID_STRUCT_GEN_MATERIAL_F1, ID_STRUCT_GEN_MATERIAL_F2,
                               ID_STRUCT_GEN_MATERIAL_F3, ID_STRUCT_GEN_MATERIAL}, SpriteFileStructures,
                             GameObject::TYPE_RES_GEN_MATERIAL, OCS_GENERATOR, OCAT_RESOURCES, 2, 2));

    mData.emplace(GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT,
                  ObjectData({ 4, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0 }, { 250, 250, 10, 10 },
                             { ID_MATERIAL_EXTRACTOR_F1, ID_MATERIAL_EXTRACTOR_F2,
                               ID_MATERIAL_EXTRACTOR_F3, ID_MATERIAL_EXTRACTOR }, SpriteFileStructures,
                             GameObject::TYPE_RES_GEN_MATERIAL_EXTRACT, OCS_GENERATOR, OCAT_RESOURCES, 2, 2));

    mData.emplace(GameObject::TYPE_RES_STORAGE_BLOBS,
                  ObjectData({ 3, 2, 0, 0, 0, 2, 3, 4, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { ID_STRUCT_STORAGE_BLOBS_F1, ID_STRUCT_STORAGE_BLOBS_F2,
                               ID_STRUCT_STORAGE_BLOBS_F3, ID_STRUCT_STORAGE_BLOBS },
                             SpriteFileStructures, GameObject::TYPE_RES_STORAGE_BLOBS,
                             OCS_STORAGE, OCAT_RESOURCES, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_DIAMONDS,
                  ObjectData({ 3, 2, 0, 0, 0, 2, 3, 4, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { ID_STRUCT_STORAGE_DIAMONDS_F1, ID_STRUCT_STORAGE_DIAMONDS_F2,
                               ID_STRUCT_STORAGE_DIAMONDS_F3, ID_STRUCT_STORAGE_DIAMONDS },
                             SpriteFileStructures, GameObject::TYPE_RES_STORAGE_DIAMONDS,
                             OCS_STORAGE, OCAT_RESOURCES, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_ENERGY,
                  ObjectData({ 3, 2, 0, 0, 0, 2, 3, 4, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { ID_STRUCT_STORAGE_ENERGY_F1, ID_STRUCT_STORAGE_ENERGY_F2,
                               ID_STRUCT_STORAGE_ENERGY_F3, ID_STRUCT_STORAGE_ENERGY },
                             SpriteFileStructures, GameObject::TYPE_RES_STORAGE_ENERGY,
                             OCS_STORAGE, OCAT_RESOURCES, 1, 1));

    mData.emplace(GameObject::TYPE_RES_STORAGE_MATERIAL,
                  ObjectData({ 3, 2, 0, 0, 0, 2, 3, 4, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { ID_STRUCT_STORAGE_MATERIAL_F1, ID_STRUCT_STORAGE_MATERIAL_F2,
                               ID_STRUCT_STORAGE_MATERIAL_F3, ID_STRUCT_STORAGE_MATERIAL },
                             SpriteFileStructures, GameObject::TYPE_RES_STORAGE_MATERIAL,
                             OCS_STORAGE, OCAT_RESOURCES, 1, 1));

    mData.emplace(GameObject::TYPE_ROCKS,
                  ObjectData({}, {}, { ROCKS_ROW_1 }, SpriteRocksFile,
                             GameObject::TYPE_ROCKS, OCO_SCENE, OCAT_UNDEFINED, 1, 1));

    mData.emplace(GameObject::TYPE_TEMPLE,
                  ObjectData({ 8, 4, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                             { ID_STRUCT_TEMPLE_F1, ID_STRUCT_TEMPLE_F2, ID_STRUCT_TEMPLE_F3,
                               ID_STRUCT_TEMPLE}, SpriteFileStructures, GameObject::TYPE_TEMPLE,
                              OCS_RELICS, OCAT_GENERIC, 3, 3));

    // GENERIC
    mData.emplace(GameObject::TYPE_TRADING_POST,
                  ObjectData({ 5, 4, 0, 0, 0, 4, 2, 2 }, { 1000, 1000, 0, 0 },
                             { ID_STRUCT_TRADING_POST_F1, ID_STRUCT_TRADING_POST_F2,
                              ID_STRUCT_TRADING_POST_F3, ID_STRUCT_TRADING_POST}, SpriteFileStructures,
                             GameObject::TYPE_TRADING_POST, OCS_TRADING, OCAT_GENERIC, 3, 3));

    mData.emplace(GameObject::TYPE_TREES,
                  ObjectData({}, {}, { TREE1_1T_1 }, SpriteFileTrees,
                             GameObject::TYPE_TREES, OCO_SCENE, OCAT_UNDEFINED, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_MEDIC1,
                  ObjectData({ 6, 5, 0, 0, 0, 6, 4, 4, 2, 0, 0, 5 }, { 500, 500, 0, 0 },
                             { SPR_UNIT_05_F1, SPR_UNIT_05_F2, SPR_UNIT_05_F3}, SpriteFileUnits,
                             GameObject::TYPE_UNIT_MEDIC1, OCU_MEDIC, OCAT_UNIT, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SCOUT1,
                  ObjectData({ 4, 4, 1, 2, 2, 4, 2, 2, 5, 2, 3, 0 }, { 300, 300, 0, 0 },
                             { SPR_UNIT_04_F1, SPR_UNIT_04_F3, SPR_UNIT_04_F3}, SpriteFileUnits,
                             GameObject::TYPE_UNIT_SCOUT1, OCU_SOLDIER, OCAT_UNIT, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SOLDIER1,
                  ObjectData({ 4, 5, 4, 5, 4, 3, 3, 2, 4, 0, 3, 0 }, { 200, 200, 0, 0 },
                             { SPR_UNIT_02_F1, SPR_UNIT_02_F2, SPR_UNIT_02_F3 }, SpriteFileUnits,
                             GameObject::TYPE_UNIT_SOLDIER1, OCU_SOLDIER, OCAT_UNIT, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_SOLDIER2,
                  ObjectData({ 6, 5, 5, 4, 5, 4, 5, 6, 3, 0, 2, 0 }, { 400, 400, 0, 0 },
                             { SPR_UNIT_03_F1, SPR_UNIT_03_F2, SPR_UNIT_03_F3 }, SpriteFileUnits,
                             GameObject::TYPE_UNIT_SOLDIER2, OCU_SOLDIER, OCAT_UNIT, 1, 1));

    mData.emplace(GameObject::TYPE_UNIT_WORKER1,
                  ObjectData({ 6, 6, 1, 1, 1, 4, 3, 2, 4, 5, 5, 0 }, { 150, 150, 0, 0 },
                             { SPR_UNIT_01_F1, SPR_UNIT_01_F2, SPR_UNIT_01_F3 }, SpriteFileUnits,
                             GameObject::TYPE_UNIT_WORKER1, OCU_WORKER, OCAT_UNIT, 1, 1));

    mData.emplace(GameObject::TYPE_WALL,
                  ObjectData({ 5, 2, 0, 0, 0, 5, 6, 6, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { WALL_L1_F1_HORIZ, WALL_L1_F2_HORIZ, WALL_L1_F3_HORIZ },
                             SpriteFileWalls, GameObject::TYPE_WALL, OCS_WALL, OCAT_DEFENSE, 1, 1));

    mData.emplace(GameObject::TYPE_WALL_GATE,
                  ObjectData({ 5, 2, 0, 0, 0, 6, 5, 5, 0, 0, 0, 0 }, { 100, 100, 0, 0 },
                             { WALL_GATE_L1_F1_HORIZ_CLOSED, WALL_GATE_L1_F2_HORIZ_CLOSED,
                               WALL_GATE_L1_F3_HORIZ_CLOSED }, SpriteFileWalls,
                             GameObject::TYPE_WALL_GATE, OCS_WALL, OCAT_DEFENSE, 1, 1));
}

} // namespace game
