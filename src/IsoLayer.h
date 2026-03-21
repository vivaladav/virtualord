#pragma once

#include <sgl/core/Point.h>

#include <vector>

namespace game
{

class IsoMap;
class IsoObject;

enum ObjectAlignment
{
    NO_ALIGNMENT    = 0,

    // vertical alignment
    TOP             = 0x01,
    VCENTER         = 0x02,
    BOTTOM          = 0x04,

    // horizontal alignment
    LEFT            = 0x10,
    HCENTER         = 0x20,
    RIGHT           = 0x40,

    // combos
    CENTER          = HCENTER + VCENTER
};

/// A layer of an IsoMap.  Basically It's a container of IsoObjects.
class IsoLayer
{
public:
    IsoLayer(const IsoMap * map);

    void UpdateSize(bool force);

    bool HasObject(IsoObject * obj) const;
    bool HasObject(unsigned int r, unsigned int c) const;
    bool AddObject(IsoObject * obj, unsigned int r, unsigned int c);
    void ClearObject(IsoObject * obj);
    void ClearObject(unsigned int r, unsigned int c);
    bool MoveObject(IsoObject * obj, unsigned int r, unsigned int c, bool updatePosition = true);
    bool MoveObject(unsigned int r0, unsigned int c0, unsigned int r1, unsigned int c1,
                    bool updatePosition = true);

    void SetObjectVisible(IsoObject * obj, bool visible);

    void ClearObjects();

    bool IsVisible() const;
    void SetVisible(bool val);
    void SetRenderingCells(unsigned int r0, unsigned int c0, unsigned int r1, unsigned int c1);
    void Render();

    void MoveObjectsPosition(int deltaX, int deltaY);

    void RepositionObject(IsoObject * obj);

    sgl::core::Pointd2D GetObjectPosition(const IsoObject * obj, unsigned int r, unsigned int c) const;

private:
    void PositionObject(IsoObject * obj, unsigned int r, unsigned int c);

    void ClearObject(unsigned int index);

    void RemoveObjectFromList(IsoObject * obj);

    void RemoveObjectFromRenderList(IsoObject * obj);

    void InsertObjectInRenderList(IsoObject * obj);
    void UpdateRenderList();

    void ClearObjectFromMap(IsoObject * obj);
    void InsertObjectInMap(IsoObject * obj);

private:
    std::vector<IsoObject *> mObjectsList;
    std::vector<IsoObject *> mRenderList;
    std::vector<IsoObject *> mObjectsMap;

    const IsoMap * mMap = nullptr;

    unsigned int mRenderingR0 = 0;
    unsigned int mRenderingR1 = 0;
    unsigned int mRenderingC0 = 0;
    unsigned int mRenderingC1 = 0;

    bool mVisible = true;
};

// ==================== INLINE METHODS ====================

/**
 * @brief Tells if a layer is visible or not.
 * @return TRUE if layer is visible, FALSE otherwise
 */
inline bool IsoLayer::IsVisible() const { return mVisible; }

/**
 * @brief Sets the visibility status of a layer.
 * @param val Visibility status (TRUE or FALSE)
 */
inline void IsoLayer::SetVisible(bool val) { mVisible = val; }

inline void IsoLayer::SetRenderingCells(unsigned int r0, unsigned int c0,
                                        unsigned int r1, unsigned int c1)
{
    // nothing has changed
    if(mRenderingR0 == r0 && mRenderingR1 == r1 && mRenderingC0 == c0 && mRenderingC1 == c1)
        return ;

    mRenderingR0 = r0;
    mRenderingR1 = r1;
    mRenderingC0 = c0;
    mRenderingC1 = c1;

    UpdateRenderList();
}

} // namespace game
