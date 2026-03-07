#include "Widgets/DialogNewMiniUnitsSquad.h"

#include "GameData.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/ButtonDialogAction.h"
#include "Widgets/ButtonDialogArrows.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ObjectVisualAttribute.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/StringManager.h>

#include <cassert>

namespace
{
constexpr int panelPreviewW = 240;
constexpr int panelPreviewH = 200;

constexpr int topPanelX0 = 40;
constexpr int topPanelY0 = 67;
}

namespace game
{

// ===== DIALOG =====
DialogNewMiniUnitsSquad::DialogNewMiniUnitsSquad(GameObject * spawner, Player * player,
                                                 const ObjectsDataRegistry * dataReg)
    : mSpawner(spawner)
    , mPlayer(player)
    , mDataReg(dataReg)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetTexture(SpriteFileDialogNewMiniUnits);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonDialogClose(this);
    mBtnClose->SetX(GetWidth() - mBtnClose->GetWidth());

    // TITLE
    const int marginL = 40;
    const int marginT = 7;

    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 28, graphic::Font::NORMAL);

    auto title = new sgui::Label(sm->GetCString("CREATE_MU_SQUADS"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);
    title->SetPosition(marginL, marginT);

    // PANELS
    CreatePanelPreview();
    CreatePanelDescription();
    CreatePanelAttributes();
    CreatePanelConfig();

    // BUTTON BUILD
    const int btnX = 940;
    const int btnY = 510;
    mBtnBuild = new ButtonDialogAction(sm->GetCString("SPAWN"), "S", core::KeyboardEvent::KEY_S, this);
    mBtnBuild->SetPosition(btnX, btnY);

    UpdateData();
}

GameObjectTypeId DialogNewMiniUnitsSquad::GetTypeToBuild() const
{
    const std::vector<GameObjectTypeId> & mu = mPlayer->GetAvailableMiniUnits();
    return mu[mCurrentIndex];
}

int DialogNewMiniUnitsSquad::GetNumElements() const
{
    return mSliderElements->GetValue();
}

int DialogNewMiniUnitsSquad::GetNumSquads() const
{
    return mSliderSquads->GetValue();
}

void DialogNewMiniUnitsSquad::AddFunctionOnBuild(const std::function<void()> & f)
{
    mBtnBuild->AddOnClickFunction(f);
}

void DialogNewMiniUnitsSquad::AddFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogNewMiniUnitsSquad::CreatePanelPreview()
{
    using namespace sgl;

    // ARROWS
    const int marginButtonLR = 10;

    int x = topPanelX0 + marginButtonLR;
    int y;

    // ARROW LEFT
    mBtnLeft = new ButtonDialogArrowLeft(this);
    // LEFT always disabled when starting as initial index is 0
    mBtnLeft->SetEnabled(false);

    y = topPanelY0 + (panelPreviewH - mBtnLeft->GetHeight()) / 2;
    mBtnLeft->SetPosition(x, y);

    mBtnLeft->AddOnClickFunction([this]
    {
        ChangeIndex(-1);
    });

    // ARROW RIGHT
    mBtnRight = new ButtonDialogArrowRight(this);

    x = topPanelX0 + panelPreviewW - marginButtonLR - mBtnRight->GetWidth();
    mBtnRight->SetPosition(x, y);

    mBtnRight->AddOnClickFunction([this]
    {
        ChangeIndex(1);
    });

    const std::vector<GameObjectTypeId> & mu = mPlayer->GetAvailableMiniUnits();

    assert(!mu.empty());

    mBtnRight->SetEnabled(mu.size() > 1);

    // PREVIEW IMAGE
    mImgPreview = new sgui::Image(this);
}

