#pragma once

namespace game
{

constexpr int MAX_NUM_PLAYERS = 4;

constexpr int MAX_UNITS_LEVEL = 2;
constexpr int COST_UNIT_UPGRADE[] = { 1, 2 };

constexpr float TIME_AI_MIN = 0.2f;

constexpr unsigned int PLAYER_COLOR[] =
{
    0xdb5757ff,
    0x57db62ff,
    0x57badbff,
    0xdb57dbff
};

enum MapLayers : unsigned int
{
    CELL_OVERLAYS1,
    CELL_OVERLAYS2,
    CELL_OVERLAYS3,
    FACTION_INFLUENCE,
    OBJECTS1,
    OBJECTS2,
    CELL_OVERLAYS4,

    NUM_LAYERS,
};

enum MissionType : unsigned int
{
    MISSION_DESTROY_ENEMY_BASE,
    MISSION_DESTROY_ALL_ENEMIES,
    MISSION_RESIST_TIME,

    NUM_MISSION_TYPES,

    MISSION_COMPLETED,
    MISSION_UNKNOWN
};

enum PlayerFaction : unsigned int
{
    FACTION_1,
    FACTION_2,
    FACTION_3,

    NUM_FACTIONS,

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

enum TerritoryStatus : unsigned int
{
    TER_ST_UNEXPLORED,
    TER_ST_FREE,
    TER_ST_OCCUPIED,
    TER_ST_OCCUPIED_UNEXPLORED,
    TER_ST_UNREACHABLE,
    TER_ST_UNAVAILABLE,

    NUM_TERRITORY_STATUSES,

    TER_ST_UNKNOWN
};

enum Planets : unsigned int
{
    PLANET_1,

    NUM_PLANETS,

    PLANET_UNKNOWN
};

extern const char * FACTIONS_NAME[NUM_FACTIONS];
extern const char * MISSIONS_TITLE[NUM_MISSION_TYPES];
extern const char * PLANETS_NAME[NUM_PLANETS];

} // namespace game
