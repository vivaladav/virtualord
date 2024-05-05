#pragma once

#include <sgl/core/Point.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace sgl { namespace graphic { class Image; } }

namespace game
{

class IsoLayer;
class IsoObject;

struct Cell2D;

/// Isometric map made of isometric cells. It might contain IsoLayers that are rendered on top of the map.
class IsoMap
{
public:
    IsoMap(int tileW);
    IsoMap(int rows, int cols, int tileW);
    ~IsoMap();

    void Clear();

    void SetSize(unsigned int rows, unsigned int cols, bool force);

    int GetNumRows() const;
    int GetNumCols() const;

    sgl::core::Pointd2D GetCellPosition(unsigned int r, unsigned int c) const;
    sgl::core::Pointd2D GetCellPosition(unsigned int index) const;

    void SetTiles(const std::vector<std::string> & files);
    void SetTiles(const char * texFile, int numSprites);

    void SetCellType(unsigned int r, unsigned int c, unsigned int type);
    void SetCellType(unsigned int index, unsigned int type);
    int GetCellType(unsigned int r, unsigned int c) const;
    int GetCellType(unsigned int index) const;

    int GetTileWidth() const;
    int GetTileHeight() const;

    int GetWidth() const;
    int GetHeight() const;

    sgl::core::Pointd2D GetOrigin() const;
    void SetOrigin(int x, int y);

    void SetVisibleArea(int x, int y, int w, int h);
    void Render();

    Cell2D CellFromWorldPoint(int x, int y) const;

    bool IsCellInside(const Cell2D & cell) const;

    // -- IsoLayers --
    IsoLayer * CreateLayer(unsigned int layerId);
    IsoLayer * GetLayer(unsigned int layerId) const;
    void SetLayerVisible(unsigned int layerId, bool visible);

    bool ChangeObjectLayer(IsoObject * obj, unsigned int src, unsigned int dst);

private:
    void UpdateTilePositions();

private:
    unsigned int mRows = 0;
    unsigned int mCols = 0;

    unsigned int mRenderingR0 = 0;
    unsigned int mRenderingR1 = 0;
    unsigned int mRenderingC0 = 0;
    unsigned int mRenderingC1 = 0;

    int mX0 = 0;
    int mRenderX0 = 0;
    int mY0 = 0;

    int mTileW = 0;
    int mTileH = 0;
    int mOrthoTileSize = 0;

    // maps
    std::vector<unsigned int> mMap;
    std::vector<sgl::graphic::Image *> mTiles;
    std::vector<sgl::core::Pointd2D> mTilePositions;

    // layers
    std::vector<IsoLayer *> mLayers;
    std::unordered_map<unsigned int, IsoLayer *> mLayersMap;
    std::vector<IsoLayer *> mLayersRenderList;
};

// ==================== INLINE METHODS ====================

/**
 * @brief Gives the top-left corner position of a cell.
 * @param r Row index of the cell
 * @param c Column index of the cell
 * @return A Point2D struct containing the (x,y) coordinates of the cell
 */
inline sgl::core::Pointd2D IsoMap::GetCellPosition(unsigned int r, unsigned int c) const
{
    const unsigned int ind = r * mCols + c;
    return GetCellPosition(ind);
}

/**
 * @brief Gives the number of rows in the map.
 * @return Number of rows in the map
 */
inline int IsoMap::GetNumRows() const { return mRows; }

/**
 * @brief Gives the number of columns in the map.
 * @return Number of colums in the map
 */
inline int IsoMap::GetNumCols() const { return mCols; }

/**
 * @brief Set the type of a cell, which basically defines what image is used to represent it.
 * @param r Row coordinate of the cell
 * @param c Column coordinate of the cell
 * @param type Type ID that defines what image to use for the cell
 */
inline void IsoMap::SetCellType(unsigned int r, unsigned int c, unsigned int type)
{
    if(r < mRows && c < mCols)
        SetCellType(r * mCols + c, type);
}

/**
 * @brief Set the type of a cell, which basically defines what image is used to represent it.
 * @param index Cell index inside the map
 * @param type Type ID that defines what image to use for the cell
 */
inline void IsoMap::SetCellType(unsigned int index, unsigned int type)
{
    if(index < mMap.size() && type < mTiles.size())
        mMap[index] = type;
}

/**
 * @brief Gets the type of a cell. Basically an ID that defines what image is used to represent it.
 * @param r Row coordinate of the cell
 * @param c Column coordinate of the cell
 * @return Type ID that defines what image to use for the cell
 */
inline int IsoMap::GetCellType(unsigned int r, unsigned int c) const
{
    if(r < mRows && c < mCols)
        return GetCellType(r * mCols + c);
    else
        return -1;
}

/**
 * @brief Gets the type of a cell. Basically an ID that defines what image is used to represent it.
 * @param index Cell index inside the map
 * @return Type ID that defines what image to use for the cell
 */
inline int IsoMap::GetCellType(unsigned int index) const
{
    if(index < mMap.size())
        return mMap[index];
    else
        return -1;
}

/**
 * @brief Gets the width of a tile.
 * @return Tile width, in pixels
 */
inline int IsoMap::GetTileWidth() const { return mTileW; }

/**
 * @brief Gets the height of a tile.
 * @return Tile height, in pixels
 */
inline int IsoMap::GetTileHeight() const { return mTileH; }

/**
 * @brief Gets the width of the whole map.
 * @return Width of the map, in pixels
 */
inline int IsoMap::GetWidth() const { return mTileW * mCols; }

/**
 * @brief Gets the height of the whole map.
 * @return Height of the map, in pixels
 */
inline int IsoMap::GetHeight() const { return mTileH * mRows; }


inline sgl::core::Pointd2D IsoMap::GetOrigin() const { return { mX0, mY0 }; }

/**
 * @brief Gets an IsoLayer stored at a given position in the stack of layers.
 * @param index Index that identifies a specific layer in the map. Starting from 0
 * @return
 */
inline IsoLayer * IsoMap::GetLayer(unsigned int layerId) const
{
    auto res = mLayersMap.find(layerId);

    if(res != mLayersMap.end())
        return res->second;
    else
        return nullptr;
}

} // namespace game
