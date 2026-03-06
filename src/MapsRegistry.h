#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace game
{

enum MissionCategory : unsigned int;
enum PlayerFaction : unsigned int;

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

class MapsRegistry
{
public:
    static const char * PLANETS_NAME[NUM_PLANETS];

public:
    bool CreatePlanet(PlanetId planetId, PlanetSize size);

    PlanetSize GetPlanetSize(PlanetId planetId) const;

    bool AddMap(PlanetId planetId, const std::string & file,
                PlayerFaction occupier, TerritoryStatus status);
    bool AddUnavailableMap(PlanetId planetId);

    int GetNumMaps(PlanetId planetId) const;

    const std::string & GetMapFile(PlanetId planetId, unsigned int index) const;
    int GetMapEnergy(PlanetId planetId, unsigned int index) const;
    int GetMapMaterial(PlanetId planetId, unsigned int index) const;
    int GetMapDiamonds(PlanetId planetId, unsigned int index) const;
    int GetMapBlobs(PlanetId planetId, unsigned int index) const;
    int GetMapRows(PlanetId planetId, unsigned int index) const;
    int GetMapCols(PlanetId planetId, unsigned int index) const;
    int GetMapValue(PlanetId planetId, unsigned int index) const;
    PlayerFaction GetMapOccupier(PlanetId planetId, unsigned int index) const;
    TerritoryStatus GetMapStatus(PlanetId planetId, unsigned int index) const;
    MissionCategory GetMapMission(PlanetId planetId, unsigned int index) const;

    void SetMapStatus(PlanetId planetId, unsigned int index, TerritoryStatus status);
    void SetMapOccupier(PlanetId planetId, unsigned int index, PlayerFaction occupier);
    void SetMapMissionCompleted(PlanetId planetId, unsigned int index);

    void ClearData();

private:
    void ExpandTerritoryReach(PlanetId planetId, int index);
    void ConvertTerritoryUnreachableToUnexplored(PlanetId planetId, int index);

private:
    struct MapData
    {
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

    std::unordered_map<PlanetId, std::vector<MapData>> mData;
    std::unordered_map<PlanetId, PlanetSize> mPlanetsSize;
};

} // namespace game
