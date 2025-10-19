#include "MapLoader.h"

#include "GameConstants.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace game
{

const std::string MapLoader::MAP_VERSION("0.2.0");

const std::string MapLoader::MAP_TAG_GOAL_PRIMARY("P");
const std::string MapLoader::MAP_TAG_GOAL_SECONDARY("S");
const std::string MapLoader::MAP_TAG_END_BASE_DATA("-----");
const std::string MapLoader::MAP_TAG_MAP_SIZE("RC");
const std::string MapLoader::MAP_TAG_VERSION("V");

void MapLoader::Clear()
{
    mObjEntries.clear();
    mCellTypes.clear();

    mVer.clear();

    mRows = 0 ;
    mCols = 0 ;
}

bool MapLoader::Load(const std::string & filename)
{
    // open map file
    std::fstream fs(filename);

    if(!fs.is_open())
        return false;

    ReadBaseData(fs);

    ReadObjectsData(fs);

    fs.close();

    return true;
}

void MapLoader::ReadBaseData(std::fstream & fs)
{
    std::string line;
    std::istringstream ss;

    const unsigned int lenTagGoalPrimary = MAP_TAG_GOAL_PRIMARY.length();
    const unsigned int lenTagGoalSecondary = MAP_TAG_GOAL_SECONDARY.length();
    const unsigned int lenTagEndBaseData = MAP_TAG_END_BASE_DATA.length();
    const unsigned int lenTagMapSize = MAP_TAG_MAP_SIZE.length();
    const unsigned int lenTagVersion = MAP_TAG_VERSION.length();

    while(std::getline(fs, line))
    {
        // skip comments
        if(!line.empty() && '#' == line[0])
            continue;

        ss.clear();
        ss.str(line);

        // reading file version
        if(line.compare(0, lenTagVersion, MAP_TAG_VERSION) == 0)
        {
            ss.ignore(lenTagVersion);
            ss >> mVer;

            if(mVer != MAP_VERSION)
            {
                std::cout << "[WAR] map file version (" << mVer
                          << ") is different from current one (" << MAP_VERSION << ")"
                          << std::endl;
            }
        }
        // reading goal
        else if(line.compare(0, lenTagGoalPrimary, MAP_TAG_GOAL_PRIMARY) == 0 ||
                line.compare(0, lenTagGoalSecondary, MAP_TAG_GOAL_SECONDARY) == 0)
        {
            const bool primary = line.compare(0, lenTagGoalPrimary, MAP_TAG_GOAL_PRIMARY) == 0;

            if(primary)
                ss.ignore(lenTagGoalPrimary);
            else
                ss.ignore(lenTagGoalSecondary);

            // goal type
            unsigned int gt;
            ss >> gt;
            auto type = static_cast<MissionGoalType>(gt);

            // only read quantity data for some goals
            unsigned int quantity = 0;

            if(MG_COLLECT_BLOBS == type || MG_COLLECT_DIAMONDS == type ||
               MG_GAIN_MONEY == type || MG_MINE_MATERIAL == type ||
               MG_MINE_ENERGY == type || MG_RESIST_TIME == type)
                ss >> quantity;

            mGoals.emplace_back(type, quantity, primary);

            // TODO remove when completed goals logic
            mMissionType = MG_DESTROY_ENEMY_BASE;
        }
        else if(line.compare(0, lenTagMapSize, MAP_TAG_MAP_SIZE) == 0)
        {
            ss.ignore(lenTagMapSize);
            ss >> mRows >> mCols;

            // READ BASE MAP
            for(unsigned int r = 0; r < mRows; ++r)
            {
                std::getline(fs, line);
                ss.clear();
                ss.str(line);

                for(unsigned int c = 0; c < mCols; ++c)
                {
                    unsigned int type;
                    ss >> type;

                    mCellTypes.push_back(type);
                }
            }
        }
        else if(line.compare(0, lenTagEndBaseData, MAP_TAG_END_BASE_DATA) == 0)
            break;
    }
}

void MapLoader::ReadObjectsData(std::fstream & fs)
{
    std::string line;
    std::istringstream ss;

    // READ OBJECTS
    while(std::getline(fs, line))
    {
        // skip comments
        if(!line.empty() && '#' == line[0])
            continue;

        ss.clear();
        ss.str(line);

        MapObjectEntry e;
        std::string objIdStr;

        ss >> e.layerId >> objIdStr >> e.variantId >> e.faction >> e.r0 >> e.c0;
        e.typeId = std::hash<std::string>{}(objIdStr);

        mObjEntries.emplace_back(e);
    }
}

} // namespace game
