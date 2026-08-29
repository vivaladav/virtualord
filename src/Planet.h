#pragma once

#include <string>
#include <vector>

namespace sgl { namespace utilities { class BinaryFile; } }

namespace game
{

enum PlanetId : unsigned int
{
    PLANET_1,

    NUM_PLANETS,

    PLANET_UNKNOWN
};

enum PlanetSize : unsigned int
{
    PLANET_SIZE_S,
    PLANET_SIZE_M,
    PLANET_SIZE_L,

    NUM_PLANET_SIZES,

    PLANET_SIZE_NULL
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

enum MissionCategory : unsigned int;
enum PlayerFaction : unsigned int;

class Planet
{
public:
    static const char * PLANETS_NAME[NUM_PLANETS];

public:
    Planet();
    Planet(PlanetId pid, PlanetSize size);

    bool Load(sgl::utilities::BinaryFile & bf);
    bool Save(sgl::utilities::BinaryFile & bf) const;

    PlanetId GetPlanetId() const;

    PlanetSize GetSize() const;

    bool AddMap(const std::string & file,
                PlayerFaction occupier, TerritoryStatus status);
    bool AddUnavailableMap();

    unsigned int GetNumMaps() const;

    const std::string & GetMapFile(unsigned int index) const;
    int GetMapEnergy(unsigned int index) const;
    int GetMapMaterial(unsigned int index) const;
    int GetMapDiamonds(unsigned int index) const;
    int GetMapBlobs(unsigned int index) const;
    int GetMapRows(unsigned int index) const;
    int GetMapCols(unsigned int index) const;
    int GetMapValue(unsigned int index) const;
    PlayerFaction GetMapOccupier(unsigned int index) const;
    TerritoryStatus GetMapStatus(unsigned int index) const;
    MissionCategory GetMapMission(unsigned int index) const;

    void SetMapStatus(unsigned int index, TerritoryStatus status);
    void SetMapOccupier(unsigned int index, PlayerFaction occupier);
    void SetMapMissionCompleted(unsigned int index);

    void ClearData();

private:
    void ExpandTerritoryReach(unsigned int index);
    void ConvertTerritoryUnreachableToUnexplored(unsigned int index);

private:
    struct MapData
    {
        MapData() { }
        MapData(const std::string & file, int energy, int material, int diamonds,
                int blobs, unsigned int rows, unsigned int cols, int value,
                PlayerFaction occupier, TerritoryStatus status, MissionCategory mission);

        std::string mFile;
        int mEnergy;
        int mMaterial;
        int mDiamonds;
        int mBlobs;
        unsigned int mRows;
        unsigned int mCols;
        int mValue;
        PlayerFaction mOccupier;
        TerritoryStatus mStatus;
        MissionCategory mMission;
    };

    std::vector<MapData> mMaps;

    PlanetId mId;
    PlanetSize mSize;
};

inline PlanetId Planet::GetPlanetId() const { return mId; }

inline PlanetSize Planet::GetSize() const { return mSize; }

inline unsigned int Planet::GetNumMaps() const
{
    return mMaps.size();
}

inline int Planet::GetMapEnergy(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mEnergy;

    return 0;
}

inline int Planet::GetMapMaterial(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mMaterial;

    return 0;
}

inline int Planet::GetMapDiamonds(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mDiamonds;

    return 0;
}

inline int Planet::GetMapBlobs(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mBlobs;

    return 0;
}

inline int Planet::GetMapRows(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mRows;

    return 0;
}

inline int Planet::GetMapCols(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mCols;

    return 0;
}

inline int Planet::GetMapValue(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mValue;

    return 0;
}

inline TerritoryStatus Planet::GetMapStatus(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mStatus;

    return TER_ST_UNKNOWN;
}


inline void Planet::SetMapStatus(unsigned int index, TerritoryStatus status)
{
    if(index < mMaps.size())
        mMaps[index].mStatus = status;
}

inline void Planet::ClearData()
{
    mMaps.clear();
}

inline void Planet::ConvertTerritoryUnreachableToUnexplored(unsigned int index)
{
    if(index < mMaps.size() && mMaps[index].mStatus == TER_ST_UNREACHABLE)
        mMaps[index].mStatus = TER_ST_UNEXPLORED;
}

} // namespace game