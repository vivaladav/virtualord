#include "IsoMap.h"

#include "Cell2D.h"
#include "IsoLayer.h"
#include "IsoObject.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

#include <algorithm>

namespace game
{

// ==================== CONSTRUCTORS AND DESTRUCTOR ====================

/**
 * @brief Creates an empty map with no dimension.
 * @param tileW Width of a cell (height is half of that)
 */
IsoMap::IsoMap(int tileW)
    : mTileW(tileW)
    , mTileH(tileW * 0.5f)
{
}

/**
 * @brief Creates an isometric map with a fixed dimension.
 * @param rows Number of rows in the map
 * @param cold Number of columns in the map
 * @param tileW Width of a cell (height is half of that)
 */
IsoMap::IsoMap(int rows, int cols, int tileW)
    : IsoMap(tileW)
{
    SetSize(rows, cols, false);
}

/// Destructor, deletes Images used for the tiles and IsoLayers.
IsoMap::~IsoMap()
{
    for(sgl::graphic::Image * img : mTiles)
        delete img;

    // -- IsoLayers --
    for(IsoLayer * layer : mLayers)
        delete layer;
}

// ==================== PUBLIC METHODS ====================

void IsoMap::Clear()
{
    const int mapSize = mRows * mCols;

    mMap.assign(mapSize, 0);
    mTilePositions.assign(mapSize, sgl::core::Pointd2D());

    mLayersRenderList.clear();
}

void IsoMap::SetSize(unsigned int rows, unsigned int cols, bool force)
{
    if(!force && (rows == mRows && cols == mCols))
        return ;

    mRows = rows;
    mCols = cols;

    mRenderingR0 = 0;
    mRenderingC0 = 0;
    mRenderingR1 = rows;
    mRenderingC1 = cols;

    // update map
    const int mapSize = rows * cols;

    mMap.clear();
    mMap.reserve(mapSize);
    mMap.assign(mapSize, 0);

    // update tiles
    mTilePositions.clear();
    mTilePositions.reserve(mapSize);
    mTilePositions.assign(mapSize, sgl::core::Pointd2D());

    UpdateTilePositions();

    // update layers
    for(IsoLayer * layer : mLayers)
        layer->UpdateSize(force);
}

/**
 * @brief Gives the top-left corner position of a cell.
 * @param index Position index in the map. Identifies the cell
 * @return A Point2D struct containing the (x,y) coordinates of the cell
 */
sgl::core::Pointd2D IsoMap::GetCellPosition(unsigned int index) const
{
    if(index < mTilePositions.size())
        return mTilePositions[index];
    else
    {
        const sgl::core::Pointd2D p(-1, -1);
        return p;
    }
}

/**
 * @brief Creates the Images used to represent the tiles in the map.
 * @param files Files to create images. Paths are relative to binary
 */
void IsoMap::SetTiles(const std::vector<std::string> & files)
{
    for(const std::string & file : files)
    {
        auto * img = new sgl::graphic::Image(file.c_str());

        mTiles.emplace_back(img);
    }

    UpdateTilePositions();
}

void IsoMap::SetTiles(const char * texFile, int numSprites)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    for(int i = 0; i < numSprites; ++i)
    {
        auto * img = new sgl::graphic::Image(tm->GetSprite(texFile, i));

        mTiles.emplace_back(img);
    }

    UpdateTilePositions();
}

/**
 * @brief Sets the origin point of the map. This is the point where the top corner of the 0,0 iso cell will be placed.
 * @param x X coordinate in pixels
 * @param y Y coordinate in pixels
 */
void IsoMap::SetOrigin(int x, int y)
{
    // rendering delta
    const int deltaX = x - mX0 - mTileH;
    const int deltaY = y - mY0;

    mX0 = x;
    mY0 = y;

    // rendering x is shifted of -w/2 because image origin is TL corner
    mRenderX0 = x - mTileH;

    UpdateTilePositions();

    // propagate move to layers
    for(IsoLayer * layer : mLayersRenderList)
        layer->MoveObjectsPosition(deltaX, deltaY);
}

// NOTE Calling this method before the map size is set has NO effect
void IsoMap::SetVisibleArea(int x, int y, int w, int h)
{
    const int x1 = x + w;
    const int y1 = y + h;

    // TOP LEFT
    Cell2D TL = CellFromWorldPoint(x, y);

    if(TL.col < 0)
        TL.col = 0;

    mRenderingC0 = TL.col;

    // TOP RIGHT
    Cell2D TR = CellFromWorldPoint(x1, y);

    if(TR.row < 0)
        TR.row = 0;

    mRenderingR0 = TR.row;

    // BOTTOM LEFT
    Cell2D BL = CellFromWorldPoint(x, y1);

    ++BL.row;

    if(BL.row > static_cast<int>(mRows))
        BL.row = mRows;

    mRenderingR1 = BL.row;

    // BOTTOM RIGHT
    Cell2D BR = CellFromWorldPoint(x1, y1);

    ++BR.col;

    if(BR.col > static_cast<int>(mCols))
        BR.col = mCols;

    mRenderingC1 = BR.col;

    // UPDATE LAYERS
    for(IsoLayer * layer : mLayersRenderList)
        layer->SetRenderingCells(mRenderingR0, mRenderingC0, mRenderingR1, mRenderingC1);
}

