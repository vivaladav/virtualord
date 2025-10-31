#include "MapIO.h"

#include "GameConstants.h"
#include "GameMapCell.h"
#include "GameObjects/GameObject.h"

#include <iostream>
#include <sstream>

namespace game
{

const std::string MapIO::MAP_VERSION("0.2.3");

const std::string MapIO::MAP_TAG_CATEGORY("C");
const std::string MapIO::MAP_TAG_COMMENT("#");
const std::string MapIO::MAP_TAG_GOAL("G");
const std::string MapIO::MAP_TAG_END_HEADER("--1--");
const std::string MapIO::MAP_TAG_END_MAP("--2--");
const std::string MapIO::MAP_TAG_MAP_SIZE("RC");
const std::string MapIO::MAP_TAG_VERSION("V");
const unsigned int LEN_SIMPLE_TAG = 1;

MapIO::MapIO()
    : mCategory(MC_UNKNOWN)
{
}

void MapIO::Clear()
{
    mObjEntries.clear();
    mCellTypes.clear();
    mGoals.clear();

    mVer.clear();

    mRows = 0 ;
    mCols = 0 ;

    mCategory = MC_UNKNOWN;
}

bool MapIO::Load(const std::string & filename)
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

bool MapIO::LoadHeader(const std::string & filename)
{
    // open map file
    std::fstream fs(filename);

    if(!fs.is_open())
        return false;

    ReadHeader(fs);

    fs.close();

    return true;
}

bool MapIO::Save(const std::string & filename, const std::vector<GameMapCell> & cells,
                     std::vector<GameObject *> objects, const std::vector<MissionGoal> & goals,
                     MissionCategory category, int rows, int cols)
{
    // no data to save -> EXIT
    if(0 == rows || 0 == cols)
        return false;

    // open map file
    std::ofstream fs(filename, std::ios::out | std::ios::trunc);

    // ERROR failed to open file -> EXIT
    if(!fs.is_open())
        return false;

    // save current map version
    fs << "# ===== VERSION =====\n";
    fs << MAP_TAG_VERSION << " " << MapIO::MAP_VERSION << "\n";

    // mission goals
    fs << "# ====== GOALS =====\n";
    fs << MAP_TAG_CATEGORY << " " << category << "\n";

    for(const MissionGoal & g : goals)
        fs << MAP_TAG_GOAL << " " << g.IsPrimary() << " "
           << MissionGoal::GeTypeString(g.GetType()) << " " << g.GetQuantity() << "\n";

    // save map size
    fs << "# ====== MAP =====\n";
    fs << MAP_TAG_MAP_SIZE << " " << rows << " " << cols << "\n";

    // save header end tag
    fs << MAP_TAG_END_HEADER << "\n";

    // save cells type
    for(unsigned int r = 0; r < rows; ++r)
    {
        const unsigned int ind0 = r * cols;

        for(unsigned int c = 0; c < cols - 1; ++c)
        {
            const unsigned int ind = ind0 + c;
            fs << cells[ind].currType << " ";
        }

        const unsigned int ind = ind0 + (cols - 1);
        fs << cells[ind].currType << "\n";
    }

    // save map end tag
    fs << MAP_TAG_END_MAP << "\n";

    // save objects
    fs << "# ====== OBJECTS =====\n";
    for(const GameObject * obj : objects)
    {
        fs << MapLayers::OBJECTS1 << " "
           << obj->GetObjectTypeStr() << " "
           << obj->GetObjectVariant() << " "
           << obj->GetFaction() << " "
           << obj->GetRow0() << " " << obj->GetCol0() << "\n";
    }

    // finalize file
    fs.close();

    return true;
}

void MapIO::ReadHeader(std::fstream & fs)
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
            std::string gt;
            ss >> gt;
            const std::size_t type = std::hash<std::string>{}(gt);

            // quantity data
            unsigned int quantity = 0;
            ss >> quantity;

            mGoals.emplace_back(type, quantity, primary);
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

void MapIO::ReadMap(std::fstream & fs)
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


void MapIO::ReadObjectsData(std::fstream & fs)
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
