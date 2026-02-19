#pragma once

#include "GameConstants.h"
#include "GameObjectTypes.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

namespace game
{

class GameObject;

enum ObjClass : unsigned int
{
    // STRUCTURES
    OCS_BARRACKS,
    OCS_BASE,
    OCS_GENERATOR,
    OCS_HOSPITAL,
    OCS_RADAR,
    OCS_RELICS,
    OCS_RESEARCH,
    OCS_STORAGE,
    OCS_TARGET,
    OCS_TOWER,
    OCS_TRADING,
    OCS_WALL,
    OCS_WALL_GATE,

    // MINI UNITS
    OCMU_SOLDIER,

    // UNITS
    OCU_MEDIC,
    OCU_SOLDIER,
    OCU_SPAWNER,
    OCU_WORKER,

    // OTHERS
    OCO_COLLECTABLE,
    OCO_SCENE,

    NUM_OBJ_CLASSES,

    OC_NULL
};

enum ObjCost : unsigned int
{
    OBJ_COST_ENERGY,
    OBJ_COST_MATERIAL,
    OBJ_COST_BLOBS,
    OBJ_COST_DIAMONDS,

    NUM_OBJ_COSTS
};

enum ObjFamily : unsigned int
{
    // STRUCTURE
    OCAT_GENERIC,
    OCAT_DEFENSE,
    OCAT_RESOURCES,
    OCAT_TECHNOLOGY,

    // UNIT
    OCAT_MINI_UNIT,
    OCAT_UNIT,

    NUM_OBJ_CATEGORIES,

    OCAT_UNDEFINED
};

enum PlayerFaction : unsigned int;

class ObjectData
{
public:
    // -- OBJECT TYPE --
    static const GameObjectTypeId TYPE_NULL;

    static const GameObjectTypeId TYPE_BARRACKS;
    static const GameObjectTypeId TYPE_BASE;
    static const GameObjectTypeId TYPE_BASE_SPOT;
    static const GameObjectTypeId TYPE_BLOBS;
    static const GameObjectTypeId TYPE_BUNKER;
    static const GameObjectTypeId TYPE_CITY_BLOCK;
    static const GameObjectTypeId TYPE_DEFENSIVE_TOWER;
    static const GameObjectTypeId TYPE_DIAMONDS;
    static const GameObjectTypeId TYPE_HOSPITAL;
    static const GameObjectTypeId TYPE_LOOTBOX;
    static const GameObjectTypeId TYPE_LOOTBOX2;
    static const GameObjectTypeId TYPE_MINI_UNIT1;
    static const GameObjectTypeId TYPE_MINI_UNIT2;
    static const GameObjectTypeId TYPE_MOUNTAINS;
    static const GameObjectTypeId TYPE_PRACTICE_TARGET;
    static const GameObjectTypeId TYPE_RADAR_STATION;
    static const GameObjectTypeId TYPE_RADAR_TOWER;
    static const GameObjectTypeId TYPE_RESEARCH_CENTER;
    static const GameObjectTypeId TYPE_RES_GEN_ENERGY;
    static const GameObjectTypeId TYPE_RES_GEN_ENERGY_SOLAR;
    static const GameObjectTypeId TYPE_RES_GEN_MATERIAL;
    static const GameObjectTypeId TYPE_RES_GEN_MATERIAL_EXTRACT;
    static const GameObjectTypeId TYPE_RES_STORAGE_BLOBS;
    static const GameObjectTypeId TYPE_RES_STORAGE_DIAMONDS;
    static const GameObjectTypeId TYPE_RES_STORAGE_ENERGY;
    static const GameObjectTypeId TYPE_RES_STORAGE_MATERIAL;
    static const GameObjectTypeId TYPE_ROCKS;
    static const GameObjectTypeId TYPE_SPAWN_TOWER;
    static const GameObjectTypeId TYPE_TEMPLE;
    static const GameObjectTypeId TYPE_TRADING_POST;
    static const GameObjectTypeId TYPE_TREES;
    static const GameObjectTypeId TYPE_UNIT_MEDIC1;
    static const GameObjectTypeId TYPE_UNIT_SCOUT1;
    static const GameObjectTypeId TYPE_UNIT_SOLDIER1;
    static const GameObjectTypeId TYPE_UNIT_SOLDIER2;
    static const GameObjectTypeId TYPE_UNIT_SPAWNER1;
    static const GameObjectTypeId TYPE_UNIT_SPAWNER2;
    static const GameObjectTypeId TYPE_UNIT_WORKER1;
    static const GameObjectTypeId TYPE_WALL;
    static const GameObjectTypeId TYPE_WALL_GATE;

