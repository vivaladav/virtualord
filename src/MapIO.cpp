#include "MapIO.h"

#include "GameConstants.h"
#include "GameMapCell.h"
#include "GameObjects/GameObject.h"

#include <sgl/utilities/BinaryFile.h>

#include <cmath>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace game
{

const std::string MapIO::MAP_VERSION("0.4.0");

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

bool MapIO::Load(const std::string & filename, bool binary)
{
    if(binary)
        return LoadBinary(filename);
    else
        return LoadText(filename);
}

bool MapIO::LoadHeader(const std::string & filename, bool binary)
{
    if(binary)
        return LoadHeaderBinary(filename);
    else
        return LoadHeaderText(filename);
}

bool MapIO::Save(const std::string & filename, const std::vector<GameMapCell> & cells,
                 const std::vector<GameObject *> & objects, const std::vector<MissionGoal> & goals,
                 int rows, int cols, bool binary)
{
    // no data to save -> EXIT
    if(0 == rows || 0 == cols)
        return false;

    if(binary)
        return SaveBinary(filename, cells, objects, goals, rows, cols);
    else
        return SaveText(filename, cells, objects, goals, rows, cols);
}

bool MapIO::LoadBinary(const std::string & filename)
{
    using namespace sgl;

    // OPEN file
    utilities::BinaryFile bf(filename, utilities::BinaryFile::OPEN_INPUT, false);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] MapIO::LoadBinary - can't open file " << filename << std::endl;
        return false;
    }

    if(!ReadHeaderBinary(bf))
        return false;

    ReadDataBinary(bf);

    return true;
}

bool MapIO::LoadText(const std::string & filename)
{
    // open map file
    std::fstream fs(filename);

    if(!fs.is_open())
        return false;

    if(!ReadHeaderText(fs))
        return false;

    ReadDataText(fs);

    fs.close();

    return true;
}

bool MapIO::LoadHeaderBinary(const std::string & filename)
{
    using namespace sgl;

    // OPEN file
    utilities::BinaryFile bf(filename, utilities::BinaryFile::OPEN_INPUT, false);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] MapIO::LoadHeaderBinary - can't open file " << filename << std::endl;
        return false;
    }

    return ReadHeaderBinary(bf);
}

bool MapIO::LoadHeaderText(const std::string & filename)
{
    // open map file
    std::fstream fs(filename);

    if(!fs.is_open())
        return false;

    return ReadHeaderText(fs);
}

bool MapIO::SaveBinary(const std::string & filename, const std::vector<GameMapCell> & cells,
                       const std::vector<GameObject *> & objects,
                       const std::vector<MissionGoal> & goals, int rows, int cols)
{
    using namespace sgl;

    // OPEN file
    utilities::BinaryFile bf(filename, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] MapIO::SaveBinary - can't open file " << filename << std::endl;
        return false;
    }

    // -- HEADER --
    // save current map version
    bf.WriteString(MAP_VERSION);

    // mission goals
    const unsigned int numGoals = goals.size();
    bf.WriteUint(numGoals);

    for(const MissionGoal & g : goals)
    {
        bf.WriteBool(g.IsPrimary());
        bf.WriteBool(g.IsHidden());
        bf.WriteSizeT(g.GetType());
        bf.WriteUint(g.GetQuantity());
        bf.WriteUint(g.GetExtraValue());
    }

    // map size
    const int mapSize = rows * cols;
    bf.WriteUint(rows);
    bf.WriteUint(cols);

    // stats
    const int statBlobs = DefineStatResourceForCells(cells, CT_BLOBS_SOURCE);
    bf.WriteInt(statBlobs);

    const int statDiamonds = DefineStatResourceForCells(cells, CT_DIAMONDS_SOURCE);
    bf.WriteInt(statDiamonds);

    const int statEnergy = DefineStatResourceForGenerators(objects, mapSize, ObjectData::TYPE_RES_GEN_ENERGY);
    bf.WriteInt(statEnergy);

    const int statMaterial = DefineStatResourceForGenerators(objects, mapSize, ObjectData::TYPE_RES_GEN_MATERIAL);
    bf.WriteInt(statMaterial);

    const int statValue = DefineStatValue(statBlobs, statDiamonds, statEnergy, statMaterial);
    bf.WriteInt(statValue);

    // -- DATA --
    // save cells type
    for(unsigned int r = 0; r < rows; ++r)
    {
        const unsigned int ind0 = r * cols;

        for(unsigned int c = 0; c < cols; ++c)
        {
            const unsigned int ind = ind0 + c;
            bf.WriteInt(cells[ind].currType);
        }
    }

    // save objects
    const unsigned int numObjs = objects.size();
    bf.WriteUint(numObjs);

    for(const GameObject * obj : objects)
    {
        bf.WriteSizeT(obj->GetObjectType());
        bf.WriteSizeT(obj->GetObjectVariant());
        bf.WriteUint(obj->GetFaction());
        bf.WriteInt(obj->GetRow0());
        bf.WriteInt(obj->GetCol0());
    }

    return true;
}

