#pragma once

#include <vector>

namespace game
{

class GameObject;
class IsoLayer;
class IsoObject;
class Player;

enum PlayerFaction : unsigned int;

class ControlMap
{
public:
    ControlMap(IsoLayer * layer, Player * localPlayer);
    ~ControlMap();
    void SetSize(unsigned int rows, unsigned int cols);

    void AddControlPointsForCell(unsigned int r, unsigned int c, PlayerFaction faction);
    void AddControlPointsForObject(GameObject * obj);

    void UpdateVisualAreas();

    int GetPercentageControlledByFaction(PlayerFaction f) const;

private:
    void AddControlPointsToArea(int rTL, int cTL, int rBR, int cBR,
                                PlayerFaction faction, int maxPoints);

    void UpdateControllers();

    IsoObject * GetNewMarker();
    void ClearUsedMarkers();

    unsigned int GetMarkerType(unsigned int r, unsigned int c, PlayerFaction f) const;
    bool IsFactionCell(unsigned int r, unsigned int c, PlayerFaction f) const;

private:
    struct ControlCell
    {
        std::vector<int> points;
        PlayerFaction controller;
    };

    enum CellNeighbors : unsigned int
    {
        CN_TL    = 0x01,
        CN_T     = 0x02,
        CN_TR    = 0x04,
        CN_R     = 0x08,
        CN_BR    = 0x10,
        CN_B     = 0x20,
        CN_BL    = 0x40,
        CN_L     = 0x80,

        CN_ALL_T = CN_TL | CN_T | CN_TR,
        CN_ALL_B = CN_BL | CN_B | CN_BR,
        CN_ALL_L = CN_TL | CN_L | CN_BL,
        CN_ALL_R = CN_TR | CN_R | CN_BR,
        CN_ALL = CN_ALL_T | CN_ALL_B | CN_L | CN_R
    };

    static const int SECTOR_SIZE = 5;

private:
    std::vector<ControlCell> mMap;
    std::vector<IsoObject * > mMarkers;
    std::vector<unsigned int> mTypesMap;
    unsigned int mUsedMarkers = 0;

    IsoLayer * mLayer = nullptr;
    Player * mPlayer = nullptr;

    unsigned int mRows = 0;
    unsigned int mCols = 0;
};

// ==================== INLINE METHODS ====================

inline void ControlMap::ClearUsedMarkers() { mUsedMarkers = 0; }

} // namespace game
