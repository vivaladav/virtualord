#include "Indicators/StructureIndicator.h"

#include "GameConstants.h"
#include "Player.h"
#include "GameObjects/ObjectData.h"
#include "Widgets/GameUIData.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Text.h>

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include <iostream>

namespace game
{

const int ALPHA = 115;

StructureIndicator::StructureIndicator(const ObjectData & data, PlayerFaction faction)
    : IsoObject(data.GetRows(), data.GetCols())
    , mFaction(faction)
{
    auto tm = sgl::graphic::TextureManager::Instance();

    SetTexture(tm->GetSprite(data.GetIconTexFile(), data.GetIconTexId(faction)));

    SetAlpha(ALPHA);

    UpdateCostColor();
}

void StructureIndicator::SetCost(float energy, float material)
{
    // update cost value
    mCostEnergy = energy;
    mCostMaterial = material;

    CreateCostData();
}

void StructureIndicator::ShowCost(bool val)
{
    if(val)
    {
        if(nullptr == mTxtCostEnergy)
            CreateCostData();
    }
    else
    {
        delete mTxtCostEnergy;
        mTxtCostEnergy = nullptr;

        delete mIconEnergy;
        mIconEnergy = nullptr;

        delete mTxtCostMaterial;
        mTxtCostMaterial = nullptr;

        delete mIconMaterial;
        mIconMaterial = nullptr;
    }
}

void StructureIndicator::Render()
{
    IsoObject::Render();

    /*
    if(mTxtCostEnergy)
    {
        const int iconMargin = 5;

        const int rowW1 = mTxtCostEnergy->GetWidth() + iconMargin + mIconEnergy->GetWidth();
        const int rowW2 = mTxtCostMaterial->GetWidth() + iconMargin + mIconMaterial->GetWidth();
        const int totH = mTxtCostEnergy->GetHeight() + mTxtCostMaterial->GetHeight();

        // ENERGY
        const int x1 = GetX() + (GetWidth() - rowW1) * 0.5f;
        const int y1 = GetY() + (GetHeight() - totH) * 0.5f;
        mTxtCostEnergy->SetPosition(x1, y1);

        const int icoX1 = x1 + mTxtCostEnergy->GetWidth() + iconMargin;
        const int icoY1 = y1 + (mTxtCostEnergy->GetHeight() - mIconEnergy->GetHeight()) * 0.5f;
        mIconEnergy->SetPosition(icoX1, icoY1);

        // MATERIAL
        const int x2 = GetX() + (GetWidth() - rowW2) * 0.5f;
        const int y2 = y1 + mTxtCostEnergy->GetHeight();
        mTxtCostMaterial->SetPosition(x2, y2);

        const int icoX2 = x2 + mTxtCostMaterial->GetWidth() + iconMargin;
        const int icoY2 = y2 + (mTxtCostMaterial->GetHeight() - mIconMaterial->GetHeight()) * 0.5f;
        mIconMaterial->SetPosition(icoX2, icoY2);

        mTxtCostEnergy->Render();
        mIconEnergy->Render();
        mTxtCostMaterial->Render();
        mIconMaterial->Render();
    }
    */
}

void StructureIndicator::CreateCostData()
{
    using namespace sgl::graphic;

    FontManager * fm = FontManager::Instance();
    Font * font = fm->GetFont("Lato-Bold.ttf", 14, Font::NORMAL);

    auto tm = sgl::graphic::TextureManager::Instance();

    // ENERGY
    delete mTxtCostEnergy;

    std::ostringstream s;
    s << std::fixed << std::setprecision(0) << std::roundf(mCostEnergy);

    mTxtCostEnergy = new Text(s.str().c_str(), font);
    mTxtCostEnergy->SetColor(mColorCost);

    if(nullptr == mIconEnergy)
        mIconEnergy = new Image(tm->GetSprite(SpriteFileMapUI, IND_ICON_CELL_ENERGY));

    // MATERIAL
    delete mTxtCostMaterial;

    s.clear();
    s.str(std::string());
    s << std::fixed << std::setprecision(0) << std::roundf(mCostMaterial);

    mTxtCostMaterial = new Text(s.str().c_str(), font);
    mTxtCostMaterial->SetColor(mColorCost);

    if(nullptr == mIconMaterial)
        mIconMaterial = new Image(tm->GetSprite(SpriteFileMapUI, IND_ICON_CELL_MATERIAL));
}

void StructureIndicator::UpdateCostColor()
{
    if(FACTION_1 == mFaction)
        mColorCost = 0xf0dbdbff;
    else if(FACTION_2 == mFaction)
        mColorCost = 0xd9f2ddff;
    else
        mColorCost = 0xd6edf5ff;

    if(mTxtCostEnergy != nullptr)
    {
        mTxtCostEnergy->SetColor(mColorCost);
        mTxtCostMaterial->SetColor(mColorCost);
    }
}

} // namespace game
