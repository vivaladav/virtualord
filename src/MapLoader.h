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

enum MissionCategory : unsigned int;
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
    static const std::string MAP_TAG_CATEGORY;
    static const std::string MAP_TAG_COMMENT;
    static const std::string MAP_TAG_GOAL_PRIMARY;
    static const std::string MAP_TAG_GOAL_SECONDARY;
    static const std::string MAP_TAG_END_HEADER;
    static const std::string MAP_TAG_END_MAP;
    static const std::string MAP_TAG_MAP_SIZE;
    static const std::string MAP_TAG_VERSION;

public:
    unsigned int GetMapRows() const;
    unsigned int GetMapCols() const;

    const std::string & GetMapVersion() const;

    const std::vector<unsigned int> & GetCellTypes() const;

    const std::vector<MapObjectEntry> & GetObjectEntries() const;

    // Mission data
    MissionCategory GetMissionCategory();
    const std::vector<MissionGoal> & GetMissionGoals() const;

    // TODO remove once goal logic is completed
    MissionGoalType GetMissionType() const;
    unsigned int GetMissionTime() const;

    void Clear();

    bool Load(const std::string & filename);

private:
    void ReadHeader(std::fstream & fs);
    void ReadMap(std::fstream & fs);
    void ReadObjectsData(std::fstream & fs);

private:
    std::vector<MapObjectEntry> mObjEntries;
    std::vector<unsigned int> mCellTypes;
    std::vector<MissionGoal> mGoals;
    MissionCategory mCategory;

    std::string mVer;

    unsigned int mRows = 0 ;
    unsigned int mCols = 0 ;

    // TODO remove once goal logic is completed
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