    static const std::unordered_map<GameObjectTypeId, std::string> TITLES;
    static const std::unordered_map<GameObjectTypeId, std::string> DESCRIPTIONS;

    // -- OBJECT CATEGORY --
    static const GameObjectCategoryId CAT_NULL;

    static const GameObjectCategoryId CAT_COLLECTABLE;
    static const GameObjectCategoryId CAT_MINI_UNIT;
    static const GameObjectCategoryId CAT_RES_GENERATOR;
    static const GameObjectCategoryId CAT_RES_STORAGE;
    static const GameObjectCategoryId CAT_SCENE_OBJ;
    static const GameObjectCategoryId CAT_STRUCTURE;
    static const GameObjectCategoryId CAT_UNIT;

    static const char * STR_CLASS[NUM_OBJ_CLASSES];
    static const char * STR_ATTRIBUTES[NUM_OBJ_ATTRIBUTES];

    static const int MAX_STAT_VAL = 10;

    static const ObjectData NullObj;

public:
    ObjectData(const std::unordered_map<ObjAttId, int> & atts,
               const std::array<int, NUM_OBJ_COSTS> & costs,
               const std::vector<unsigned int> & texIds,
               const char * file, GameObjectTypeId type,
               GameObjectCategoryId cat, ObjClass oClass,
               ObjFamily family, WeaponType weapon,
               unsigned int rows, unsigned int cols);

    const std::unordered_map<ObjAttId, int> & GetAttributes() const;
    int GetAttribute(ObjAttId attID) const;
    const std::array<int, NUM_OBJ_COSTS> & GetCosts() const;

    unsigned int GetIconTexId(PlayerFaction f, const GameObject * obj = nullptr) const;
    const char * GetIconTexFile() const;

    GameObjectTypeId GetType() const;
    GameObjectCategoryId GetCategory() const;
    ObjClass GetClass() const;
    ObjFamily GetFamily() const;

    WeaponType GetWeapon() const;

    int GetRows() const;
    int GetCols() const;

private:
    std::unordered_map<ObjAttId, int> mAttributes;
    std::array<int, NUM_OBJ_COSTS> mCosts;

    std::vector<unsigned int> mIconTexIds;
    const char * mIconTexFile = nullptr;

    GameObjectTypeId mType;
    GameObjectCategoryId mCategory;
    ObjClass mClass;
    ObjFamily mFamily;

    WeaponType mWeapon;

    unsigned int mRows;
    unsigned int mCols;
};

inline ObjectData::ObjectData(const std::unordered_map<ObjAttId, int> & atts,
                              const std::array<int, NUM_OBJ_COSTS> & costs,
                              const std::vector<unsigned int> & texIds,
                              const char * file, GameObjectTypeId type, GameObjectCategoryId cat,
                              ObjClass oClass, ObjFamily family, WeaponType weapon,
                              unsigned int rows, unsigned int cols)
    : mAttributes(atts)
    , mCosts(costs)
    , mIconTexIds(texIds)
    , mIconTexFile(file)
    , mType(type)
    , mCategory(cat)
    , mClass(oClass)
    , mFamily(family)
    , mWeapon(weapon)
    , mRows(rows)
    , mCols(cols)
{
}

inline const std::unordered_map<ObjAttId, int> & ObjectData::GetAttributes() const
{
    return mAttributes;
}

inline int ObjectData::GetAttribute(ObjAttId attID) const
{
    const auto it = mAttributes.find(attID);

    return (it != mAttributes.end()) ? it->second : 0;
}

inline const std::array<int, NUM_OBJ_COSTS> & ObjectData::GetCosts() const
{
    return mCosts;
}

inline const char * ObjectData::GetIconTexFile() const { return mIconTexFile; }

inline GameObjectTypeId ObjectData::GetType() const { return mType; }
inline GameObjectCategoryId ObjectData::GetCategory() const { return mCategory; }
inline ObjClass ObjectData::GetClass() const { return mClass; }
inline ObjFamily ObjectData::GetFamily() const { return mFamily; }

inline WeaponType ObjectData::GetWeapon() const { return mWeapon; }

inline int ObjectData::GetRows() const { return mRows; }
inline int ObjectData::GetCols() const { return mCols; }

} // namespace game
