#include "MapIO.h"

#include "GameConstants.h"
#include "GameMapCell.h"
#include "GameObjects/GameObject.h"

#include <cmath>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace game
{

const std::string MapIO::MAP_VERSION("0.3.1");

const std::string MapIO::MAP_TAG_COMMENT("#");
const std::string MapIO::MAP_TAG_GOAL("G");
const std::string MapIO::MAP_TAG_END_HEADER("--1--");
const std::string MapIO::MAP_TAG_END_MAP("--2--");
const std::string MapIO::MAP_TAG_MAP_SIZE("RC");
const std::string MapIO::MAP_TAG_STAT_BLOBS("SB");
const std::string MapIO::MAP_TAG_STAT_DIAMONDS("SD");
const std::string MapIO::MAP_TAG_STAT_ENERGY("SE");
const std::string MapIO::MAP_TAG_STAT_MATERIAL("SM");
const std::string MapIO::MAP_TAG_STAT_VALUE("SV");
const std::string MapIO::MAP_TAG_VERSION("V");

const unsigned int LEN_SIMPLE_TAG = 1;
const unsigned int LEN_DOUBLE_TAG = 2;
const unsigned int LEN_END_TAG = 5;

const float MAX_STAT_VALUE = 10.f;

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

    mStatBlobs = 0;
    mStatDiamonds = 0;
    mStatEnergy = 0;
    mStatMaterial = 0;
    mStatValue = 0;
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
                 const std::vector<GameObject *> & objects, const std::vector<MissionGoal> & goals,
                 int rows, int cols)
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

    for(const MissionGoal & g : goals)
        fs << MAP_TAG_GOAL << " " << g.IsPrimary() << " "
           << g.GetType() << " " << g.GetQuantity() << "\n";

    // save map size
    fs << "# ====== MAP =====\n";
    fs << MAP_TAG_MAP_SIZE << " " << rows << " " << cols << "\n";

    // stats
    const int mapSize = rows * cols;

    const int statBlobs = DefineStatResourceForCells(cells, BLOBS_SOURCE);
    fs << MAP_TAG_STAT_BLOBS << " " << statBlobs << "\n";

    const int statDiamonds = DefineStatResourceForCells(cells, DIAMONDS_SOURCE);
    fs << MAP_TAG_STAT_DIAMONDS << " " << statDiamonds << "\n";

    const int statEnergy = DefineStatResourceForObjects(objects, mapSize, ObjectData::TYPE_RES_GEN_ENERGY);
    fs << MAP_TAG_STAT_ENERGY << " " << statEnergy << "\n";

    const int statMaterial = DefineStatResourceForObjects(objects, mapSize, ObjectData::TYPE_RES_GEN_MATERIAL);
    fs << MAP_TAG_STAT_MATERIAL << " " << statMaterial << "\n";

    const int statValue = DefineStatValue(statBlobs, statDiamonds, statEnergy, statMaterial);
    fs << MAP_TAG_STAT_VALUE << " " << statValue << "\n";

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
        fs << MapLayers::REGULAR_OBJECTS << " "
           << obj->GetObjectType() << " "
           << obj->GetObjectVariant() << " "
           << obj->GetFaction() << " "
           << obj->GetRow0() << " " << obj->GetCol0() << "\n";
    }

    // finalize file
    fs.close();

    return true;
}

int MapIO::DefineStatResourceForCells(const std::vector<GameMapCell> & cells, CellTypes res) const
{
    const float maxResPerc = 0.05f;

    const int resCells = std::count_if(cells.begin(), cells.end(),
                                       [res](const GameMapCell & cell)
                                       {
                                           return cell.basicType == res;
                                       });

    const int mapSize = cells.size();

    const float perc = resCells / (mapSize * maxResPerc);

    const float stat = std::roundf(MAX_STAT_VALUE * perc);

    if(stat < MAX_STAT_VALUE)
        return stat;
    else
        return MAX_STAT_VALUE;
}

int MapIO::DefineStatResourceForObjects(const std::vector<GameObject *> & objects,
                                        int mapSize, GameObjectTypeId type) const
{
    const float genArea = 16.f;
    const float maxPerc = 10.f;
    const float maxGetSlots = mapSize / (genArea * maxPerc);

    const int resGens = std::count_if(objects.begin(), objects.end(),
                                      [type](const GameObject * obj)
                                      {
                                          return obj->GetObjectType() == type;
                                      });

    const float perc = resGens / maxGetSlots;

    const float stat = std::roundf(MAX_STAT_VALUE * perc);

    if(stat < MAX_STAT_VALUE)
        return stat;
    else
        return MAX_STAT_VALUE;
}

int MapIO::DefineStatValue(int statBlobs, int statDiamonds, int statEnergy, int statMaterial) const
{
    const float w1 = 0.3f;
    const float w2 = 0.2f;

    return (statEnergy + statMaterial) * w1 + (statBlobs + statDiamonds) * w2;
}

void MapIO::ReadHeader(std::fstream & fs)
{
    std::string line;
    std::istringstream ss;

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
        // reading goal
        else if(line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_GOAL) == 0)
        {
            ss.ignore(LEN_SIMPLE_TAG);

            // check if primary or secondary goal
            bool primary = false;
            ss >> primary;

            // goal type
            MissionGoalType type;
            ss >> type;

            // quantity data
            unsigned int quantity = 0;
            ss >> quantity;

            // extra data
            unsigned int extraData = 0;
            ss >> extraData;

            // store goal
            mGoals.emplace_back(type, quantity, extraData, primary);

            // assign category based on first primary goal
            if(primary && MC_UNKNOWN == mCategory)
                mCategory = mGoals[mGoals.size() - 1].GetCategory();
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_MAP_SIZE) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mRows >> mCols;
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_STAT_BLOBS) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mStatBlobs;
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_STAT_DIAMONDS) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mStatDiamonds;
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_STAT_ENERGY) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mStatEnergy;
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_STAT_MATERIAL) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mStatMaterial;
        }
        else if(line.compare(0, LEN_DOUBLE_TAG, MAP_TAG_STAT_VALUE) == 0)
        {
            ss.ignore(LEN_DOUBLE_TAG);
            ss >> mStatValue;
        }
        else if(line.compare(0, LEN_END_TAG, MAP_TAG_END_HEADER) == 0)
            break;
    }
}

void MapIO::ReadMap(std::fstream & fs)
{
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
        if(line.compare(0, LEN_END_TAG, MAP_TAG_END_MAP) == 0)
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

        ss >> e.layerId >> e.typeId >> e.variantId >> e.faction >> e.r0 >> e.c0;

        mObjEntries.emplace_back(e);
    }
}

} // namespace game