void DialogNewMiniUnitsSquad::CreatePanelDescription()
{
    using namespace sgl;

    const int panelX0 = 280;
    const int marginL = 20;
    const int marginT = 20;
    const int marginHeaderB = 5;
    const int x0 = panelX0 + marginL;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFilePanelHeader, 20, graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    int x = x0;
    int y = topPanelY0 + marginT;

    // -- DESCRIPTION --
    // HEADER DESCRIPTION
    auto header = new sgui::Label(sm->GetCString("DESCRIPTION"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorPanelHeader);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // TEXT DESCRIPTION
    const int textW = 480;
    const int textH = 70;
    const int marginTextB = 10;

    mDescription = new sgui::TextArea(textW, textH, fontText, false, this);
    mDescription->SetColor(WidgetsConstants::colorPanelText);
    mDescription->SetPosition(x, y);

    y +=  textH + marginTextB;

    // -- COST --
    const int marginHeaderCostB = 10;
    const int marginIconR = 10;
    const int marginIconToNextR = 140;

    // HEADER COST
    header = new sgui::Label(sm->GetCString("UNITARY_COST"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorPanelHeader);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderCostB;

    // ENERGY
    auto tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_ENERGY_24);
    auto icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelCostEnergy = new sgui::Label(fontText, this);
    mLabelCostEnergy->SetColor(WidgetsConstants::colorDialogText);
    mLabelCostEnergy->SetPosition(x, y);

    x = icon->GetX() + marginIconToNextR;

    // MATERIAL
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_MATERIAL_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelCostMaterial = new sgui::Label(fontText, this);
    mLabelCostMaterial->SetColor(WidgetsConstants::colorDialogText);
    mLabelCostMaterial->SetPosition(x, y);

    x = icon->GetX() + marginIconToNextR;

    // DIAMONDS
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_DIAMONDS_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelCostDiamonds = new sgui::Label(fontText, this);
    mLabelCostDiamonds->SetColor(WidgetsConstants::colorDialogText);
    mLabelCostDiamonds->SetPosition(x, y);

    x = icon->GetX() + marginIconToNextR;

    // BLOBS
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_BLOBS_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelCostBlobs = new sgui::Label(fontText, this);
    mLabelCostBlobs->SetColor(WidgetsConstants::colorDialogText);
    mLabelCostBlobs->SetPosition(x, y);
}

void DialogNewMiniUnitsSquad::CreatePanelAttributes()
{
    using namespace sgl;

    const int panelY0 = topPanelY0 + 200;

    const int rows = 6;
    const int cols = 2;

    int x = topPanelX0;
    int y = panelY0;

    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            auto panel = new ObjectVisualAttribute(this);
            panel->SetPosition(x, y);

            mAttributes.emplace_back(panel);

            x += panel->GetWidth();
        }

        x = topPanelX0;
        y += mAttributes[0]->GetHeight();
    }
}

void DialogNewMiniUnitsSquad::CreatePanelConfig()
{
    using namespace sgl;

    const int panelX0 = 840;
    const int marginL = 20;
    const int marginT = 20;
    const int x0 = panelX0 + marginL;
    const int marginHeaderB = 20;
    const int marginBlockB = 60;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFilePanelHeader, 20, graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    auto texSliderBg = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BG);
    auto texSliderBar = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BAR);
    auto texSliderBtn = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BTN);

    int x = x0;
    int y = topPanelY0 + marginT;

    // -- ELEMENTS --
    // HEADER ELEMENTS
    auto header = new sgui::Label(sm->GetCString("ELEMENTS"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorPanelHeader);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // SLIDER ELEMENTS
    const int minElements = 1;
    const int maxElements = 5;
    const int defElements = 2;

    const int marginSliderR = 20;
    const int marginSliderB = 60;

    mSliderElements = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderElements->SetMinMax(minElements, maxElements);
    mSliderElements->SetValue(defElements);
    mSliderElements->SetPosition(x, y);

    x += mSliderElements->GetWidth() + marginSliderR;

    auto label = new sgui::Label(std::to_string(defElements).c_str(), fontText, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderElements->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        UpdateTotalCosts();

        UpdatePreview();
    });

    x = x0;
    y += mSliderElements->GetHeight() + marginBlockB;

    // -- SQUADS --
    // HEADER SQUADS
    header = new sgui::Label(sm->GetCString("SQUADS"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorPanelHeader);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // SLIDER SQUADS
    const int squadLimits[ObjectData::MAX_STAT_VAL + 1] = { 0, 3, 4, 5, 6, 7, 8, 10, 11, 12 };
    const int attSpawning = mSpawner->GetAttribute(OBJ_ATT_SPAWNING);

    const int minSquads = 1;
    const int maxSquads = squadLimits[attSpawning];
    const int defSquads = 2;

    mSliderSquads = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderSquads->SetMinMax(minSquads, maxSquads);
    mSliderSquads->SetValue(defSquads);
    mSliderSquads->SetPosition(x, y);

    x += mSliderSquads->GetWidth() + marginSliderR;

    label = new sgui::Label(std::to_string(defSquads).c_str(), fontText, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderSquads->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        UpdateTotalCosts();
    });

    x = x0;
    y += mSliderSquads->GetHeight() + marginBlockB;

    // -- TOTAL COST --
    const int marginIconR = 10;
    const int marginIconB = 50;
    const int marginIconToNextR = 150;

    // HEADER TOTAL COST
    header = new sgui::Label(sm->GetCString("TOTAL_COST"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorPanelHeader);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // ENERGY
    auto tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_ENERGY_24);
    auto icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelTotCostEnergy = new sgui::Label(fontText, this);
    mLabelTotCostEnergy->SetPosition(x, y);

    x = icon->GetX() + marginIconToNextR;

    // MATERIAL
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_MATERIAL_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelTotCostMaterial = new sgui::Label(fontText, this);
    mLabelTotCostMaterial->SetPosition(x, y);

    x = x0;
    y += marginIconB;

    // DIAMONDS
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_DIAMONDS_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelTotCostDiamonds = new sgui::Label(fontText, this);
    mLabelTotCostDiamonds->SetPosition(x, y);

    x = icon->GetX() + marginIconToNextR;

    // BLOBS
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_W_RES_BLOBS_24);
    icon = new sgui::Image(tex, this);
    icon->SetColor(WidgetsConstants::colorDialogIcon);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    mLabelTotCostBlobs = new sgui::Label(fontText, this);
    mLabelTotCostBlobs->SetPosition(x, y);
}

