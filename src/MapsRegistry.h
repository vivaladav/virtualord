#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace game
{

enum MissionCategory : unsigned int;
enum PlayerFaction : unsigned int;
enum TerritoryStatus : unsigned int;

class MapsRegistry
{
public:
    bool CreatePlanet(unsigned int planetId);

    bool AddMap(unsigned int planetId, const std::string & file, int energy,
                int material, int diamonds, int blobs, int value,
                PlayerFaction occupier, TerritoryStatus status);
    bool AddUnavailableMap(unsigned int planetId);

    int GetNumMaps(unsigned int planetId) const;

    const std::string & GetMapFile(unsigned int planetId, unsigned int index) const;
    int GetMapEnergy(unsigned int planetId, unsigned int index) const;
    int GetMapMaterial(unsigned int planetId, unsigned int index) const;
    int GetMapDiamonds(unsigned int planetId, unsigned int index) const;
    int GetMapBlobs(unsigned int planetId, unsigned int index) const;
    int GetMapRows(unsigned int planetId, unsigned int index) const;
    int GetMapCols(unsigned int planetId, unsigned int index) const;
    int GetMapValue(unsigned int planetId, unsigned int index) const;
    PlayerFaction GetMapOccupier(unsigned int planetId, unsigned int index) const;
    TerritoryStatus GetMapStatus(unsigned int planetId, unsigned int index) const;
    MissionCategory GetMapMission(unsigned int planetId, unsigned int index) const;

    void SetMapStatus(unsigned int planetId, unsigned int index, TerritoryStatus status);
    void SetMapOccupier(unsigned int planetId, unsigned int index, PlayerFaction occupier);
    void SetMapMissionCompleted(unsigned int planetId, unsigned int index);

    void ClearData();

private:
    void ExpandTerritoryReach(unsigned int planetId, int index);
    void ConvertTerritoryUnreachableToUnexplored(unsigned int planetId, int index);

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

    std::unordered_map<unsigned int, std::vector<MapData>> mData;
};

} // namespace game
