#pragma once

#include <unordered_map>
#include <unordered_set>

namespace game
{

class GameObject;
class Player;
class ResourceGenerator;
class Unit;

enum PlayerFaction : unsigned int;

enum CellTypes : int
{
    CT_EMPTY = 0,
    CT_MOUNTAINS,
    CT_MOUNTAINS_SURR,
    CT_NO_FACTION_OBJ,
    CT_BLOBS_SOURCE,
    CT_DIAMONDS_SOURCE,
    CT_BLOBS_SURR,
    CT_DIAMONDS_SURR,
    CT_TREES1,
    CT_TREES1_SURR,
    CT_F1,
    CT_F1_CONNECTED,
    CT_F1_INFLUENCED,
    CT_F2,
    CT_F2_CONNECTED,
    CT_F2_INFLUENCED,
    CT_F3,
    CT_F3_CONNECTED,
    CT_F3_INFLUENCED,
    CT_FOG_OF_WAR,
    CT_NO_FACTION_OBJ_SURR,
    CT_ROCKS,
    CT_ROCKS_SURR,

    NUM_CELL_TYPES
};

struct GameMapCell
{
    GameMapCell();

    Unit * GetUnit() const;
    bool HasUnit() const;

    ResourceGenerator * GetResourceGenerator() const;

    static bool IsTypePrimary(CellTypes t);

    std::unordered_map<int, bool> influencers;
    PlayerFaction influencer;

    Player * owner = nullptr;
    GameObject * objBottom = nullptr;
    GameObject * objTop = nullptr;

    CellTypes currType = CT_EMPTY;
    CellTypes basicType = CT_EMPTY;

    int row = 0;
    int col = 0;
    bool walkable = true;
    bool linked = false;

private:
    static const std::unordered_set<CellTypes> PRIMARY_TYPES;
};

inline bool GameMapCell::IsTypePrimary(CellTypes t)
{
    return PRIMARY_TYPES.find(t) != PRIMARY_TYPES.end();
}

} // namespace game
