#include "CameraMapController.h"

#include "Game.h"
#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/core/event/MouseButtonEvent.h>
#include <sgl/core/event/MouseMotionEvent.h>
#include <sgl/graphic/Camera.h>

namespace game
{

constexpr float DEF_SCROLLING_SPEED = 500.f;
constexpr float DEF_DRAGGING_SPEED = 90.f;
constexpr int SCROLL_L = -1;
constexpr int SCROLL_R = 1;
constexpr int SCROLL_U = -1;
constexpr int SCROLL_D = 1;
constexpr int NO_SCROLL = 0;

CameraMapController::CameraMapController(sgl::graphic::Camera * cam, Game * game)
    : mCamera(cam)
    , mGame(game)
    , mSpeedScrolling(DEF_SCROLLING_SPEED)
    , mSpeedDragging(DEF_DRAGGING_SPEED)
    , mDirX(NO_SCROLL)
    , mDirY(NO_SCROLL)
{
}

void CameraMapController::SetEnabled(bool enabled)
{
    mEnabled = enabled;

    if(!enabled)
        ClearMovement();
}

void CameraMapController::SetMapArea(const sgl::core::Pointd2D & t, const sgl::core::Pointd2D & r,
                                     const sgl::core::Pointd2D & b, const sgl::core::Pointd2D & l)
{
    mMapT = t;
    mMapR = r;
    mMapB = b;
    mMapL = l;
}

void CameraMapController::CenterCameraToPoint(int x, int y)
{
    // NOTE
    // I removed the "enabled" check for now as it affects the tutorial a lot.
    // this should not be a problem, but if a finer control is required it's possible
    // to add a SetInputEnabled to disable all mouse/keyboard interaction while leaving on
    // programatically moves like this one

    sgl::core::Pointd2D p(x, y);

    // if point not inside -> use orthogonal projection
    // LEFT
    if(x < mMapT.x)
    {
        // TOP - LEFT
        if(y < mMapL.y)
        {
            if(!IsPointInsideTL(p))
                p = GetVectorProjection(mMapL, mMapT, p);
        }
        // BOTTOM - LEFT
        else
        {
            // point not inside -> orthogonal projection
            if(!IsPointInsideBL(p))
                p = GetVectorProjection(mMapL, mMapB, p);
        }
    }
    // RIGHT
    else
    {
        // TOP - RIGHT
        if(y < mMapL.y)
        {
            if(!IsPointInsideTR(p))
                p = GetVectorProjection(mMapT, mMapR, p);
        }
        // BOTTOM - RIGHT
        else
        {
            if(!IsPointInsideTR(p))
                p = GetVectorProjection(mMapB, mMapR, p);
        }
    }

    mCamera->CenterToPoint(p.x, p.y);
}

void CameraMapController::ResetPosition()
{
    mCamera->ResetPosition();
}

void CameraMapController::HandleKeyDown(sgl::core::KeyboardEvent & event)
{
    using namespace sgl::core;

    if(!mEnabled)
        return ;

    const bool wasScrolling = mScrolling;
    const int key = event.GetKey();

    if(key == KeyboardEvent::KEY_A)
    {
        if(!mMouseScrollX)
        {
            mDirX = SCROLL_L;
            mKeyScrollX = true;
        }
    }
    else if(key == KeyboardEvent::KEY_D)
    {
        if(!mMouseScrollX)
        {
            mDirX = SCROLL_R;
            mKeyScrollX = true;
        }
    }
    else if(key == KeyboardEvent::KEY_W)
    {
        if(!mMouseScrollY)
        {
            mDirY = SCROLL_U;
            mKeyScrollY = true;
        }
    }
    else if(key == KeyboardEvent::KEY_S)
    {
        if(!mMouseScrollY)
        {
            mDirY = SCROLL_D;
            mKeyScrollY = true;
        }
    }

    mScrolling = mKeyScrollX || mKeyScrollY || mMouseScrollX || mMouseScrollY;

    if(mScrolling && !wasScrolling)
        InitScrollingVelocity();
}

void CameraMapController::HandleKeyUp(sgl::core::KeyboardEvent & event)
{
    using namespace sgl::core;

    if(!mEnabled)
        return ;

    const int key = event.GetKey();

    if(key == KeyboardEvent::KEY_A)
    {
        if(!mMouseScrollX)
            mDirX = NO_SCROLL;

        mKeyScrollX = false;
    }
    else if(key == KeyboardEvent::KEY_D)
    {
        if(!mMouseScrollX)
            mDirX = NO_SCROLL;

        mKeyScrollX = false;
    }
    else if(key == KeyboardEvent::KEY_W)
    {
        if(!mMouseScrollY)
            mDirY = NO_SCROLL;

        mKeyScrollY = false;
    }
    else if(key == KeyboardEvent::KEY_S)
    {
        if(!mMouseScrollY)
            mDirY = NO_SCROLL;

        mKeyScrollY = false;
    }

    mScrolling = mKeyScrollX || mKeyScrollY || mMouseScrollX || mMouseScrollY;
}

void CameraMapController::HandleMouseButtonUp(sgl::core::MouseButtonEvent & event)
{
    mDragging &= event.GetButton() != sgl::core::MouseEvent::BUTTON_LEFT;
}

void CameraMapController::HandleMouseMotion(sgl::core::MouseMotionEvent & event)
{
    if(!mEnabled)
        return;

    // -- DRAGGING --
    if(mGame->IsMapDragging() && event.IsButtonPushed(sgl::core::MouseEvent::BUTTON_LEFT))
    {
        const int minDrag = 5;

        if(std::abs(event.GetDeltaX()) > minDrag)
            mDragX = event.GetDeltaX();
        else
            mDragX = 0;

        if(std::abs(event.GetDeltaY()) > minDrag)
            mDragY = event.GetDeltaY();
        else
            mDragY = 0;

        mDragging |= mDragX != 0 || mDragY != 0;
    }

    // -- MAP SCROLLING ON EDGES --
    if(!mGame->IsMapScrollingOnEdges())
        return ;

    const int screenX = event.GetX();
    const int screenY = event.GetY();

    const int scrollingMargin = 10;
    const bool wasScrolling = mScrolling;

    if(screenX < scrollingMargin)
    {
        if(!mKeyScrollX)
        {
            mDirX = SCROLL_L;
            mMouseScrollX = true;
        }
    }
    else if(screenX > (mCamera->GetWidth() - scrollingMargin))
    {
        if(!mKeyScrollX)
        {
            mDirX = SCROLL_R;
            mMouseScrollX = true;
        }
    }
    else if(!mKeyScrollX)
    {
        mDirX = NO_SCROLL;
        mMouseScrollX = false;
    }

    if(screenY < scrollingMargin)
    {
        if(!mKeyScrollY)
        {
            mDirY = SCROLL_U;
            mMouseScrollY = true;
        }
    }
    else if(screenY > (mCamera->GetHeight() - scrollingMargin))
    {
        if(!mKeyScrollY)
        {
            mDirY = SCROLL_D;
            mMouseScrollY = true;
        }
    }
    else if(!mKeyScrollY)
    {
        mDirY = NO_SCROLL;
        mMouseScrollY = false;
    }

    mScrolling = mKeyScrollX || mKeyScrollY || mMouseScrollX || mMouseScrollY;

    if(mScrolling && !wasScrolling)
        InitScrollingVelocity();
}

void CameraMapController::HandleMouseLeftWindow()
{
    ClearMovement();
}

void CameraMapController::ClearMovement()
{
    mDragging = false;

    mKeyScrollX = false;
    mKeyScrollY = false;
    mMouseScrollX = false;
    mMouseScrollY = false;

    mDirX = NO_SCROLL;
    mDirY = NO_SCROLL;
    mDragX = NO_SCROLL;
    mDragY = NO_SCROLL;
}

void CameraMapController::InitScrollingVelocity()
{
    // constant speed scrolling
    if(mGame->IsMapScrollingConstant())
    {
        mVelocityScrolling = mSpeedScrolling;
        mAccelScrolling = 0.f;
    }
    // initial acceleration scrolling
    else
    {
        const float speedMult = 0.25f;
        mVelocityScrolling = mSpeedScrolling * speedMult;

        if(mVelocityScrolling < mMinSpeedScrolling)
            mVelocityScrolling = mMinSpeedScrolling;

        const float timeAccelerating = 1.f;
        mAccelScrolling = (mSpeedScrolling - mVelocityScrolling) / timeAccelerating;
    }
}

void CameraMapController::Update(float delta)
{
    if(!mEnabled)
        return;

    const float halfP = 0.5f;

    // NOTE formula to check if center is inside the map area:
    // d = (x−x1)(y2−y1)−(y−y1)(x2−x1)
    sgl::core::Pointd2D cc(mCamera->GetX() + (mCamera->GetWidth() / 2),
                           mCamera->GetY() + (mCamera->GetHeight() / 2));

    // ----- DRAGGING -----
    if(mDragging)
    {
        // HORIZONTAL
        const float dragX = -mDragX * mSpeedDragging * delta;

        if(mDragX > 0)
        {
            cc.x += static_cast<int>(dragX + halfP);

            if((cc.y < mMapL.y && IsPointInsideTL(cc)) || (cc.y >= mMapL.y && IsPointInsideBL(cc)))
                mCamera->MoveX(dragX);
        }
        else if(mDragX < 0)
        {
            cc.x += static_cast<int>(dragX - halfP);

            if((cc.y < mMapL.y && IsPointInsideTR(cc)) || (cc.y >= mMapL.y && IsPointInsideBR(cc)))
                mCamera->MoveX(dragX);
        }

        // VERTICAL
        const float dragY = -mDragY * mSpeedDragging * delta;

        if(mDragY > 0)
        {
            cc.y += static_cast<int>(dragY + halfP);

            if((cc.x < mMapT.x && IsPointInsideTL(cc)) || (cc.x >= mMapT.x && IsPointInsideTR(cc)))
                mCamera->MoveY(dragY);
        }
        else if(mDragY < 0)
        {
            cc.y += static_cast<int>(dragY - halfP);

            if((cc.x < mMapT.x && IsPointInsideBL(cc)) || (cc.x >= mMapT.x && IsPointInsideBR(cc)))
                mCamera->MoveY(dragY);
        }

        // no scrolling while dragging
        return ;
    }

    // ----- SCROLLING -----
    if(mAccelScrolling > 0.f)
    {
        mVelocityScrolling += mAccelScrolling * delta;

        // reached peak velocity
        if(mVelocityScrolling > mSpeedScrolling)
        {
            mVelocityScrolling = mSpeedScrolling;
            mAccelScrolling = 0.f;
        }
    }

    // HORIZONTAL
    const float movX = mDirX * mVelocityScrolling * delta;

    if(mDirX < 0)
    {
        cc.x += static_cast<int>(movX - halfP);

        if((cc.y < mMapL.y && IsPointInsideTL(cc)) || (cc.y >= mMapL.y && IsPointInsideBL(cc)))
            mCamera->MoveX(movX);
    }
    else if(mDirX > 0)
    {
        cc.x += static_cast<int>(movX + halfP);

        if((cc.y < mMapL.y && IsPointInsideTR(cc)) || (cc.y >= mMapL.y && IsPointInsideBR(cc)))
            mCamera->MoveX(movX);
    }

    // VERTICAL
    const float movY = mDirY * mVelocityScrolling * delta;

    if(mDirY < 0)
    {
        cc.y += static_cast<int>(movY - halfP);

        if((cc.x < mMapT.x && IsPointInsideTL(cc)) || (cc.x >= mMapT.x && IsPointInsideTR(cc)))
            mCamera->MoveY(movY);
    }
    else if(mDirY > 0)
    {
        cc.y += static_cast<int>(movY + halfP);

        if((cc.x < mMapT.x && IsPointInsideBL(cc)) || (cc.x >= mMapT.x && IsPointInsideBR(cc)))
           mCamera->MoveY(movY);
    }
}

sgl::core::Pointd2D CameraMapController::GetVectorProjection(const sgl::core::Pointd2D & a0,
                                                             const sgl::core::Pointd2D & b0,
                                                             const sgl::core::Pointd2D & p) const
{
    const int ax = p.x - a0.x;
    const int ay = p.y - a0.y;
    const int bx = b0.x - a0.x;
    const int by = b0.y - a0.y;
    const float m = static_cast<float>(ax * bx + ay + by) / static_cast<float>(bx * bx + by * by);

    const sgl::core::Pointd2D ret((bx * m) + a0.x, (by * m) + a0.y);
    return ret;
}

} // namespace game