void DialogNewMiniUnitsSquad::ChangeIndex(int delta)
{
    // can't go further left
    if(delta < 0 && 0 == mCurrentIndex)
        return;

    const std::vector<GameObjectTypeId> & mu = mPlayer->GetAvailableMiniUnits();
    const int unsigned numMiniUnits = mu.size();

    // can't go further right
    if(delta > 0 && (mCurrentIndex + 1) == numMiniUnits)
        return ;

    // update index
    mCurrentIndex += delta;

    // update arrows
    mBtnLeft->SetEnabled(mCurrentIndex > 0);
    mBtnRight->SetEnabled((mCurrentIndex + 1) < numMiniUnits);

    // finalize update
    UpdateData();
}

void DialogNewMiniUnitsSquad::UpdateTotalCosts()
{
    const GameObjectTypeId typeToBuild = GetTypeToBuild();
    const ObjectData & data = mDataReg->GetObjectData(typeToBuild);
    const std::array<int, NUM_OBJ_COSTS> & costs = data.GetCosts();

    const int totElements = mSliderElements->GetValue() * mSliderSquads->GetValue();

    const int totCost[NUM_OBJ_COSTS] =
    {
        costs[OBJ_COST_ENERGY] * totElements,
        costs[OBJ_COST_MATERIAL] * totElements,
        costs[OBJ_COST_DIAMONDS] * totElements,
        costs[OBJ_COST_BLOBS] * totElements
    };

    const bool canSpend[NUM_OBJ_COSTS] =
    {
        mPlayer->HasEnough(Player::ENERGY,  totCost[OBJ_COST_ENERGY]),
        mPlayer->HasEnough(Player::MATERIAL, totCost[OBJ_COST_MATERIAL]),
        mPlayer->HasEnough(Player::DIAMONDS, totCost[OBJ_COST_DIAMONDS]),
        mPlayer->HasEnough(Player::BLOBS, totCost[OBJ_COST_BLOBS])
    };

    const unsigned int colors[] =
    {
        WidgetsConstants::colorDialogBad,
        WidgetsConstants::colorDialogGood
    };

    // ENERGY
    mLabelTotCostEnergy->SetText(std::to_string(totCost[OBJ_COST_ENERGY]).c_str());
    mLabelTotCostEnergy->SetColor(colors[canSpend[OBJ_COST_ENERGY]]);

    // MATERIAL
    mLabelTotCostMaterial->SetText(std::to_string(totCost[OBJ_COST_MATERIAL]).c_str());
    mLabelTotCostMaterial->SetColor(colors[canSpend[OBJ_COST_MATERIAL]]);

    // DIAMONDS
    mLabelTotCostDiamonds->SetText(std::to_string(totCost[OBJ_COST_DIAMONDS]).c_str());
    mLabelTotCostDiamonds->SetColor(colors[canSpend[OBJ_COST_DIAMONDS]]);

    // BLOBS
    mLabelTotCostBlobs->SetText(std::to_string(totCost[OBJ_COST_BLOBS]).c_str());
    mLabelTotCostBlobs->SetColor(colors[canSpend[OBJ_COST_BLOBS]]);

    // BUTTON BUILD
    mBtnBuild->SetEnabled(canSpend[OBJ_COST_ENERGY] && canSpend[OBJ_COST_MATERIAL] &&
                          canSpend[OBJ_COST_DIAMONDS] && canSpend[OBJ_COST_BLOBS]);
}