/// Renders all the cells in the map and on top of that all the IsoLayers.
void IsoMap::Render()
{
    for(unsigned int r = mRenderingR0; r < mRenderingR1; ++r)
    {
        const unsigned int indb = r * mCols;

        for(unsigned int c = mRenderingC0; c < mRenderingC1; ++c)
        {
            const unsigned int ind = indb + c;

            sgl::graphic::Image * img = mTiles[mMap[ind]];

            const sgl::core::Pointd2D & p = mTilePositions[ind];

            img->SetPosition(p.x, p.y);
            img->Render();
        }
    }

    // -- IsoLayers --
    for(IsoLayer * layer : mLayersRenderList)
        layer->Render();
}

/**
 * @brief Identifies the cell corresponding to a (x,y) screen position.
 * @param x X coordinate of the position
 * @param y Y coordinate of the position
 * @return A Cell2D struct containing a (row,col) pair that identifies the correspongind cell
 */
Cell2D IsoMap::CellFromWorldPoint(int x, int y) const
{
    const float xf = x - mX0;
    const float yf = y - mY0;
    const float w = mTileW;
    const float h = mTileH;

    float col = (2. * yf + xf) / w;
    float row = col - (xf / h);

    // negative cells need special handling
    if(row < 0.f)
        row -= 1.f;

    if(col < 0.f)
        col -= 1.f;

    const Cell2D cell(static_cast<int>(row), static_cast<int>(col));
    return cell;
}

/**
 * @brief Checks if a cell is inside the map borders.
 * @param cell Struct containing the row,col coordinates of the cell
 * @return TRUE if the cell is inside the map, FALSE otherwise
 */
bool IsoMap::IsCellInside(const Cell2D & cell) const
{
    const unsigned int cr = static_cast<unsigned int>(cell.row);
    const unsigned int cc = static_cast<unsigned int>(cell.col);
    return  cr < mRows && cc < mCols;
}

/**
 * @brief Creates and stores an IsoLayer.
 * @param layerId Layer ID
 * @param files Files to create images. Paths are relative to binary
 * @return A pointer to the new IsoLayer
 */
IsoLayer * IsoMap::CreateLayer(unsigned int layerId)
{
    // check layer has not been created yet
    IsoLayer * layer = GetLayer(layerId);

    if(layer != nullptr)
        return layer;

    // create and store new layer
    layer = new IsoLayer(this);

    mLayers.emplace_back(layer);
    mLayersMap.insert({layerId, layer});
    mLayersRenderList.emplace_back(layer);

    return layer;
}

/**
 * @brief Makes an IsoLayer visible or invisible.
 * @param layerId Layer ID
 * @param visible TRUE if you want to make the layer visible, FALSE otherwise
 */
void IsoMap::SetLayerVisible(unsigned int layerId, bool visible)
{
    IsoLayer * layer = GetLayer(layerId);

    // layer not found
    if(nullptr == layer)
        return ;

    // nothing to do
    if(layer->IsVisible() == visible)
        return ;

    layer->SetVisible(visible);

    if(!visible)
    {
        auto it = std::find(mLayersRenderList.begin(), mLayersRenderList.end(), layer);

        if(it != mLayersRenderList.end())
            mLayersRenderList.erase(it);
    }
    else
    {
        // layer should be first
        if(layer == mLayers.front())
            mLayersRenderList.insert(mLayersRenderList.begin(), layer);
        // layer should be last
        else if(layer == mLayers.back())
            mLayersRenderList.emplace_back(layer);
        // layer should be in the middle
        else
        {
            for(unsigned int i = 1; i < (mLayers.size() - 1); ++i)
            {
                if(mLayers[i] == layer)
                {
                    mLayersRenderList.insert(mLayersRenderList.begin() + i, layer);
                    break;
                }
            }
        }
    }
}

bool IsoMap::ChangeObjectLayer(IsoObject * obj, unsigned int src, unsigned int dst)
{
    // same layer -> nothing to do
    if(src == dst)
        return true;

    // find layers
    IsoLayer * layerSrc = GetLayer(src);

    if(nullptr == layerSrc)
        return false;

    IsoLayer * layerDst = GetLayer(dst);

    if(nullptr == layerDst)
        return false;

    // move object
    layerSrc->ClearObject(obj);
    const bool res = layerDst->AddObject(obj, obj->GetRow(), obj->GetCol());

    return res;
}

// ==================== PRIVATE METHODS ====================

/// Updates the position of all the tiles. Based on tile size and map origin.
void IsoMap::UpdateTilePositions()
{
    // no positions to update yet
    if(mTilePositions.empty())
        return ;

     const int tileHalfH = mTileH * 0.5f;

     // TODO consider image size of the different tiles
    for(unsigned int r = 0; r < mRows; ++r)
    {
        const unsigned int indb = r * mCols;

        for(unsigned int c = 0; c < mCols; ++c)
        {
            const unsigned int ind = indb + c;

            sgl::core::Pointd2D & p = mTilePositions[ind];

            // x =  (h * c)   -   (h * r)   = h * (c - r)
            // y = (h/2 * c)  +  (h/2 * r)  = h/2 * (c + r)
            p.x = mRenderX0 + mTileH * (c - r);
            p.y = mY0 + tileHalfH * (c + r);
        }
    }
}

} // namespace game