bool MapIO::SaveText(const std::string & filename, const std::vector<GameMapCell> & cells,
                     const std::vector<GameObject *> & objects,
                     const std::vector<MissionGoal> & goals, int rows, int cols)
{
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
        fs << MAP_TAG_GOAL << " " << g.IsPrimary() << " " << g.IsHidden() << " "
           << g.GetType() << " " << g.GetQuantity() << " "
           << g.GetExtraValue() << "\n";

    // save map size
    fs << "# ====== MAP =====\n";
    fs << MAP_TAG_MAP_SIZE << " " << rows << " " << cols << "\n";

    // stats
    const int mapSize = rows * cols;

    const int statBlobs = DefineStatResourceForCells(cells, CT_BLOBS_SOURCE);
    fs << MAP_TAG_STAT_BLOBS << " " << statBlobs << "\n";

    const int statDiamonds = DefineStatResourceForCells(cells, CT_DIAMONDS_SOURCE);
    fs << MAP_TAG_STAT_DIAMONDS << " " << statDiamonds << "\n";

    const int statEnergy = DefineStatResourceForGenerators(objects, mapSize, ObjectData::TYPE_RES_GEN_ENERGY);
    fs << MAP_TAG_STAT_ENERGY << " " << statEnergy << "\n";

    const int statMaterial = DefineStatResourceForGenerators(objects, mapSize, ObjectData::TYPE_RES_GEN_MATERIAL);
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
        fs << obj->GetObjectType() << " "
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
    const float maxResPerc = 0.01f;

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

int MapIO::DefineStatResourceForGenerators(const std::vector<GameObject *> & objects,
                                        int mapSize, GameObjectTypeId type) const
{
    const int numGenerators = std::count_if(objects.begin(), objects.end(),
                                            [type](const GameObject * obj)
                                            {
                                                return obj->GetObjectType() == type;
                                            });
    const float generatorArea = 4.f;
    const float scaleMult = 1.25f;
    const float perc = scaleMult * (generatorArea * numGenerators) * 100.f / mapSize;

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

bool MapIO::ReadHeaderBinary(sgl::utilities::BinaryFile & bf)
{
    // current map version
    bf.ReadString(mVer);

    // map version doesn't match expected one
    if(mVer != MAP_VERSION)
    {
#ifdef DEV_MODE
        std::cout << "[ERR] MapIO::ReadHeaderBinary - map file version (" << mVer
                  << ") is different from current one (" << MAP_VERSION << ")"
                  << std::endl;
#endif
                return false;
            }

    // mission goals
    const unsigned int numGoals = bf.ReadUint();

    for(unsigned int i = 0; i < numGoals; i++)
    {
        const bool primary = bf.ReadBool();
        const bool hidden = bf.ReadBool();
        const auto type = static_cast<MissionGoalType>(bf.ReadSizeT());
        const unsigned int quantity = bf.ReadUint();
        const unsigned int extra = bf.ReadUint();

        // store goal
        mGoals.emplace_back(type, quantity, extra, primary, hidden);

        // assign category based on first primary goal
        if(primary && MC_UNKNOWN == mCategory)
            mCategory = mGoals[mGoals.size() - 1].GetCategory();
    }

    // map size
    mRows = bf.ReadUint();
    mCols = bf.ReadUint();

    // stats
    mStatBlobs = bf.ReadInt();
    mStatDiamonds = bf.ReadInt();
    mStatEnergy = bf.ReadInt();
    mStatMaterial = bf.ReadInt();
    mStatValue = bf.ReadInt();

    return true;
}

bool MapIO::ReadHeaderText(std::fstream & fs)
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
                std::cout << "[ERR] MapIO::ReadHeaderText - map file version (" << mVer
                          << ") is different from current one (" << MAP_VERSION << ")"
                          << std::endl;
#endif

                return false;
            }
        }
        // reading goal
        else if(line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_GOAL) == 0)
        {
            ss.ignore(LEN_SIMPLE_TAG);

            // check if primary or secondary goal
            bool primary = false;
            ss >> primary;

            // check if hidden goal
            bool hidden = false;
            ss >> hidden;

            // goal type
            MissionGoalType type = MissionGoal::TYPE_NULL;
            ss >> type;

            // quantity data
            unsigned int quantity = 0;
            ss >> quantity;

            // extra value
            unsigned int extraVal = 0;
            ss >> extraVal;

            // store goal
            mGoals.emplace_back(type, quantity, extraVal, primary, hidden);

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

    return true;
}

void MapIO::ReadDataBinary(sgl::utilities::BinaryFile & bf)
{
    // read cells type
    mCellTypes.reserve(mRows * mCols);

    for(unsigned int r = 0; r < mRows; ++r)
    {
        for(unsigned int c = 0; c < mCols; ++c)
            mCellTypes.emplace_back(bf.ReadInt());
    }

    // save objects
    const unsigned int numObjs = bf.ReadUint();

    for(unsigned int i = 0; i < numObjs; ++i)
    {
        MapObjectEntry e;

        e.typeId = bf.ReadSizeT();
        e.variantId = bf.ReadSizeT();
        e.faction = bf.ReadUint();
        e.r0 = bf.ReadInt();
        e.c0 = bf.ReadInt();

        mObjEntries.emplace_back(e);
    }
}

void MapIO::ReadDataText(std::fstream & fs)
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

    // READ OBJECTS
    while(std::getline(fs, line))
    {
        // skip comments
        if(!line.empty() && line.compare(0, LEN_SIMPLE_TAG, MAP_TAG_COMMENT) == 0)
            continue;

        ss.clear();
        ss.str(line);

        MapObjectEntry e;

        ss >> e.typeId >> e.variantId >> e.faction >> e.r0 >> e.c0;

        mObjEntries.emplace_back(e);
    }
}

} // namespace game
