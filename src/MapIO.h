#pragma once

#include "MissionGoal.h"
#include "GameObjects/GameObjectTypes.h"

#include <fstream>
#include <string>
#include <vector>

namespace game
{

class GameMap;
class GameMapCell;
class GameObject;
class IsoMap;

enum MissionCategory : unsigned int;

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

class MapIO
{
public:
    static const std::string MAP_VERSION;

public:
    MapIO();

    unsigned int GetMapRows() const;
    unsigned int GetMapCols() const;

    const std::string & GetMapVersion() const;

    const std::vector<unsigned int> & GetCellTypes() const;

    const std::vector<MapObjectEntry> & GetObjectEntries() const;

    // Mission data
    MissionCategory GetMissionCategory();
    const std::vector<MissionGoal> & GetMissionGoals() const;

    void Clear();

    bool Load(const std::string & filename);
    bool LoadHeader(const std::string & filename);

    bool Save(const std::string & filename, const std::vector<GameMapCell> & cells,
              std::vector<GameObject *> objects, const std::vector<MissionGoal> & goals,
              MissionCategory category, int rows, int cols);

private:
    void ReadHeader(std::fstream & fs);
    void ReadMap(std::fstream & fs);
    void ReadObjectsData(std::fstream & fs);

private:
    // -- FILE TAGS --
    static const std::string MAP_TAG_COMMENT;
    static const std::string MAP_TAG_GOAL;
    static const std::string MAP_TAG_END_HEADER;
    static const std::string MAP_TAG_END_MAP;
    static const std::string MAP_TAG_MAP_SIZE;
    static const std::string MAP_TAG_VERSION;

private:
    std::vector<MapObjectEntry> mObjEntries;
    std::vector<unsigned int> mCellTypes;
    std::vector<MissionGoal> mGoals;
    MissionCategory mCategory;

    std::string mVer;

    unsigned int mRows = 0 ;
    unsigned int mCols = 0 ;
};

inline unsigned int MapIO::GetMapRows() const { return mRows; }
inline unsigned int MapIO::GetMapCols() const { return mCols; }

inline const std::string &  MapIO::GetMapVersion() const { return mVer; }

inline const std::vector<unsigned int> & MapIO::GetCellTypes() const { return mCellTypes; }

inline const std::vector<MapObjectEntry> & MapIO::GetObjectEntries() const { return mObjEntries; }

inline MissionCategory MapIO::GetMissionCategory() { return mCategory; }
inline const std::vector<MissionGoal> & MapIO::GetMissionGoals() const { return mGoals; }

} // namespace game
