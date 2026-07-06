#include "Planet.h"

#include "GameConstants.h"
#include "MapIO.h"

namespace game
{

const char * Planet::PLANETS_NAME[NUM_PLANETS] =
{
    "Tutoria 50",
};

// == MAP DATA ==
Planet::MapData::MapData(const std::string & file, int energy, int material, int diamonds,
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

// == PLANET ==
Planet::Planet(PlanetId pid, PlanetSize size)
    : mId(pid)
    , mSize(size)
{
    const int numMissions[] = { 5, 7, 9 };
    mMaps.reserve(numMissions[size]);
}


bool Planet::AddMap(const std::string & file,
                          PlayerFaction occupier, TerritoryStatus status)
{
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
    mMaps.emplace_back(file, energy, material, diamonds, blobs, rows, cols, value, occupier,
                       status, mission);

    return true;
}

bool Planet::AddUnavailableMap()
{
    mMaps.emplace_back(std::string(), 0, 0, 0, 0, 0, 0, 0, NO_FACTION, TER_ST_UNAVAILABLE, MC_UNKNOWN);

    return true;
}

const std::string & Planet::GetMapFile(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mFile;

    static const std::string empty;
    return empty;
}

PlayerFaction Planet::GetMapOccupier(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mOccupier;

    return NO_FACTION;
}

MissionCategory Planet::GetMapMission(unsigned int index) const
{
    if(index < mMaps.size())
        return mMaps[index].mMission;

    return MC_UNKNOWN;
}

void Planet::SetMapOccupier(unsigned int index, PlayerFaction occupier)
{
    if(index < mMaps.size())
        mMaps[index].mOccupier = occupier;
}

void Planet::SetMapMissionCompleted(unsigned int index)
{
    if(index < mMaps.size())
    {
        mMaps[index].mMission = MISSION_COMPLETED;

        // expand player's reach once a territory is conquered
        ExpandTerritoryReach(index);
    }
}

void Planet::ExpandTerritoryReach(unsigned int index)
{
    if(mSize == PLANET_SIZE_S)
    {
        switch(index)
        {
        case 0:
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(3);
            break;

        case 1:
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(3);
            break;

        case 2:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(4);
            break;

        case 3:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(4);
            break;

        default:
            break;
        }
    }
    else if(mSize == PLANET_SIZE_M)
    {
        switch(index)
        {
        case 0:
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(4);
            break;

        case 1:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(5);
            break;

        case 2:
            ConvertTerritoryUnreachableToUnexplored(4);
            ConvertTerritoryUnreachableToUnexplored(4);
            break;

        case 3:
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(5);
            break;

        case 4:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(6);
            break;

        case 5:
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(3);
            ConvertTerritoryUnreachableToUnexplored(6);
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
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(3);
            break;

        case 1:
            ConvertTerritoryUnreachableToUnexplored(4);
            ConvertTerritoryUnreachableToUnexplored(5);
            break;

        case 2:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(6);
            break;

        case 3:
            ConvertTerritoryUnreachableToUnexplored(0);
            ConvertTerritoryUnreachableToUnexplored(7);
            break;

        case 4:
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(6);
            break;

        case 5:
            ConvertTerritoryUnreachableToUnexplored(1);
            ConvertTerritoryUnreachableToUnexplored(7);
            break;

        case 6:
            ConvertTerritoryUnreachableToUnexplored(2);
            ConvertTerritoryUnreachableToUnexplored(4);
            ConvertTerritoryUnreachableToUnexplored(8);
            break;

        case 7:
            ConvertTerritoryUnreachableToUnexplored(3);
            ConvertTerritoryUnreachableToUnexplored(5);
            ConvertTerritoryUnreachableToUnexplored(8);
            break;

        default:
            break;
        }
    }
}

} // namespace game
