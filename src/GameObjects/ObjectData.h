#pragma once

#include "GameObjectTypes.h"

#include <array>
#include <vector>

namespace game
{

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
    OCS_WALL,
    OCS_WALL_GATE,

    // UNITS
    OCU_GENERIC,
    OCU_MEDIC,
    OCU_SOLDIER,
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

enum ObjAttId : unsigned int
{
    // GENERIC
    OBJ_ATT_ENERGY,
    OBJ_ATT_VIEW_RADIUS,
    OBJ_ATT_FIRE_POWER,
    OBJ_ATT_FIRE_ACCURACY,
    OBJ_ATT_FIRE_RANGE,
    OBJ_ATT_REGENERATION,
    OBJ_ATT_RESISTANCE,
    OBJ_ATT_SHIELD,

    NUM_GEN_OBJ_STATS,

    // UNIT ONLY
    OBJ_ATT_SPEED = NUM_GEN_OBJ_STATS,
    OBJ_ATT_CONSTRUCTION,
    OBJ_ATT_CONQUEST,
    OBJ_ATT_HEALING,

    NUM_OBJ_ATTRIBUTES,
};

enum ObjFamily : unsigned int
{
    // STRUCTURE
    OCAT_GENERIC,
    OCAT_DEFENSE,
    OCAT_RESOURCES,
    OCAT_TECHNOLOGY,

    // UNIT
    OCAT_UNIT,

    NUM_OBJ_CATEGORIES,

    OCAT_UNDEFINED
};

class ObjectData
{
public:
    static const char * STR_CLASS[NUM_OBJ_CLASSES];
    static const char * STR_STAT[NUM_OBJ_ATTRIBUTES];

    static const int MAX_STAT_VAL = 10;

    static const ObjectData NullObj;

public:
    ObjectData(const std::array<int, NUM_OBJ_ATTRIBUTES> & atts,
               const std::array<int, NUM_OBJ_COSTS> & costs,
               const std::vector<unsigned int> & texIds,
               const char * file, GameObjectTypeId type, ObjClass oClass,
               ObjFamily family,
               unsigned int rows, unsigned int cols);

    const std::array<int, NUM_OBJ_ATTRIBUTES> & GetAttributes() const;
    const std::array<int, NUM_OBJ_COSTS> & GetCosts() const;

    const std::vector<unsigned int> & GetTexIds() const;
    const char * GetTextureFile() const;

    GameObjectTypeId GetType() const;
    ObjClass GetClass() const;
    ObjFamily GetFamily() const;

    int GetRows() const;
    int GetCols() const;

private:
    std::array<int, NUM_OBJ_ATTRIBUTES> mAttributes;
    std::array<int, NUM_OBJ_COSTS> mCosts;

    std::vector<unsigned int> mTexIds;
    const char * mTexFile = nullptr;

    GameObjectTypeId mType;
    ObjClass mClass;
    ObjFamily mFamily;

    unsigned int mRows;
    unsigned int mCols;
};

inline ObjectData::ObjectData(const std::array<int, NUM_OBJ_ATTRIBUTES> & atts,
                              const std::array<int, NUM_OBJ_COSTS> & costs,
                              const std::vector<unsigned int> & texIds,
                              const char * file, GameObjectTypeId type, ObjClass oClass,
                              ObjFamily family, unsigned int rows, unsigned int cols)
    : mAttributes(atts)
    , mCosts(costs)
    , mTexIds(texIds)
    , mTexFile(file)
    , mType(type)
    , mClass(oClass)
    , mFamily(family)
    , mRows(rows)
    , mCols(cols)
{
}

inline const std::array<int, NUM_OBJ_ATTRIBUTES> & ObjectData::GetAttributes() const
{
    return mAttributes;
}

inline const std::array<int, NUM_OBJ_COSTS> & ObjectData::GetCosts() const
{
    return mCosts;
}

inline const std::vector<unsigned int> & ObjectData::GetTexIds() const
{
    return mTexIds;
}
inline const char * ObjectData::GetTextureFile() const { return mTexFile; }

inline GameObjectTypeId ObjectData::GetType() const { return mType; }
inline ObjClass ObjectData::GetClass() const { return mClass; }
inline ObjFamily ObjectData::GetFamily() const { return mFamily; }

inline int ObjectData::GetRows() const { return mRows; }
inline int ObjectData::GetCols() const { return mCols; }

} // namespace game
