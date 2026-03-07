#include "ObjectData.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjectTools/WeaponData.h"

namespace game
{
// -- OBJECT TYPE --
const GameObjectTypeId ObjectData::TYPE_NULL = 0;

const GameObjectTypeId ObjectData::TYPE_BARRACKS = 13257775875673419068u;
const GameObjectTypeId ObjectData::TYPE_BASE = 9094196730522238043u;
const GameObjectTypeId ObjectData::TYPE_BASE_SPOT = 14134897545835206898u;
const GameObjectTypeId ObjectData::TYPE_BLOBS = 1941259080517359621u;
const GameObjectTypeId ObjectData::TYPE_BUNKER = 499816459377098859u;
const GameObjectTypeId ObjectData::TYPE_CITY_BLOCK = 17708476866137851777u;
const GameObjectTypeId ObjectData::TYPE_DEFENSIVE_TOWER = 15727419491481753576u;
const GameObjectTypeId ObjectData::TYPE_DIAMONDS = 4977511725422730637u;
const GameObjectTypeId ObjectData::TYPE_HOSPITAL = 6200336039472478830u;
const GameObjectTypeId ObjectData::TYPE_LOOTBOX = 11950838591583399786u;
const GameObjectTypeId ObjectData::TYPE_LOOTBOX2 = 10560136022578840531u;
const GameObjectTypeId ObjectData::TYPE_MINI_UNIT1 = 11607141519420994464u;
const GameObjectTypeId ObjectData::TYPE_MINI_UNIT2 = 548432711892482871u;
const GameObjectTypeId ObjectData::TYPE_MOUNTAINS = 5129307706687418303u;
const GameObjectTypeId ObjectData::TYPE_PRACTICE_TARGET = 17051503364827447577u;
const GameObjectTypeId ObjectData::TYPE_RADAR_STATION = 5616405813767426356u;
const GameObjectTypeId ObjectData::TYPE_RADAR_TOWER = 8134168089495879870u;
const GameObjectTypeId ObjectData::TYPE_RESEARCH_CENTER = 17316365153700919911u;
const GameObjectTypeId ObjectData::TYPE_RES_GEN_ENERGY = 16217113543901141587u;
const GameObjectTypeId ObjectData::TYPE_RES_GEN_ENERGY_SOLAR = 2665703515310069325u;
const GameObjectTypeId ObjectData::TYPE_RES_GEN_MATERIAL = 12053102171863802704u;
const GameObjectTypeId ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT = 284025962403672852u;
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_BLOBS = 5908792393179120300u;
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_DIAMONDS = 1091758047440634538u;
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_ENERGY = 13096021402464255316u;
const GameObjectTypeId ObjectData::TYPE_RES_STORAGE_MATERIAL = 7642339922187204949u;
const GameObjectTypeId ObjectData::TYPE_ROCKS = 642473717644353764u;
const GameObjectTypeId ObjectData::TYPE_SPAWN_TOWER = 3913629860324638827u;
const GameObjectTypeId ObjectData::TYPE_TEMPLE = 5444014183845484671u;
const GameObjectTypeId ObjectData::TYPE_TRADING_POST = 8633289264715340838u;
const GameObjectTypeId ObjectData::TYPE_TREES = 2838858888503128034u;
const GameObjectTypeId ObjectData::TYPE_UNIT_MEDIC1 = 11104197402347881621u;
const GameObjectTypeId ObjectData::TYPE_UNIT_SCOUT1 = 5683934829746184003u;
const GameObjectTypeId ObjectData::TYPE_UNIT_SOLDIER1 = 4212048785386180u;
const GameObjectTypeId ObjectData::TYPE_UNIT_SOLDIER2 = 9380430909724697381u;
const GameObjectTypeId ObjectData::TYPE_UNIT_SPAWNER1 = 4914265752968477164u;
const GameObjectTypeId ObjectData::TYPE_UNIT_SPAWNER2 = 11500951537051146772u;
const GameObjectTypeId ObjectData::TYPE_UNIT_WORKER1 = 6957508224252226271u;
const GameObjectTypeId ObjectData::TYPE_WALL = 17023645684236267704u;
const GameObjectTypeId ObjectData::TYPE_WALL_GATE = 18263604905592984660u;

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

const GameObjectCategoryId ObjectData::CAT_COLLECTABLE = 9419833046200742204u;
const GameObjectCategoryId ObjectData::CAT_MINI_UNIT = 13146483742249843079u;
const GameObjectCategoryId ObjectData::CAT_RES_GENERATOR = 7012214588672144715u;
const GameObjectCategoryId ObjectData::CAT_RES_STORAGE = 12817435265416001740u;
const GameObjectCategoryId ObjectData::CAT_SCENE_OBJ = 16719829775621082597u;
const GameObjectCategoryId ObjectData::CAT_STRUCTURE = 17957022383009386536u;
const GameObjectCategoryId ObjectData::CAT_UNIT = 2375135268229788161u;

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

const char * ObjectData::STR_ATTRIBUTE_TOOLTIPS[NUM_OBJ_ATTRIBUTES] =
{
    // GENERIC
    "TT_ATT_ENERGY",
    "TT_ATT_HEALTH",
    "TT_ATT_VIEW_RANGE",
    "TT_ATT_REGENERATION",
    "TT_ATT_RESISTANCE",
    "TT_ATT_SHIELD",
    // UNIT
    "TT_ATT_SPEED",
    "TT_ATT_CONSTRUCTION",
    "TT_ATT_CONQUEST",
    "TT_ATT_HEALING_RANGE",
    "TT_ATT_HEALING_POWER",
    "TT_ATT_SPAWNING",
    // RESOURCE STORAGE
    "TT_ATT_STORAGE",
    // WEAPON
    "TT_ATT_ATTACK_ACCURACY",
    "TT_ATT_ATTACK_POWER",
    "TT_ATT_ATTACK_RANGE",
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
