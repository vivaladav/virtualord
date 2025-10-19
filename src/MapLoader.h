#pragma once

#include "MissionGoal.h"
#include "GameObjects/GameObjectTypes.h"

#include <string>
#include <vector>

namespace game
{

class GameMap;
class IsoMap;
class MissionGoal;

enum MissionGoalType : unsigned int;

// structure used to define GameObjects to create
struct MapObjectEntry
{
    unsigned int layerId;
    GameObjectTypeId typeId;
    GameObjectVariantId variantId;
    unsigned int faction;
    unsigned int r0;
    unsigned int c0;
};

class MapLoader
{
public:
    static const std::string MAP_VERSION;

    // -- FILE TAGS --
    static const std::string MAP_TAG_GOAL_PRIMARY;
    static const std::string MAP_TAG_GOAL_SECONDARY;
    static const std::string MAP_TAG_END_BASE_DATA;
    static const std::string MAP_TAG_MAP_SIZE;
    static const std::string MAP_TAG_VERSION;

public:
    unsigned int GetMapRows() const;
    unsigned int GetMapCols() const;

    const std::string & GetMapVersion() const;

    const std::vector<unsigned int> & GetCellTypes() const;

    const std::vector<MapObjectEntry> & GetObjectEntries() const;

    const std::vector<MissionGoal> & GetMissionGoals() const;

    // Mission data
    MissionGoalType GetMissionType() const;
    unsigned int GetMissionTime() const;

    void Clear();

public:
    bool Load(const std::string & filename);

private:
    void ReadBaseData(std::fstream & fs);
    void ReadObjectsData(std::fstream & fs);

private:
    std::vector<MapObjectEntry> mObjEntries;
    std::vector<unsigned int> mCellTypes;
    std::vector<MissionGoal> mGoals;

    std::string mVer;

    unsigned int mRows = 0 ;
    unsigned int mCols = 0 ;

    MissionGoalType mMissionType;
    unsigned int mMissionTime = 0;
};

inline unsigned int MapLoader::GetMapRows() const { return mRows; }
inline unsigned int MapLoader::GetMapCols() const { return mCols; }

inline const std::string &  MapLoader::GetMapVersion() const { return mVer; }

inline const std::vector<unsigned int> & MapLoader::GetCellTypes() const { return mCellTypes; }

inline const std::vector<MapObjectEntry> & MapLoader::GetObjectEntries() const { return mObjEntries; }

inline const std::vector<MissionGoal> & MapLoader::GetMissionGoals() const { return mGoals; }

// Mission data
inline MissionGoalType MapLoader::GetMissionType() const { return mMissionType; }
inline unsigned int MapLoader::GetMissionTime() const { return mMissionTime; }

} // namespace game