void DialogNewMiniUnitsSquad::UpdatePreview()
{
    using namespace sgl;

    const GameObjectTypeId typeToBuild = GetTypeToBuild();
    const ObjectData & data = mDataReg->GetObjectData(typeToBuild);

    auto tm = graphic::TextureManager::Instance();

    const PlayerFaction faction = mPlayer->GetFaction();
    const unsigned int texInd0 = data.GetIconTexId(faction);
    const unsigned int texInd = texInd0 + NUM_MUNIT_SPRITES_PER_SQUAD * (GetNumElements() - 1);

    auto tex = tm->GetSprite(data.GetIconTexFile(), texInd);
    mImgPreview->SetTexture(tex);

    const int previewX = topPanelX0 + (panelPreviewW - mImgPreview->GetWidth()) / 2;
    const int previewY = topPanelY0 + (panelPreviewH - mImgPreview->GetHeight()) / 2;
    mImgPreview->SetPosition(previewX, previewY);
}

void DialogNewMiniUnitsSquad::UpdateData()
{
    const GameObjectTypeId typeToBuild = GetTypeToBuild();
    const ObjectData & data = mDataReg->GetObjectData(typeToBuild);

    // PREVIEW
    UpdatePreview();

    // DESCRIPTION
    mDescription->SetText(ObjectData::DESCRIPTIONS.at(typeToBuild).c_str());

    // COSTS
    const std::array<int, NUM_OBJ_COSTS> & costs = data.GetCosts();

    mLabelCostEnergy->SetText(std::to_string(costs[OBJ_COST_ENERGY]).c_str());
    mLabelCostMaterial->SetText(std::to_string(costs[OBJ_COST_MATERIAL]).c_str());
    mLabelCostDiamonds->SetText(std::to_string(costs[OBJ_COST_DIAMONDS]).c_str());
    mLabelCostBlobs->SetText(std::to_string(costs[OBJ_COST_BLOBS]).c_str());

    // ATTRIBUTES
    const unsigned int numPanelAttributes = mAttributes.size();

    auto sm = sgl::utilities::StringManager::Instance();

    unsigned int attsAdded = 0;

    for(unsigned int i = 0; i < NUM_OBJ_ATTRIBUTES; ++i)
    {
        const int val = data.GetAttribute(static_cast<ObjAttId>(i));

        if(val > 0)
        {
            mAttributes[attsAdded]->SetData(sm->GetCString(ObjectData::STR_ATTRIBUTES[i]), val);
            mAttributes[attsAdded]->SetTooltipData(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[i]));

            ++attsAdded;
        }

        // it shouldn't happen, but just in case
        if(numPanelAttributes == attsAdded)
            break;
    }

    // WEAPON ATTRIBUTES
    const WeaponType wt = data.GetWeapon();

    if(wt != WeaponData::TYPE_NULL)
    {
        const WeaponData & wData = mDataReg->GetWeaponData(wt);
        const std::unordered_map<ObjAttId, int> &  wAttributes = wData.GetAttributes();

        for(unsigned int i = 0; i < NUM_WEAPON_ATTRIBUTES; ++i)
        {
            const auto attId = static_cast<ObjAttId>(FIRST_WEAPON_ATTRIBUTE + i);

            const int val = wAttributes.at(attId);

            if(val > 0)
            {
                mAttributes[attsAdded]->SetData(sm->GetCString(ObjectData::STR_ATTRIBUTES[attId]), val);
                mAttributes[attsAdded]->SetTooltipData(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[attId]));

                ++attsAdded;
            }
        }
    }

    // clear remaining slots
    for(unsigned int i = attsAdded; i < numPanelAttributes; ++i)
        mAttributes[i]->ClearData();

    // TOTAL COSTS
    UpdateTotalCosts();
}

void DialogNewMiniUnitsSquad::HandlePositionChanged()
{
    SetPositions();
}

void DialogNewMiniUnitsSquad::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mBg->SetPosition(x0, y0);
}

} // namespace game
