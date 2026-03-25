#include "Particles/ParticleOutput.h"

#include "GameData.h"
#include "Particles/DataParticleOutput.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TexturedRenderable.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

ParticleOutput::ParticleOutput()
    : mIcon(new sgl::graphic::TexturedRenderable)
{
}

ParticleOutput::~ParticleOutput()
{
    delete mIcon;
    delete mTxt;
}

void ParticleOutput::SetData(const DataParticleOutput & data)
{
    using namespace sgl;

    // init data
    mSpeed = -data.speed;
    mDecaySpeed = data.decaySpeed;
    mTimerLife = data.timeLife;

    const float maxAlpha = 255.f;
    mAlpha = maxAlpha;

    // clear visuals
    delete mTxt;

    // type dependent data
    unsigned int iconTexId = 0;
    unsigned int color = 0;

    switch(data.type)
    {
        case OT_BLOBS:
            iconTexId = ID_PART_ICON_BLOBS;
            color = 0xe481e4ff;
        break;

        case OT_DIAMONDS:
            iconTexId = ID_PART_ICON_DIAMONDS;
            color = 0xe4e481ff;
        break;

        case OT_ENERGY:
            iconTexId = ID_PART_ICON_ENERGY;
            color = 0xe4cb81ff;
        break;

        case OT_MATERIAL:
            iconTexId = ID_PART_ICON_MATERIAL;
            color = 0xa3c2bfff;
        break;

        case OT_MONEY:
            iconTexId = ID_PART_ICON_MONEY;
            color = 0x81e481ff;
        break;

        case OT_RESEARCH:
            iconTexId = ID_PART_ICON_RESEARCH;
            color = 0x9eccfaff;
        break;

        default: break;
    }

    // ICON
    auto tex = sgl::graphic::TextureManager::Instance()->GetSprite(SpriteFileParticles, iconTexId);
    mIcon->SetTexture(tex);
    mIcon->SetColor(color);

    // TEXT
    const unsigned int colorText = 0xeff0f5ff;
    const int fontSize = 20;
    auto fm = graphic::FontManager::Instance();
    graphic::Font * font = fm->GetFont("Lato-Bold.ttf", fontSize, graphic::Font::NORMAL);

    mTxt = new graphic::Text(std::to_string(data.quantity).c_str(), font);
    mTxt->SetColor(colorText);

    SetStart(data.x0, data.y0);
}

void ParticleOutput::SetStart(int x0, int y0)
{
    mPosXf = x0;
    mPosYf = y0;

    const int margin = 5;
    const int totW = mIcon->GetWidth() + margin + mTxt->GetWidth();
    const int halfW = totW / 2;

    // text
    const int txtX = x0 - halfW;
    const int txtY = y0;
    mTxt->SetPosition(txtX, txtY);

    // icon
    const int iconX = txtX + mTxt->GetWidth() + margin;
    const int iconY = y0;
    mIcon->SetPosition(iconX, iconY);
}

void ParticleOutput::Update(float delta)
{
    // UPDATE LIFE
    mTimerLife -= delta;

    if(mTimerLife < 0.f)
    {
        SetDone();
        return ;
    }

    // UPDATE PARTICLE
    const float speed = mSpeed * delta;
    const float alphaDecay = mDecaySpeed * delta;

    // move graphics
    mPosYf += speed;
    int posY = roundf(mPosYf);

    mIcon->SetY(posY);
    mTxt->SetY(posY);

    // update alpha
    const int minAlpha = 1.f;

    mAlpha -= alphaDecay;
    const int alpha = static_cast<unsigned char>(mAlpha);

    mIcon->SetAlpha(alpha);
    mTxt->SetAlpha(alpha);

    // alpha is too low -> DONE
    if(mAlpha < minAlpha)
         SetDone();
}

void ParticleOutput::Render()
{
    mIcon->Render();
    mTxt->Render();
}

} // namespace game
