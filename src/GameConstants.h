#pragma once

namespace game
{

constexpr int MAX_NUM_PLAYERS = 4;

constexpr int MAX_UNITS_LEVEL = 2;
constexpr int COST_UNIT_UPGRADE[] = { 1, 2 };

constexpr float TIME_AI_MIN = 0.2f;
constexpr float TIME_GOD_MODE = 0.25f;

constexpr float TIME_AUTO_END_TURN = 2.f;

constexpr float MAX_STAT_FVAL = 10.f;
constexpr int MAX_STAT_IVAL = 10;

constexpr unsigned int PLAYER_COLOR[] =
{
    0xdb5757ff,
    0x57db62ff,
    0x57badbff,
    0xdb57dbff
};

enum AttackMode : unsigned int
{
    ATT_QUICK_SHOT,
    ATT_AIMED_SHOT,
    ATT_BURST_SHOT,

    NUM_ATTACK_MODES
};

enum GameCursorId : unsigned int
{
    CURSOR_DEFAULT,

    NUM_GAME_CURSORS,

    CURSOR_NULL
};

enum LanguageId : unsigned int
{
    LANG_ENGLISH,
    LANG_ITALIAN,
    LANG_SPANISH,

    NUM_LANGUAGES,

    LANG_NULL
};

enum MapLayers : unsigned int
{
    CELL_OVERLAYS1,
    CELL_OVERLAYS2,
    CELL_OVERLAYS3,
    FACTION_INFLUENCE,
    // used for objects below the regular ones, like Gate when closed
    GROUND_OBJECTS,
    // normal objects
    REGULAR_OBJECTS,
    CELL_OVERLAYS4,

    NUM_LAYERS,
};

// mission category represents the main goal of a mission
enum MissionCategory : unsigned int
{
    MC_COLLECTION,      // collect resources
    MC_CONQUEST,        // conquest
    MC_CREATION,        // build something
    MC_DESTRUCTION,     // destroy enemies
    MC_PRODUCTION,      // produce resources
    MC_RESCUE,          // rescue something
    MC_RESISTANCE,      // hold for some time
    MC_TUTORIAL,        // game tutorial

    NUM_MISSION_CATEGORIES,

    MISSION_COMPLETED,

    MC_UNKNOWN
};

enum ObjAttId : unsigned int
{
    // GENERIC
    OBJ_ATT_ENERGY,
    OBJ_ATT_HEALTH,
    OBJ_ATT_VIEW_RANGE,
    OBJ_ATT_REGENERATION,
    OBJ_ATT_RESISTANCE,
    OBJ_ATT_SHIELD,

    // UNIT ONLY
    OBJ_ATT_SPEED,
    OBJ_ATT_CONSTRUCTION,
    OBJ_ATT_CONQUEST,
    OBJ_ATT_HEALING_RANGE,
    OBJ_ATT_HEALING_POWER,
    OBJ_ATT_SPAWNING,

    // STORAGE
    OBJ_ATT_STORAGE,

    // WEAPON
    OBJ_ATT_ATTACK_ACCURACY,
    OBJ_ATT_ATTACK_POWER,
    OBJ_ATT_ATTACK_RANGE,

    NUM_OBJ_ATTRIBUTES,

    OBJ_ATT_NULL,

    FIRST_WEAPON_ATTRIBUTE = OBJ_ATT_ATTACK_ACCURACY,
    NUM_WEAPON_ATTRIBUTES = NUM_OBJ_ATTRIBUTES - FIRST_WEAPON_ATTRIBUTE,
    NUM_BASIC_ATTRIBUTES = FIRST_WEAPON_ATTRIBUTE,
};

enum PlayerFaction : unsigned int
{
    FACTION_1,
    FACTION_2,
    FACTION_3,

    NUM_FACTIONS,
    NUM_FACTIONS_PLUS_NO,

    NO_FACTION = 99
};

enum ResourceType : unsigned int
{
    RES_ENERGY,
    RES_MATERIAL1,
    RES_DIAMONDS,
    RES_BLOBS,

    NUM_RESOURCES,

    RES_INVALID
};

enum ExtendedResource : unsigned int
{
    ER_ENERGY,
    ER_MATERIAL,
    ER_DIAMONDS,
    ER_BLOBS,
    ER_MONEY,
    ER_RESEARCH,

    NUM_EXTENDED_RESOURCES,

    ER_INVALID
};

enum TechUpgradeId : unsigned int
{
    TECH_UP_BASE_IMPROVE_1,
    TECH_UP_BASE_IMPROVE_2,
    TECH_UP_BASE_IMPROVE_3,
    TECH_UP_BASE_IMPROVE_4,
    TECH_UP_BASE_IMPROVE_5,
    TECH_UP_RADAR_STATION,
    TECH_UP_RADAR_TOWER,
    TECH_UP_STORAGE_STRUCTS,
    TECH_UP_STORAGE_ENERGY_1,
    TECH_UP_STORAGE_ENERGY_2,
    TECH_UP_STORAGE_MATERIAL_1,
    TECH_UP_STORAGE_MATERIAL_2,
    TECH_UP_STORAGE_DIAMONDS_1,
    TECH_UP_STORAGE_DIAMONDS_2,
    TECH_UP_STORAGE_BLOBS_1,
    TECH_UP_STORAGE_BLOBS_2,

    NUM_TECH_UPGRADES,

    TECH_UP_NULL
};

enum ParticlesUpdaterId : unsigned int
{
    PU_DAMAGE,
    PU_HEALING,
    PU_HIT_POINTS,
    PU_OUTPUT,
    PU_SINGLE_LASER
};

enum TurnStage : unsigned int
{
    TURN_STAGE_MINI_UNITS_ATTACK,
    TURN_STAGE_MINI_UNITS_MOVE,
    TURN_STAGE_PLAY,
    TURN_STAGE_TOWERS_ATTACK,

    NUM_TURN_STAGES
};

extern const char * FACTIONS_NAME[NUM_FACTIONS];
extern const char * MISSION_CATEGORY_TITLE[NUM_MISSION_CATEGORIES];

} // namespace game
