#include "Particles/ParticleSingleLaser.h"

#include "GameMap.h"
#include "GameObjects/GameObject.h"
#include "Particles/DataParticleSingleLaser.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

#include <cmath>

namespace game
{

void ParticleSingleLaser::SetData(const DataParticleSingleLaser & data)
{
    mRenderable.SetTexture(data.tex);
    mRenderable.SetRotation(data.angle);
    SetSpeed(data.speed);
    // this has to be called after SetRotation
    SetStartAndTarget(data.x0, data.y0, data.targetX, data.targetY);
    SetDelay(data.delay);

    mShooter = data.shooter;
    mGameMap = data.map;
    mTarget = data.target;
    mDamage = data.damage;
    mFatal = data.fatal;
}

void ParticleSingleLaser::SetStartAndTarget(int x0, int y0, int tx, int ty)
{
    // adjust initial position based on rotation
    const double rot = mRenderable.GetRotation();
    int x = x0;
    int y = y0;

    const int rdX = roundf((mRenderable.GetWidth() - mRenderable.GetRotatedWidth()) * 0.5f);
    const int rdY = roundf((mRenderable.GetHeight() - mRenderable.GetRotatedHeight()) * 0.5f);

    if(rot < 90)
    {
        x = x0 - rdX;
        y = y0 - rdY;
    }
    else if(rot < 180)
    {
        x = x0 - mRenderable.GetRotatedWidth() - rdX;
        y = y0 - rdY;
    }
    else if(rot < 270)
    {
        x = x0 - mRenderable.GetRotatedWidth() - rdX;
        y = y0;
    }
    // rot < 360
    else
    {
        x = x0;
        y = y0;
    }

    // define velocity
    const float dx = static_cast<float>(tx - x);
    const float dy = static_cast<float>(ty - y);
    const float len = sqrtf(dx * dx + dy * dy);

    mVelX = dx / len;
    mVelY = dy / len;

    // store positions
    mPosXf = static_cast<float>(x);
    mPosYf = static_cast<float>(y);
    mTargetX = tx;
    mTargetY = ty;

    // set initial position
    mRenderable.SetPosition(x, y);

    // reset done flags
    mDoneX = false;
    mDoneY = false;
}

void ParticleSingleLaser::Update(float delta)
{
    const float speed = mSpeed * delta;

    if(!mDoneX)
    {
        mPosXf += mVelX * speed;

        int posX = roundf(mPosXf);

        if((mVelX > 0.f && posX >= mTargetX) || (mVelX < 0.f && posX <= mTargetX) || mVelX == 0.f)
        {
            posX = mTargetX;
            mDoneX = true;
        }

        mRenderable.SetX(posX);
    }

    if(!mDoneY)
    {
        mPosYf += mVelY * speed;

        int posY = roundf(mPosYf);

        if((mVelY > 0.f && posY >= mTargetY) || (mVelY < 0.f && posY <= mTargetY) || mVelY == 0.f)
        {
            posY = mTargetY;
            mDoneY = true;
        }

        mRenderable.SetY(posY);
    }

    // DONE!
    if(mDoneX && mDoneY)
        SetDone();
}

void ParticleSingleLaser::OnDone()
{
    // target is gone
    if(!mGameMap->HasObject(mTarget))
        return ;

    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    if(mDamage > 0.f)
    {
        mTarget->Hit(mDamage, mShooter, mFatal);

        // play hit SFX if not destroyed
        if(!mTarget->IsDestroyed())
            ap->PlaySound("game/hit-01.ogg");
    }
    else
    {
        mTarget->MissHit();

        ap->PlaySound("game/miss-01.ogg");
    }
}

} // namespace game
