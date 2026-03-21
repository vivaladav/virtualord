#include "IsoLayer.h"

#include "IsoMap.h"
#include "IsoObject.h"

#include <algorithm>

namespace game
{

// ==================== CONSTRUCTORS AND DESTRUCTOR ====================

/**
 * @brief Creates a layer of an IsoMap. It's a container of IsoObjects.
 * @param map Pointer to the owner map
 * @param files Files to create images to display in the layer. Paths are relative to binary
 */
IsoLayer::IsoLayer(const IsoMap * map)
    : mMap(map)
{
    const int size = mMap->GetNumRows() * mMap->GetNumCols();

    mObjectsMap.reserve(size);
    mObjectsMap.assign(size, nullptr);
}

// ==================== PUBLIC METHODS ====================

void IsoLayer::UpdateSize(bool force)
{
    const unsigned int size = mMap->GetNumRows() * mMap->GetNumCols();

    if(!force && size == mObjectsMap.size())
        return ;

    mObjectsMap.clear();
    mObjectsMap.resize(size, nullptr);

    // update objects list
    mObjectsList.clear();
    mRenderList.clear();
}

bool IsoLayer::HasObject(IsoObject * obj) const
{
    return std::find(mObjectsList.begin(), mObjectsList.end(), obj) != mObjectsList.end();
}

/**
 * @brief Checks if a cell contains an object.
 * @param r Row index, starting from 0
 * @param c Col index, starting from 0
 * @return TRUE if cell contains an object, FALSE otherwise
 */
bool IsoLayer::HasObject(unsigned int r, unsigned int c) const
{
    const unsigned int rows = mMap->GetNumRows();
    const unsigned int cols = mMap->GetNumCols();

    // ERROR out of bounds
    if(!(r < rows && c < cols))
        return false;

    const unsigned int index = r * cols + c;

    return mObjectsMap[index] != nullptr;
}

bool IsoLayer::AddObject(IsoObject * obj, unsigned int r, unsigned int c)
{
    const unsigned int mapRows = mMap->GetNumRows();
    const unsigned int mapCols = mMap->GetNumCols();

    // object origin is out of map
    if(r >= mapRows || c >= mapCols)
        return false;

    // full size is out of map
    const unsigned int r1 = 1 + r - obj->GetRows();
    const unsigned int c1 = 1 + c - obj->GetCols();

    if(r1 >= mapRows || c1 >= mapCols)
        return false;

    // object already added
    if(std::find(mObjectsList.begin(), mObjectsList.end(), obj) != mObjectsList.end())
        return false;

    // link layer and object
    obj->SetLayer(this);

    // set object position in layer
    obj->SetRow(r);
    obj->SetCol(c);

    mObjectsList.emplace_back(obj);

    // position it in a cell
    PositionObject(obj, r, c);

    // store object
    InsertObjectInMap(obj);
    InsertObjectInRenderList(obj);

    return true;
}

void IsoLayer::ClearObject(IsoObject * obj)
{
    if(nullptr == obj)
        return ;

    obj->SetLayer(nullptr);

    ClearObjectFromMap(obj);
    RemoveObjectFromList(obj);
    RemoveObjectFromRenderList(obj);
}

/**
 * @brief Destroys an object in a cell.
 * @param r Row index, starting from 0
 * @param c Col index, starting from 0
 */
void IsoLayer::ClearObject(unsigned int r, unsigned int c)
{
    const unsigned int rows = mMap->GetNumRows();
    const unsigned int cols = mMap->GetNumCols();

    // ERROR out of bounds
    if(!(r < rows && c < cols))
        return;

    const unsigned int index = r * cols + c;

    // empty cell -> nothing to do
    if(!mObjectsMap[index])
        return ;

    ClearObject(index);
}

bool IsoLayer::MoveObject(IsoObject * obj, unsigned int r, unsigned int c, bool updatePosition)
{
    return MoveObject(obj->GetRow(), obj->GetCol(), r, c, updatePosition);
}

/**
 * @brief Moves an object from one cell to another.
 * @param r0 Row index of start cell, starting from 0
 * @param c0 Col index of start cell, starting from 0
 * @param r1 Row index of dest cell, starting from 0
 * @param c1 Col index of start cell, starting from 0
 * @param alignment Visual alignment of the object in the cell
 * @return TRUE on success, FALSE otherwise
 */
bool IsoLayer::MoveObject(unsigned int r0, unsigned int c0,
                          unsigned int r1, unsigned int c1,
                          bool updatePosition)
{
    const unsigned int rows = mMap->GetNumRows();
    const unsigned int cols = mMap->GetNumCols();

    // ERROR out of bounds
    if(!(r0 < rows && c0 < cols && r1 < rows && c1 < cols))
        return false;

    const unsigned int index0 = r0 * cols + c0;

    IsoObject * obj = mObjectsMap[index0];

    // nothing to move
    if(!obj)
        return false;

    const unsigned int index1 = r1 * cols + c1;

    // cell is full
    if(mObjectsMap[index1])
        return false;

    // re-position object
    if(updatePosition)
        PositionObject(obj, r1, c1);

    // remove object
    ClearObjectFromMap(obj);
    RemoveObjectFromRenderList(obj);

    // need to update object position before adding it again
    obj->SetRow(r1);
    obj->SetCol(c1);

    // add object back
    InsertObjectInMap(obj);
    InsertObjectInRenderList(obj);

    return true;
}

void IsoLayer::SetObjectVisible(IsoObject * obj, bool visible)
{
    obj->SetVisible(visible);

    if(visible)
    {
        // object is not already in
        if(std::find(mRenderList.begin(), mRenderList.end(), obj) == mRenderList.end())
            InsertObjectInRenderList(obj);
    }
    else
        RemoveObjectFromRenderList(obj);
}

/// Destroy all the objects
void IsoLayer::ClearObjects()
{
    // reset Layer link
    for(auto * obj : mObjectsList)
        obj->SetLayer(nullptr);

    // clear lists
    mObjectsList.clear();
    mRenderList.clear();

    const unsigned int mapsSize = mMap->GetNumRows() * mMap->GetNumCols();
    mObjectsMap.assign(mapsSize, nullptr);
}

/// Renders all the objects in the layer.
void IsoLayer::Render()
{
    for(auto * obj : mRenderList)
        obj->Render();
}

void IsoLayer::MoveObjectsPosition(int deltaX, int deltaY)
{
    for(auto * obj : mObjectsList)
    {
        const int x = obj->GetX() + deltaX;
        const int y = obj->GetY() + deltaY;

        obj->SetPosition(x, y);
    }
}

void IsoLayer::RepositionObject(IsoObject * obj)
{
    PositionObject(obj, obj->GetRow(), obj->GetCol());
}

sgl::core::Pointd2D IsoLayer::GetObjectPosition(const IsoObject * obj, unsigned int r, unsigned int c) const
{
    const sgl::core::Pointd2D cellPos = mMap->GetCellPosition(r, c);
    const int cellH = mMap->GetTileHeight();

    const int x0 = cellPos.x + cellH;
    const int y0 = cellPos.y + cellH;

    const int imgW0 = obj->GetCols() * cellH;
    const int imgH = obj->GetHeight();

    return sgl::core::Pointd2D(x0 - imgW0, y0 - imgH);
}

// ==================== PRIVATE METHODS ====================

void IsoLayer::PositionObject(IsoObject * obj, unsigned int r, unsigned int c)
{
    const sgl::core::Pointd2D pos = GetObjectPosition(obj, r, c);

    obj->SetPosition(pos.x, pos.y);
}

/**
 * @brief Removes an object from a cell. Object is not deleted.
 * @param index Position index in the layer. Identifies the cell
 */
void IsoLayer::ClearObject(unsigned int index)
{
    IsoObject * obj = mObjectsMap[index];

    ClearObject(obj);
}

void IsoLayer::RemoveObjectFromList(IsoObject * obj)
{
    auto it = std::find(mObjectsList.begin(), mObjectsList.end(), obj);

    if(it != mObjectsList.end())
        mObjectsList.erase(it);
}

void IsoLayer::RemoveObjectFromRenderList(IsoObject * obj)
{
    auto it = std::find(mRenderList.begin(), mRenderList.end(), obj);

    if(it != mRenderList.end())
        mRenderList.erase(it);
}

void IsoLayer::InsertObjectInRenderList(IsoObject * obj)
{
    const int objYC = obj->GetY() + (obj->GetHeight() / 2);

    // find where to insert the object
    auto it = mRenderList.begin();

    while(it != mRenderList.end())
    {
        const auto o = *it;
        const int oYC = o->GetY() + (o->GetHeight() / 2);

        if(oYC < objYC)
            ++it;
        else
            break;
    }

    // insert the object to the list
    mRenderList.insert(it, obj);
}

void IsoLayer::UpdateRenderList()
{
    // clear list
    mRenderList.clear();

    // add all objects in rendering area
    const int cols = mMap->GetNumCols();

    for(unsigned int r = mRenderingR0; r < mRenderingR1; ++r)
    {
        const int ind0 = r * cols;

        for(unsigned int c = mRenderingC0; c < mRenderingC1; ++c)
        {
            const unsigned int ind = ind0 + c;

            IsoObject * obj = mObjectsMap[ind];

            if(obj != nullptr && obj->IsVisible() && obj->GetRow() == r && obj->GetCol() == c)
                mRenderList.push_back(obj);
        }
    }

    // sort objects based on Y
    std::sort(mRenderList.begin(), mRenderList.end(), [](IsoObject * o1, IsoObject * o2)
    {
        const int y1 = o1->GetY() + (o1->GetHeight() / 2);
        const int y2 = o2->GetY() + (o2->GetHeight() / 2);

        return y1 < y2;
    });
}

void IsoLayer::ClearObjectFromMap(IsoObject * obj)
{
    const int mapCols = mMap->GetNumCols();

    const int r0 = obj->GetRow();
    const int c0 = obj->GetCol();

    const int r1 = 1 + r0 - obj->GetRows();
    const int c1 = 1 + c0 - obj->GetCols();

    // store object
    for(int r = r1; r <= r0; ++r)
    {
        const unsigned int indBase = r * mapCols;

        for(int c = c1; c <= c0; ++c)
        {
            const unsigned int ind = indBase + c;
            mObjectsMap[ind] = nullptr;
        }
    }
}

void IsoLayer::InsertObjectInMap(IsoObject * obj)
{
    const int mapCols = mMap->GetNumCols();

    const int r0 = obj->GetRow();
    const int c0 = obj->GetCol();

    const int r1 = 1 + r0 - obj->GetRows();
    const int c1 = 1 + c0 - obj->GetCols();

    // store object
    for(int r = r1; r <= r0; ++r)
    {
        const unsigned int indBase = r * mapCols;

        for(int c = c1; c <= c0; ++c)
        {
            const unsigned int ind = indBase + c;
            mObjectsMap[ind] = obj;
        }
    }
}

} // namespace game
