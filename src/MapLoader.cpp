#include "MapLoader.h"

#include "GameConstants.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace game
{

const std::string MapLoader::MAP_VERSION("0.2.2");

const std::string MapLoader::MAP_TAG_CATEGORY("C");
const std::string MapLoader::MAP_TAG_COMMENT("#");
const std::string MapLoader::MAP_TAG_GOAL("G");
const std::string MapLoader::MAP_TAG_END_HEADER("--1--");
const std::string MapLoader::MAP_TAG_END_MAP("--2--");
const std::string MapLoader::MAP_TAG_MAP_SIZE("RC");
const std::string MapLoader::MAP_TAG_VERSION("V");
const unsigned int LEN_SIMPLE_TAG = 1;

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

    ReadHeader(fs);

    ReadMap(fs);

    ReadObjectsData(fs);

    fs.close();

    return true;
}

bool MapLoader::LoadHeader(const std::string & filename)
{
    // open map file
    std::fstream fs(filename);

    if(!fs.is_open())
        return false;

    ReadHeader(fs);

    fs.close();

    return true;
}

void MapLoader::ReadHeader(std::fstream & fs)
{
    std::string line;
    std::istringstream ss;

    const unsigned int lenTagEndHeader = MAP_TAG_END_HEADER.length();
    const unsigned int lenTagMapSize = MAP_TAG_MAP_SIZE.length();

    while(std::getline(fs, line))
    {
        // skip comments
        if(!line.empty() && line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_COMMENT) == 0)
            continue;

        ss.clear();
        ss.str(line);

        // reading file version
        if(line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_VERSION) == 0)
        {
            ss.ignore(LEN_SIMPLE_TAG);
            ss >> mVer;

            // map version doesn't match expected one
            if(mVer != MAP_VERSION)
            {
#ifdef DEV_MODE
                std::cout << "[WAR] map file version (" << mVer
                          << ") is different from current one (" << MAP_VERSION << ")"
                          << std::endl;
#endif
            }
        }
        // reading category
        else if(line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_CATEGORY) == 0)
        {
            ss.ignore(LEN_SIMPLE_TAG);

            // goal type
            unsigned int cat = MC_UNKNOWN;
            ss >> cat;
            mCategory = static_cast<MissionCategory>(cat);
        }
        // reading goal
        else if(line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_GOAL) == 0)
        {
            ss.ignore(LEN_SIMPLE_TAG);

            // check if primary or secondary goal
            bool primary = false;
            ss >> primary;

            // goal type
            unsigned int gt = MG_UNKNOWN;
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
        }
        else if(line.compare(0, lenTagEndHeader, MAP_TAG_END_HEADER) == 0)
            break;
    }
}

void MapLoader::ReadMap(std::fstream & fs)
{
    const unsigned int lenTagEndMap = MAP_TAG_END_MAP.length();

    std::string line;
    std::istringstream ss;

    unsigned int rows = 0;

    // READ MAP
    while(std::getline(fs, line))
    {
        // skip comments
        if(!line.empty() && line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_COMMENT) == 0)
            continue;

        // exit when read end tag
        if(line.compare(0, lenTagEndMap, MAP_TAG_END_MAP) == 0)
            break;

        // good row
        ++rows;

        // reading too may rows !?
        if(rows > mRows)
        {
#ifdef DEV_MODE
            std::cout << "[WAR] rows (" << rows << ") are more than expected (" << mRows << ")"
                      << std::endl;
#endif
            break;
        }

        ss.clear();
        ss.str(line);

        // parse row
        for(unsigned int c = 0; c < mCols; ++c)
        {
            unsigned int type;
            ss >> type;

            mCellTypes.push_back(type);
        }
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
        if(!line.empty() && line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_COMMENT) == 0)
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
