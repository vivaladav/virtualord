#pragma once

#include <sgl/core/Point.h>

namespace sgl
{
    namespace core
    {
        class KeyboardEvent;
        class MouseButtonEvent;
        class MouseMotionEvent;
    }

    namespace graphic { class Camera; }
}

namespace game
{

class Game;
class GameObject;

class CameraMapController
{
public:
    CameraMapController(sgl::graphic::Camera * cam, Game * game);

    const sgl::graphic::Camera * GetCamera() const;

    bool IsEnabled() const;
    void SetEnabled(bool enabled);

    void SetMapArea(const sgl::core::Pointd2D & t, const sgl::core::Pointd2D & r,
                    const sgl::core::Pointd2D & b, const sgl::core::Pointd2D & l);

    void SetDraggingSpeed(int val);
    void SetScrollingSpeed(int val);

    void CenterCameraToPoint(int x, int y);
    void MoveCenterCameraToPoint(int x, int y, float speed = 0.f);
    void TrackObject(const GameObject * obj);
    void ClearObjectTracking();
    void StopMovement();
    void ResetPosition();

    bool IsDragging() const;

    void HandleKeyDown(sgl::core::KeyboardEvent & event);
    void HandleKeyUp(sgl::core::KeyboardEvent & event);
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event);
    void HandleMouseMotion(sgl::core::MouseMotionEvent & event);
    void HandleMouseLeftWindow();

    void Update(float delta);

private:
    bool IsPointInsideTL(const sgl::core::Pointd2D & p) const;
    bool IsPointInsideTR(const sgl::core::Pointd2D & p) const;
    bool IsPointInsideBL(const sgl::core::Pointd2D & p) const;
    bool IsPointInsideBR(const sgl::core::Pointd2D & p) const;

    sgl::core::Pointd2D ClampPointInside(int x, int y) const;

    sgl::core::Pointd2D GetVectorProjection(const sgl::core::Pointd2D & a0,
                                            const sgl::core::Pointd2D & b0,
                                            const sgl::core::Pointd2D & p) const;

    void ClearMovement();

    void InitScrollingVelocity();

    void UpdateMove(float delta);

private:
    sgl::graphic::Camera * mCamera = nullptr;

    Game * mGame = nullptr;

    const GameObject * mObjTracked = nullptr;

    sgl::core::Pointd2D mMapT;
    sgl::core::Pointd2D mMapR;
    sgl::core::Pointd2D mMapB;
    sgl::core::Pointd2D mMapL;

    sgl::core::Pointd2D mTargetMove;

    float mMinSpeedScrolling = 100.f;
    float mSpeedScrolling = 0.f;
    float mSpeedDragging = 0.f;
    float mSpeedMove = 0.f;
    float mVelocityScrolling = 0.f;
    float mAccelScrolling = 0.f;
    float mVelocityMoveX = 0.f;
    float mVelocityMoveY = 0.f;

    int mDirX = 0;
    int mDirY = 0;

    int mDragX = 0;
    int mDragY = 0;

    bool mEnabled = true;

    bool mKeyScrollX = false;
    bool mKeyScrollY = false;
    bool mMouseScrollX = false;
    bool mMouseScrollY = false;
    bool mDragging = false;
    bool mScrolling = false;
    bool mMoving = false;
    bool mMovingX = false;
    bool mMovingY = false;
};

inline const sgl::graphic::Camera * CameraMapController::GetCamera() const
{
    return mCamera;
}

inline bool CameraMapController::IsEnabled() const { return mEnabled; }

inline void CameraMapController::SetDraggingSpeed(int val)
{
    const float mult = 15.f;
    mSpeedDragging = mult * val;

    const float minSpeed = 25.f;

    if(mSpeedDragging < minSpeed)
        mSpeedDragging = minSpeed;
}

inline void CameraMapController::SetScrollingSpeed(int val)
{
    // add 1 so min is guaranteed and 5 is the old 6
    mSpeedScrolling = mMinSpeedScrolling * (val + 1);
}

inline void CameraMapController::TrackObject(const GameObject * obj) { mObjTracked = obj; }
inline void CameraMapController::ClearObjectTracking() { mObjTracked = nullptr; }

inline bool CameraMapController::IsDragging() const { return mDragging; }

inline bool CameraMapController::IsPointInsideTL(const sgl::core::Pointd2D & p) const
{
    return (p.x - mMapT.x) * (mMapL.y - mMapT.y) > (p.y - mMapT.y) * (mMapL.x - mMapT.x);
}

inline bool CameraMapController::IsPointInsideTR(const sgl::core::Pointd2D & p) const
{
    return (p.x - mMapR.x) * (mMapT.y - mMapR.y) > (p.y - mMapR.y) * (mMapT.x - mMapR.x);
}

inline bool CameraMapController::IsPointInsideBL(const sgl::core::Pointd2D & p) const
{
    return (p.x - mMapL.x) * (mMapB.y - mMapL.y) > (p.y - mMapL.y) * (mMapB.x - mMapL.x);
}

inline bool CameraMapController::IsPointInsideBR(const sgl::core::Pointd2D & p) const
{
    return (p.x - mMapB.x) * (mMapR.y - mMapB.y) > (p.y - mMapB.y) * (mMapR.x - mMapB.x);;
}

} // namespace game
