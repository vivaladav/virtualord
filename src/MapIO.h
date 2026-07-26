#pragma once

#include "MissionGoal.h"
#include "GameObjects/GameObjectTypes.h"

#include <fstream>
#include <string>
#include <vector>

namespace game
{

class GameMap;
class GameObject;
class IsoMap;
struct GameMapCell;

enum CellTypes : int;
enum MissionCategory : unsigned int;

// structure used to define GameObjects to create
struct MapObjectEntry
{
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

    int GetStatBlobs() const;
    int GetStatDiamonds() const;
    int GetStatEnergy() const;
    int GetStatMaterial() const;
    int GetStatMapValue() const;

    void Clear();

    bool Load(const std::string & filename);
    bool LoadHeader(const std::string & filename);

    bool Save(const std::string & filename, const std::vector<GameMapCell> & cells,
              const std::vector<GameObject *> & objects, const std::vector<MissionGoal> & goals,
              int rows, int cols);

private:
    int DefineStatResourceForCells(const std::vector<GameMapCell> & cells, CellTypes res) const;
    int DefineStatResourceForObjects(const std::vector<GameObject *> & objects,
                                     int mapSize, GameObjectTypeId type) const;
    int DefineStatValue(int statBlobs, int statDiamonds, int statEnergy, int statMaterial) const;

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
    static const std::string MAP_TAG_STAT_BLOBS;
    static const std::string MAP_TAG_STAT_DIAMONDS;
    static const std::string MAP_TAG_STAT_ENERGY;
    static const std::string MAP_TAG_STAT_MATERIAL;
    static const std::string MAP_TAG_STAT_VALUE;
    static const std::string MAP_TAG_VERSION;

private:
    std::vector<MapObjectEntry> mObjEntries;
    std::vector<unsigned int> mCellTypes;
    std::vector<MissionGoal> mGoals;
    MissionCategory mCategory;

    std::string mVer;

    unsigned int mRows = 0 ;
    unsigned int mCols = 0 ;

    int mStatBlobs = 0;
    int mStatDiamonds = 0;
    int mStatEnergy = 0;
    int mStatMaterial = 0;
    int mStatValue = 0;
};

inline unsigned int MapIO::GetMapRows() const { return mRows; }
inline unsigned int MapIO::GetMapCols() const { return mCols; }

inline const std::string &  MapIO::GetMapVersion() const { return mVer; }

inline const std::vector<unsigned int> & MapIO::GetCellTypes() const { return mCellTypes; }

inline const std::vector<MapObjectEntry> & MapIO::GetObjectEntries() const { return mObjEntries; }

inline MissionCategory MapIO::GetMissionCategory() { return mCategory; }
inline const std::vector<MissionGoal> & MapIO::GetMissionGoals() const { return mGoals; }

inline int MapIO::GetStatBlobs() const { return mStatBlobs; }
inline int MapIO::GetStatDiamonds() const { return mStatDiamonds; }
inline int MapIO::GetStatEnergy() const { return mStatEnergy; }
inline int MapIO::GetStatMaterial() const { return mStatMaterial; }
inline int MapIO::GetStatMapValue() const { return mStatValue; }

} // namespace game
