#include "MapsRegistry.h"

#include "GameConstants.h"
#include "MapIO.h"

namespace game
{


const char * MapsRegistry::PLANETS_NAME[NUM_PLANETS] =
{
    "Tutoria 50"
};

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
bool MapsRegistry::CreatePlanet(PlanetId planetId, PlanetSize size)
{
    // planet already added
    if(mData.find(planetId) != mData.end())
        return false;

    mPlanetsSize.emplace(planetId, size);

    mData.emplace(planetId, std::vector<MapData>());

    const int numMissions[] = { 5, 7, 9 };
    mData[planetId].reserve(numMissions[size]);

    return true;
}

PlanetSize MapsRegistry::GetPlanetSize(PlanetId planetId) const
{
    auto it = mPlanetsSize.find(planetId);

    if(it != mPlanetsSize.end())
        return it->second;
    else
        return PLANET_SIZE_NULL;
}

bool MapsRegistry::AddMap(PlanetId planetId, const std::string & file,
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
    const int blobs = mio.GetStatBlobs();
    const int diamonds = mio.GetStatDiamonds();
    const int energy = mio.GetStatEnergy();
    const int material = mio.GetStatMaterial();
    const int value = mio.GetStatMapValue();

    // store data
    mData[planetId].emplace_back(file, energy, material, diamonds, blobs,
                                 rows, cols, value, occupier, status, mission);

    return true;
}

bool MapsRegistry::AddUnavailableMap(PlanetId planetId)
{
    // planet not found
    if(mData.find(planetId) == mData.end())
        return false;

    mData[planetId].emplace_back(std::string(), 0, 0, 0, 0, 0, 0, 0,
                                 NO_FACTION, TER_ST_UNAVAILABLE, MC_UNKNOWN);

    return true;
}

int MapsRegistry::GetNumMaps(PlanetId planetId) const
{
    // planet not found
    if(mData.find(planetId) == mData.end())
        return 0;

    return mData.at(planetId).size();
}

const std::string & MapsRegistry::GetMapFile(PlanetId planetId, unsigned int index) const
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

int MapsRegistry::GetMapEnergy(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mEnergy;
    }

    return 0;
}

int MapsRegistry::GetMapMaterial(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mMaterial;
    }

    return 0;
}

int MapsRegistry::GetMapDiamonds(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mDiamonds;
    }

    return 0;
}

int MapsRegistry::GetMapBlobs(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mBlobs;
    }
    return 0;
}

int MapsRegistry::GetMapRows(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) == mData.end())
        return 0;

    const auto & data = mData.at(planetId);

    if(index < data.size())
        return data[index].mRows;
    else
        return 0;
}

int MapsRegistry::GetMapCols(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) == mData.end())
        return 0;

    const auto & data = mData.at(planetId);

    if(index < data.size())
        return data[index].mCols;
    else
        return 0;
}

int MapsRegistry::GetMapValue(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mValue;
    }

    return 0;
}

PlayerFaction MapsRegistry::GetMapOccupier(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mOccupier;
    }

    return NO_FACTION;
}

TerritoryStatus MapsRegistry::GetMapStatus(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mStatus;
    }

    return TER_ST_UNKNOWN;
}

MissionCategory MapsRegistry::GetMapMission(PlanetId planetId, unsigned int index) const
{
    if(mData.find(planetId) != mData.end())
    {
        const auto & data = mData.at(planetId);

        if(index < data.size())
            return data[index].mMission;
    }

    return MC_UNKNOWN;
}

void MapsRegistry::SetMapStatus(PlanetId planetId, unsigned int index, TerritoryStatus status)
{
    if(mData.find(planetId) != mData.end())
    {
        auto & data = mData.at(planetId);

        if(index < data.size())
            data[index].mStatus = status;
    }
}

void MapsRegistry::SetMapOccupier(PlanetId planetId, unsigned int index, PlayerFaction occupier)
{
    if(mData.find(planetId) != mData.end())
    {
        auto & data = mData.at(planetId);

        if(index < data.size())
            data[index].mOccupier = occupier;
    }
}

void MapsRegistry::SetMapMissionCompleted(PlanetId planetId, unsigned int index)
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

void MapsRegistry::ExpandTerritoryReach(PlanetId planetId, int index)
{
    const PlanetSize ps = GetPlanetSize(planetId);

    if(ps == PLANET_SIZE_S)
    {
        switch(index)
        {
            case 0:
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 3);
            break;

            case 1:
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 3);
            break;

            case 2:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
            break;

            case 3:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
            break;

            default:
            break;
        }
    }
    else if(ps == PLANET_SIZE_M)
    {
        switch(index)
        {
            case 0:
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
            break;

            case 1:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 5);
            break;

            case 2:
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
            break;

            case 3:
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 5);
            break;

            case 4:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 6);
            break;

            case 5:
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 3);
                ConvertTerritoryUnreachableToUnexplored(planetId, 6);
            break;

            default:
            break;
        }
    }
    else
    {
        switch(index)
        {
            case 0:
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 3);
            break;

            case 1:
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
                ConvertTerritoryUnreachableToUnexplored(planetId, 5);
            break;

            case 2:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 6);
            break;

            case 3:
                ConvertTerritoryUnreachableToUnexplored(planetId, 0);
                ConvertTerritoryUnreachableToUnexplored(planetId, 7);
            break;

            case 4:
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 6);
            break;

            case 5:
                ConvertTerritoryUnreachableToUnexplored(planetId, 1);
                ConvertTerritoryUnreachableToUnexplored(planetId, 7);
            break;

            case 6:
                ConvertTerritoryUnreachableToUnexplored(planetId, 2);
                ConvertTerritoryUnreachableToUnexplored(planetId, 4);
                ConvertTerritoryUnreachableToUnexplored(planetId, 8);
            break;

            case 7:
                ConvertTerritoryUnreachableToUnexplored(planetId, 3);
                ConvertTerritoryUnreachableToUnexplored(planetId, 5);
                ConvertTerritoryUnreachableToUnexplored(planetId, 8);
            break;

            default:
            break;
        }
    }
}

void MapsRegistry::ConvertTerritoryUnreachableToUnexplored(PlanetId planetId, int index)
{
    if(GetMapStatus(planetId, index) == TER_ST_UNREACHABLE)
        SetMapStatus(planetId, index, TER_ST_UNEXPLORED);
}

} // namespace game
