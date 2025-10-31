#include "MapsRegistry.h"

#include "GameConstants.h"
#include "MapIO.h"

namespace game
{

constexpr int MAX_MAPS_PER_PLANET = 12;

// == MAP DATA ==
MapsRegistry::MapData::MapData(const std::string & file, int energy, int material, int diamonds,
                               int blobs, unsigned int rows, unsigned int cols, int value,
                               PlayerFaction occupier, TerritoryStatus status, MissionCategory mission)
    : mFile(file)
    , mEnergy(energy)
    , mMaterial(material)
    , mDiamonds(diamonds)
    , mBlobs(blobs)
    , mRows(rows)
    , mCols(cols)
    , mValue(value)
    , mOccupier(occupier)
    , mStatus(status)
    , mMission(mission)
{
}

// == MAPS REGISTRY ==
bool MapsRegistry::CreatePlanet(unsigned int planetId)
{
    // planet already added
    if(mData.find(planetId) != mData.end())
        return false;

     mData.emplace(planetId, std::vector<MapData>());
     mData[planetId].reserve(MAX_MAPS_PER_PLANET);

     return true;
}

bool MapsRegistry::AddMap(unsigned int planetId, const std::string & file, int energy,
                          int material, int diamonds, int blobs, int value,
                          PlayerFaction occupier, TerritoryStatus status)
{
    // planet not found
    if(mData.find(planetId) == mData.end())
        return false;

    // load data from header
    MapIO mio;
    mio.LoadHeader(file);

    const MissionCategory mission = mio.GetMissionCategory();
    const unsigned int rows = mio.GetMapRows();
    const unsigned int cols = mio.GetMapCols();

    // store data
    mData[planetId].emplace_back(file, energy, material, diamonds, blobs,
                                 rows, cols, value, occupier, status, mission);

    return true;
}

bool MapsRegistry::AddUnavailableMap(unsigned int planetId)
{
    // planet not found
    if(mData.find(planetId) == mData.end())
        return false;

    mData[planetId].emplace_back(std::string(), 0, 0, 0, 0, 0, 0, 0,
                                 NO_FACTION, TER_ST_UNAVAILABLE, MC_UNKNOWN);

    return true;
}

int MapsRegistry::GetNumMaps(unsigned int planetId) const
{
    // planet not found
    if(mData.find(planetId) == mData.end())
        return 0;

    return mData.at(planetId).size();
}

const std::string & MapsRegistry::GetMapFile(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mFile;
    }

    static const std::string empty;
    return empty;
}

int MapsRegistry::GetMapEnergy(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mEnergy;
    }

    return 0;
}

int MapsRegistry::GetMapMaterial(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mMaterial;
    }

    return 0;
}

int MapsRegistry::GetMapDiamonds(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mDiamonds;
    }

    return 0;
}

int MapsRegistry::GetMapBlobs(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mBlobs;
    }
    return 0;
}

int MapsRegistry::GetMapRows(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) == mData.end())
        return 0;

    const auto & data = mData.at(planetId);

    if(index < data.size())
        return data[index].mRows;
    else
        return 0;
}

int MapsRegistry::GetMapCols(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) == mData.end())
        return 0;

    const auto & data = mData.at(planetId);

    if(index < data.size())
        return data[index].mCols;
    else
        return 0;
}

int MapsRegistry::GetMapValue(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mValue;
    }

    return 0;
}

PlayerFaction MapsRegistry::GetMapOccupier(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mOccupier;
    }

    return NO_FACTION;
}

TerritoryStatus MapsRegistry::GetMapStatus(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mStatus;
    }

    return TER_ST_UNKNOWN;
}

MissionCategory MapsRegistry::GetMapMission(unsigned int planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mMission;
    }

    return MC_UNKNOWN;
}

void MapsRegistry::SetMapStatus(unsigned int planetId, unsigned int index, TerritoryStatus status)
{
    if(mData.find(planetId) != mData.end())
    {
        auto & data = mData.at(planetId);

        if(index < data.size())
            data[index].mStatus = status;
    }
}

void MapsRegistry::SetMapOccupier(unsigned int planetId, unsigned int index, PlayerFaction occupier)
{
    if(mData.find(planetId) != mData.end())
    {
        auto & data = mData.at(planetId);

        if(index < data.size())
            data[index].mOccupier = occupier;
    }
}

void MapsRegistry::SetMapMissionCompleted(unsigned int planetId, unsigned int index)
{
    if(mData.find(planetId) != mData.end())
    {
        auto & data = mData.at(planetId);

        if(index < data.size())
        {
            data[index].mMission = MISSION_COMPLETED;

            // expand player's reach once a territory is conquered
            ExpandTerritoryReach(planetId, index);
        }
    }
}

void MapsRegistry::ClearData()
{
    mData.clear();
}

void MapsRegistry::ExpandTerritoryReach(unsigned int planetId, int index)
{
    switch (index)
    {
        case 0:
            ConvertTerritoryUnreachableToUnexplored(planetId, 2);
        break;

        case 1:
            ConvertTerritoryUnreachableToUnexplored(planetId, 3);
        break;

        case 2:
            ConvertTerritoryUnreachableToUnexplored(planetId, 0);
            ConvertTerritoryUnreachableToUnexplored(planetId, 3);
            ConvertTerritoryUnreachableToUnexplored(planetId, 5);
        break;

        case 3:
            ConvertTerritoryUnreachableToUnexplored(planetId, 1);
            ConvertTerritoryUnreachableToUnexplored(planetId, 2);
            ConvertTerritoryUnreachableToUnexplored(planetId, 6);
        break;

        case 4:
            ConvertTerritoryUnreachableToUnexplored(planetId, 5);
        break;

        case 5:
            ConvertTerritoryUnreachableToUnexplored(planetId, 2);
            ConvertTerritoryUnreachableToUnexplored(planetId, 4);
            ConvertTerritoryUnreachableToUnexplored(planetId, 8);
        break;

        case 6:
            ConvertTerritoryUnreachableToUnexplored(planetId, 3);
            ConvertTerritoryUnreachableToUnexplored(planetId, 7);
            ConvertTerritoryUnreachableToUnexplored(planetId, 9);
        break;

        case 7:
            ConvertTerritoryUnreachableToUnexplored(planetId, 6);
        break;

        case 8:
            ConvertTerritoryUnreachableToUnexplored(planetId, 5);
            ConvertTerritoryUnreachableToUnexplored(planetId, 9);
            ConvertTerritoryUnreachableToUnexplored(planetId, 10);
        break;

        case 9:
            ConvertTerritoryUnreachableToUnexplored(planetId, 6);
            ConvertTerritoryUnreachableToUnexplored(planetId, 8);
            ConvertTerritoryUnreachableToUnexplored(planetId, 11);
        break;

        case 10:
            ConvertTerritoryUnreachableToUnexplored(planetId, 8);
        break;

        case 11:
            ConvertTerritoryUnreachableToUnexplored(planetId, 9);
        break;

        default:
        break;
    }
}

void MapsRegistry::ConvertTerritoryUnreachableToUnexplored(unsigned int planetId, int index)
{
    if(GetMapStatus(planetId, index) == TER_ST_UNREACHABLE)
        SetMapStatus(planetId, index, TER_ST_UNEXPLORED);
}


} // namespace game
