#include "ObjectData.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjectTools/WeaponData.h"

namespace game
{
using h = std::hash<std::string>;

// -- OBJECT TYPE --
const std::string ObjectData::TYPE_STR_BARRACKS("BARRACKS");
const std::string ObjectData::TYPE_STR_BASE("BASE");
const std::string ObjectData::TYPE_STR_BASE_SPOT("BASE_SPOT");
const std::string ObjectData::TYPE_STR_BLOBS("BLOBS");
const std::string ObjectData::TYPE_STR_BUNKER("BUNKER");
const std::string ObjectData::TYPE_STR_CITY_BLOCK("CITY_BLOCK");
const std::string ObjectData::TYPE_STR_DEFENSIVE_TOWER("DEF_TOWER");
const std::string ObjectData::TYPE_STR_DIAMONDS("DIAMONDS");
const std::string ObjectData::TYPE_STR_HOSPITAL("HOSPITAL");
const std::string ObjectData::TYPE_STR_LOOTBOX("LOOTBOX");
const std::string ObjectData::TYPE_STR_LOOTBOX2("LOOTBOX2");
const std::string ObjectData::TYPE_STR_MINI_UNIT1("MINIUNIT1");
const std::string ObjectData::TYPE_STR_MINI_UNIT2("MINIUNIT2");
const std::string ObjectData::TYPE_STR_MOUNTAINS("MOUNTAINS");
const std::string ObjectData::TYPE_STR_PRACTICE_TARGET("TARGET");
const std::string ObjectData::TYPE_STR_RADAR_STATION("RADAR_STATION");
const std::string ObjectData::TYPE_STR_RADAR_TOWER("RADAR_TOWER");
const std::string ObjectData::TYPE_STR_RESEARCH_CENTER("RESEARCH_CENTER");
const std::string ObjectData::TYPE_STR_RES_GEN_ENERGY("RESGEN_ENER");
const std::string ObjectData::TYPE_STR_RES_GEN_ENERGY_SOLAR("RESGEN_SOLAR");
const std::string ObjectData::TYPE_STR_RES_GEN_MATERIAL("RESGEN_MAT");
const std::string ObjectData::TYPE_STR_RES_GEN_MATERIAL_EXTRACT("RESGEN_MAT_EXT");
const std::string ObjectData::TYPE_STR_RES_STORAGE_BLOBS("RESSTOR_BLOBS");
const std::string ObjectData::TYPE_STR_RES_STORAGE_DIAMONDS("RESSTOR_DIAM");
const std::string ObjectData::TYPE_STR_RES_STORAGE_ENERGY("RESSTOR_ENER");
const std::string ObjectData::TYPE_STR_RES_STORAGE_MATERIAL("RESSTOR_MAT");
const std::string ObjectData::TYPE_STR_ROCKS("ROCKS");
const std::string ObjectData::TYPE_STR_SPAWN_TOWER("SPAWN_TOWER");
const std::string ObjectData::TYPE_STR_TEMPLE("TEMPLE");
const std::string ObjectData::TYPE_STR_TRADING_POST("TRADING_POST");
const std::string ObjectData::TYPE_STR_TREES("TREES");
const std::string ObjectData::TYPE_STR_UNIT_MEDIC1("UNIT_MEDIC1");
const std::string ObjectData::TYPE_STR_UNIT_SCOUT1("UNIT_SCOUT1");
const std::string ObjectData::TYPE_STR_UNIT_SOLDIER1("UNIT_SOLDIER1");
const std::string ObjectData::TYPE_STR_UNIT_SOLDIER2("UNIT_SOLDIER2");
const std::string ObjectData::TYPE_STR_UNIT_SPAWNER1("UNIT_SPAWNER1");
const std::string ObjectData::TYPE_STR_UNIT_SPAWNER2("UNIT_SPAWNER2");
const std::string ObjectData::TYPE_STR_UNIT_WORKER1("UNIT_WORKER1");
const std::string ObjectData::TYPE_STR_WALL("WALL");
const std::string ObjectData::TYPE_STR_WALL_GATE("WALL_GATE");

const GameObjectTypeId ObjectData::TYPE_NULL = 0;

const GameObjectTypeId ObjectData::TYPE_BARRACKS = h{}(TYPE_STR_BARRACKS);
const GameObjectTypeId ObjectData::TYPE_BASE = h{}(TYPE_STR_BASE);
const GameObjectTypeId ObjectData::TYPE_BASE_SPOT = h{}(TYPE_STR_BASE_SPOT);
const GameObjectTypeId ObjectData::TYPE_BLOBS = h{}(TYPE_STR_BLOBS);
const GameObjectTypeId ObjectData::TYPE_BUNKER = h{}(TYPE_STR_BUNKER);
const GameObjectTypeId ObjectData::TYPE_CITY_BLOCK = h{}(TYPE_STR_CITY_BLOCK);
const GameObjectTypeId ObjectData::TYPE_DEFENSIVE_TOWER = h{}(TYPE_STR_DEFENSIVE_TOWER);
const GameObjectTypeId ObjectData::TYPE_DIAMONDS = h{}(TYPE_STR_DIAMONDS);
const GameObjectTypeId ObjectData::TYPE_HOSPITAL = h{}(TYPE_STR_HOSPITAL);
const GameObjectTypeId ObjectData::TYPE_LOOTBOX = h{}(TYPE_STR_LOOTBOX);
const GameObjectTypeId ObjectData::TYPE_LOOTBOX2 = h{}(TYPE_STR_LOOTBOX2);
const GameObjectTypeId ObjectData::TYPE_MINI_UNIT1 = h{}(TYPE_STR_MINI_UNIT1);
const GameObjectTypeId ObjectData::TYPE_MINI_UNIT2 = h{}(TYPE_STR_MINI_UNIT2);
const GameObjectTypeId ObjectData::TYPE_MOUNTAINS = h{}(TYPE_STR_MOUNTAINS);
const GameObjectTypeId ObjectData::TYPE_PRACTICE_TARGET = h{}(TYPE_STR_PRACTICE_TARGET);
const GameObjectTypeId ObjectData::TYPE_RADAR_STATION = h{}(TYPE_STR_RADAR_STATION);
const GameObjectTypeId ObjectData::TYPE_RADAR_TOWER = h{}(TYPE_STR_RADAR_TOWER);
const GameObjectTypeId ObjectData::TYPE_RESEARCH_CENTER = h{}(TYPE_STR_RESEARCH_CENTER);
const GameObjectTypeId ObjectData::TYPE_RES_GEN_ENERGY = h{}(TYPE_STR_RES_GEN_ENERGY);
const GameObjectTypeId ObjectData::TYPE_RES_GEN_ENERGY_SOLAR = h{}(TYPE_STR_RES_GEN_ENERGY_SOLAR);
const GameObjectTypeId ObjectData::TYPE_RES_GEN_MATERIAL = h{}(TYPE_STR_RES_GEN_MATERIAL);
const GameObjectTypeId ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT = h{}(TYPE_STR_RES_GEN_MATERIAL_EXTRACT);
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_BLOBS = h{}(TYPE_STR_RES_STORAGE_BLOBS);
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_DIAMONDS = h{}(TYPE_STR_RES_STORAGE_DIAMONDS);
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_ENERGY = h{}(TYPE_STR_RES_STORAGE_ENERGY);
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_MATERIAL = h{}(TYPE_STR_RES_STORAGE_MATERIAL);
const GameObjectTypeId ObjectData::TYPE_ROCKS = h{}(TYPE_STR_ROCKS);
const GameObjectTypeId ObjectData::TYPE_SPAWN_TOWER = h{}(TYPE_STR_SPAWN_TOWER);
const GameObjectTypeId ObjectData::TYPE_TEMPLE = h{}(TYPE_STR_TEMPLE);
const GameObjectTypeId ObjectData::TYPE_TRADING_POST = h{}(TYPE_STR_TRADING_POST);
const GameObjectTypeId ObjectData::TYPE_TREES = h{}(TYPE_STR_TREES);
const GameObjectTypeId ObjectData::TYPE_UNIT_MEDIC1 = h{}(TYPE_STR_UNIT_MEDIC1);
const GameObjectTypeId ObjectData::TYPE_UNIT_SCOUT1 = h{}(TYPE_STR_UNIT_SCOUT1);
const GameObjectTypeId ObjectData::TYPE_UNIT_SOLDIER1 = h{}(TYPE_STR_UNIT_SOLDIER1);
const GameObjectTypeId ObjectData::TYPE_UNIT_SOLDIER2 = h{}(TYPE_STR_UNIT_SOLDIER2);
const GameObjectTypeId ObjectData::TYPE_UNIT_SPAWNER1 = h{}(TYPE_STR_UNIT_SPAWNER1);
const GameObjectTypeId ObjectData::TYPE_UNIT_SPAWNER2 = h{}(TYPE_STR_UNIT_SPAWNER2);
const GameObjectTypeId ObjectData::TYPE_UNIT_WORKER1 = h{}(TYPE_STR_UNIT_WORKER1);
const GameObjectTypeId ObjectData::TYPE_WALL = h{}(TYPE_STR_WALL);
const GameObjectTypeId ObjectData::TYPE_WALL_GATE = h{}(TYPE_STR_WALL_GATE);

const std::unordered_map<GameObjectTypeId, std::string> ObjectData::TYPE_STR_MAP =
{
    { ObjectData::TYPE_BARRACKS, TYPE_STR_BARRACKS},
    { ObjectData::TYPE_BASE, TYPE_STR_BASE},
    { ObjectData::TYPE_BASE_SPOT, TYPE_STR_BASE_SPOT},
    { ObjectData::TYPE_BUNKER, TYPE_STR_BUNKER},
    { ObjectData::TYPE_CITY_BLOCK, TYPE_STR_CITY_BLOCK},
    { ObjectData::TYPE_DEFENSIVE_TOWER, TYPE_STR_DEFENSIVE_TOWER },
    { ObjectData::TYPE_HOSPITAL, TYPE_STR_HOSPITAL },
    { ObjectData::TYPE_LOOTBOX, TYPE_STR_LOOTBOX },
    { ObjectData::TYPE_LOOTBOX2, TYPE_STR_LOOTBOX2 },
    { ObjectData::TYPE_MINI_UNIT1, TYPE_STR_MINI_UNIT1 },
    { ObjectData::TYPE_MINI_UNIT2, TYPE_STR_MINI_UNIT2 },
    { ObjectData::TYPE_MOUNTAINS, TYPE_STR_MOUNTAINS },
    { ObjectData::TYPE_PRACTICE_TARGET, TYPE_STR_PRACTICE_TARGET },
    { ObjectData::TYPE_RADAR_STATION, TYPE_STR_RADAR_STATION },
    { ObjectData::TYPE_RADAR_TOWER, TYPE_STR_RADAR_TOWER },
    { ObjectData::TYPE_RESEARCH_CENTER, TYPE_STR_RESEARCH_CENTER },
    { ObjectData::TYPE_RES_GEN_ENERGY, TYPE_STR_RES_GEN_ENERGY },
    { ObjectData::TYPE_RES_GEN_ENERGY_SOLAR, TYPE_STR_RES_GEN_ENERGY_SOLAR },
    { ObjectData::TYPE_RES_GEN_MATERIAL, TYPE_STR_RES_GEN_MATERIAL },
    { ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT, TYPE_STR_RES_GEN_MATERIAL_EXTRACT },
    { ObjectData::TYPE_RES_STORAGE_BLOBS, TYPE_STR_RES_STORAGE_BLOBS },
    { ObjectData::TYPE_RES_STORAGE_DIAMONDS, TYPE_STR_RES_STORAGE_DIAMONDS },
    { ObjectData::TYPE_RES_STORAGE_ENERGY, TYPE_STR_RES_STORAGE_ENERGY },
    { ObjectData::TYPE_RES_STORAGE_MATERIAL, TYPE_STR_RES_STORAGE_MATERIAL },
    { ObjectData::TYPE_ROCKS, TYPE_STR_ROCKS },
    { ObjectData::TYPE_SPAWN_TOWER, TYPE_STR_SPAWN_TOWER },
    { ObjectData::TYPE_TEMPLE, TYPE_STR_TEMPLE },
    { ObjectData::TYPE_TREES, TYPE_STR_TREES },
    { ObjectData::TYPE_TRADING_POST, TYPE_STR_TRADING_POST },
    { ObjectData::TYPE_UNIT_MEDIC1, TYPE_STR_UNIT_MEDIC1 },
    { ObjectData::TYPE_UNIT_SCOUT1, TYPE_STR_UNIT_SCOUT1 },
    { ObjectData::TYPE_UNIT_SOLDIER1, TYPE_STR_UNIT_SOLDIER1 },
    { ObjectData::TYPE_UNIT_SOLDIER2, TYPE_STR_UNIT_SOLDIER2 },
    { ObjectData::TYPE_UNIT_SPAWNER1, TYPE_STR_UNIT_SPAWNER1 },
    { ObjectData::TYPE_UNIT_SPAWNER2, TYPE_STR_UNIT_SPAWNER2 },
    { ObjectData::TYPE_UNIT_WORKER1, TYPE_STR_UNIT_WORKER1 },
    { ObjectData::TYPE_WALL, TYPE_STR_WALL },
    { ObjectData::TYPE_WALL_GATE, TYPE_STR_WALL_GATE }
};


const std::string & ObjectData::GetObjectTypeStr(const GameObjectTypeId type)
{
    static const std::string noStr;

    auto it = TYPE_STR_MAP.find(type);

    if(it != TYPE_STR_MAP.end())
        return it->second;
    else
        return noStr;
}

const std::unordered_map<GameObjectTypeId, std::string> ObjectData::TITLES =
{
    { ObjectData::TYPE_BARRACKS, "BARRACKS"},
    { ObjectData::TYPE_BASE, "BASE"},
    { ObjectData::TYPE_BASE_SPOT, "BASE_SPOT"},
    { ObjectData::TYPE_BUNKER, "BUNKER"},
    { ObjectData::TYPE_CITY_BLOCK, "CITY_BLOCK"},
    { ObjectData::TYPE_DEFENSIVE_TOWER, "DEF_TOWER"},
    { ObjectData::TYPE_HOSPITAL, "HOSPITAL"},
    { ObjectData::TYPE_LOOTBOX, "LOOTBOX"},
    { ObjectData::TYPE_LOOTBOX2, "LOOTBOX2"},
    { ObjectData::TYPE_MINI_UNIT1, "MINI_UNIT1"},
    { ObjectData::TYPE_MINI_UNIT2, "MINI_UNIT2"},
    { ObjectData::TYPE_MOUNTAINS, "MOUNTAINS"},
    { ObjectData::TYPE_PRACTICE_TARGET, "PRACTICE_TARGET"},
    { ObjectData::TYPE_RADAR_STATION, "RADAR_STATION"},
    { ObjectData::TYPE_RADAR_TOWER, "RADAR_TOWER"},
    { ObjectData::TYPE_RESEARCH_CENTER, "RESEARCH_CENTER"},
    { ObjectData::TYPE_RES_GEN_ENERGY, "RESGEN_ENERGY"},
    { ObjectData::TYPE_RES_GEN_ENERGY_SOLAR, "RESGEN_ENERGY_SOLAR"},
    { ObjectData::TYPE_RES_GEN_MATERIAL, "RESGEN_MATERIAL"},
    { ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT, "RESGEN_MATERIAL_EXTRACT"},
    { ObjectData::TYPE_RES_STORAGE_BLOBS, "RES_STORAGE_BLOBS"},
    { ObjectData::TYPE_RES_STORAGE_DIAMONDS, "RES_STORAGE_DIAMONDS"},
    { ObjectData::TYPE_RES_STORAGE_ENERGY, "RES_STORAGE_ENERGY"},
    { ObjectData::TYPE_RES_STORAGE_MATERIAL, "RES_STORAGE_MATERIAL"},
    { ObjectData::TYPE_ROCKS, "ROCKS"},
    { ObjectData::TYPE_SPAWN_TOWER, "SPAWN_TOWER"},
    { ObjectData::TYPE_TEMPLE, "TEMPLE"},
    { ObjectData::TYPE_TRADING_POST, "TRADING_POST"},
    { ObjectData::TYPE_TREES, "TREES"},
    { ObjectData::TYPE_UNIT_MEDIC1, "UNIT_MEDIC1"},
    { ObjectData::TYPE_UNIT_SCOUT1, "UNIT_SCOUT1"},
    { ObjectData::TYPE_UNIT_SOLDIER1, "UNIT_SOLDIER1"},
    { ObjectData::TYPE_UNIT_SOLDIER2, "UNIT_SOLDIER2"},
    { ObjectData::TYPE_UNIT_SPAWNER1, "UNIT_SPAWNER1"},
    { ObjectData::TYPE_UNIT_SPAWNER2, "UNIT_SPAWNER2"},
    { ObjectData::TYPE_UNIT_WORKER1, "UNIT_WORKER1"},
    { ObjectData::TYPE_WALL, "WALL"},
    { ObjectData::TYPE_WALL_GATE, "GATE"}
};

const std::unordered_map<GameObjectTypeId, std::string> ObjectData::DESCRIPTIONS =
{
        { ObjectData::TYPE_BARRACKS, "DESC_BARRACKS"},
        { ObjectData::TYPE_BASE, "DESC_BASE"},
        { ObjectData::TYPE_BASE_SPOT, "DESC_BASE_SPOT"},
        { ObjectData::TYPE_BUNKER, "DESC_BUNKER"},
        { ObjectData::TYPE_CITY_BLOCK, "DESC_CITY_BLOCK"},
        { ObjectData::TYPE_DEFENSIVE_TOWER, "DESC_DEFENSIVE_TOWER"},
        { ObjectData::TYPE_HOSPITAL, "DESC_HOSPITAL"},
        { ObjectData::TYPE_LOOTBOX, "DESC_LOOTBOX"},
        { ObjectData::TYPE_LOOTBOX2, "DESC_LOOTBOX2"},
        { ObjectData::TYPE_MINI_UNIT1, "DESC_MINI_UNIT1"},
        { ObjectData::TYPE_MINI_UNIT2, "DESC_MINI_UNIT2"},
        { ObjectData::TYPE_MOUNTAINS, "DESC_MOUNTAINS"},
        { ObjectData::TYPE_PRACTICE_TARGET, "DESC_PRACTICE_TARGET"},
        { ObjectData::TYPE_RADAR_STATION, "DESC_RADAR_STATION"},
        { ObjectData::TYPE_RADAR_TOWER, "DESC_RADAR_TOWER"},
        { ObjectData::TYPE_RESEARCH_CENTER, "DESC_RESEARCH_CENTER"},
        { ObjectData::TYPE_RES_GEN_ENERGY, "DESC_RES_GEN_ENERGY"},
        { ObjectData::TYPE_RES_GEN_ENERGY_SOLAR, "DESC_RES_GEN_ENERGY_SOLAR"},
        { ObjectData::TYPE_RES_GEN_MATERIAL, "DESC_RES_GEN_MATERIAL"},
        { ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT, "DESC_RES_GEN_MATERIAL_EXTRACT"},
        { ObjectData::TYPE_RES_STORAGE_BLOBS, "DESC_RES_STORAGE_BLOBS"},
        { ObjectData::TYPE_RES_STORAGE_DIAMONDS, "DESC_RES_STORAGE_DIAMONDS"},
        { ObjectData::TYPE_RES_STORAGE_ENERGY, "DESC_RES_STORAGE_ENERGY"},
        { ObjectData::TYPE_RES_STORAGE_MATERIAL, "DESC_RES_STORAGE_MATERIAL"},
        { ObjectData::TYPE_ROCKS, "DESC_ROCKS"},
        { ObjectData::TYPE_SPAWN_TOWER, "DESC_SPAWN_TOWER"},
        { ObjectData::TYPE_TEMPLE, "DESC_TEMPLE"},
        { ObjectData::TYPE_TREES, "DESC_TREES"},
        { ObjectData::TYPE_TRADING_POST, "DESC_TRADING_POST"},
        { ObjectData::TYPE_UNIT_MEDIC1, "DESC_UNIT_MEDIC1"},
        { ObjectData::TYPE_UNIT_SCOUT1, "DESC_UNIT_SCOUT1"},
        { ObjectData::TYPE_UNIT_SOLDIER1, "DESC_UNIT_SOLDIER1"},
        { ObjectData::TYPE_UNIT_SOLDIER2, "DESC_UNIT_SOLDIER2"},
        { ObjectData::TYPE_UNIT_SPAWNER1, "DESC_UNIT_SPAWNER1"},
        { ObjectData::TYPE_UNIT_SPAWNER2, "DESC_UNIT_SPAWNER2"},
        { ObjectData::TYPE_UNIT_WORKER1, "DESC_UNIT_WORKER1"},
        { ObjectData::TYPE_WALL, "DESC_WALL"},
        { ObjectData::TYPE_WALL_GATE, "DESC_WALL_GATE"}
};

// -- OBJECT CATEGORY --
const GameObjectCategoryId ObjectData::CAT_NULL = 0;

const GameObjectCategoryId ObjectData::CAT_COLLECTABLE = h{}("COLLECTABLE");
const GameObjectCategoryId ObjectData::CAT_MINI_UNIT = h{}("MINI_UNIT");
const GameObjectCategoryId ObjectData::CAT_RES_GENERATOR = h{}("RES_GEN");
const GameObjectCategoryId ObjectData::CAT_RES_STORAGE = h{}("RES_STORAGE");
const GameObjectCategoryId ObjectData::CAT_SCENE_OBJ = h{}("SCENE_OBJ");
const GameObjectCategoryId ObjectData::CAT_STRUCTURE = h{}("STRUCTURE");
const GameObjectCategoryId ObjectData::CAT_UNIT = h{}("UNIT");


const char * ObjectData::STR_CLASS[NUM_OBJ_CLASSES] =
{
    // STRUCTURES
    "BARRACKS",
    "BASE",
    "GENERATOR",
    "HOSPITAL",
    "RADAR",
    "RELICS",
    "RESEARCH",
    "WAREHOUSE",
    "TARGET",
    "TOWER",
    "TRADING",
    "WALL",
    "GATE",

    // MINI UNITS
    "SOLDIER",

    // UNITS
    "MEDIC",
    "SOLDIER",
    "SPAWNER",
    "WORKER",

    // OTHERS
    "COLLECTABLE",
    "SCENE"
};

// WEAPON
const char * ObjectData::STR_ATTRIBUTES[NUM_OBJ_ATTRIBUTES] =
{
    // GENERIC
    "ENERGY",
    "HEALTH",
    "VIEW_RANGE",
    "REGENERATION",
    "RESISTANCE",
    "SHIELD",
    // UNIT
    "SPEED",
    "CONSTRUCTION",
    "CONQUEST",
    "HEALING_RANGE",
    "HEALING_POWER",
    "SPAWNING",
    // RESOURCE STORAGE
    "STORAGE",
    // WEAPON
    "ATTACK_ACCURACY",
    "ATTACK_POWER",
    "ATTACK_RANGE",
};

const ObjectData ObjectData::NullObj({}, {}, {}, nullptr, ObjectData::TYPE_NULL, ObjectData::CAT_NULL,
                                     OC_NULL, OCAT_UNDEFINED, WeaponData::TYPE_NULL, 0, 0);

unsigned int ObjectData::GetIconTexId(PlayerFaction f, const GameObject * obj) const
{
    if(f >= mIconTexIds.size())
        return mIconTexIds.back();

    if(obj != nullptr && mCategory == CAT_MINI_UNIT)
    {
        const auto mu = static_cast<const MiniUnit *>(obj);
        const unsigned int texInd0 = mIconTexIds[f];
        return  texInd0 + NUM_MUNIT_SPRITES_PER_SQUAD * (mu->GetNumElements() - 1);
    }
    else
        return mIconTexIds[f];
}

} // namespace game
