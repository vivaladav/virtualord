#include "Widgets/DialogObject.h"

#include "Game.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ObjectVisualAttribute.h"
#include "Widgets/ProgressBarObjectVisualStat.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>
#include <sgl/utilities/System.h>

#include <cmath>
#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ========== OBJECT STAT ==========
class ObjectExtendedVisualStat : public sgl::sgui::Widget
{
public:
    enum VisualStatType : unsigned int
    {
        VST_ENERGY,
        VST_HEALTH,
        VST_EXPERIENCE,

        NUM_VISUAL_STAT_TYPES
    };

public:
    ObjectExtendedVisualStat(VisualStatType type, sgl::sgui::Widget * parent)
        : sgl::sgui::Widget(parent)
    {
        using namespace sgl;

        auto sm = utilities::StringManager::Instance();

        // HEADER
        const char * HEADERS[NUM_VISUAL_STAT_TYPES] =
        {
            sm->GetCString("ENERGY"),
            sm->GetCString("HEALTH"),
            sm->GetCString("EXPERIENCE"),
        };

        auto fm = graphic::FontManager::Instance();

        const unsigned int colorHeader = 0xb3d4e5ff;
        const int sizeHeader = 18;

        auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader, graphic::Font::NORMAL);
        mHeader = new sgui::Label(HEADERS[type], fontHeader, this);
        mHeader->SetColor(colorHeader);

        // DATA
        const unsigned int colorData = 0x70a7c2ff;
        const int sizeData = 18;

        auto fontData = fm->GetFont(WidgetsConstants::FontFileText, sizeData, graphic::Font::NORMAL);
        mData = new sgui::Label(fontData, this);
        mData->SetColor(colorData);

        // BAR
        const float minBar = 0.f;
        const float maxBar = 100.f;
        mBar = new ProgressBarObjectVisualStat(minBar, maxBar, this);

        // SIZE
        const int w = 450;
        const int h = mHeader->GetHeight();
        SetSize(w, h);

        // POSITION ELEMENTS
        mBar->SetPosition(w - mBar->GetWidth(), (h - mBar->GetHeight()) / 2);
   }

    void SetValue(float val, float max)
    {
        // DATA
        std::ostringstream ss;
        ss << static_cast<int>(std::roundf(val)) << " / " << static_cast<int>(std::roundf(max));
        mData->SetText(ss.str().c_str());

        const int marginData = 20;
        const int dataX = mBar->GetX() - marginData - mData->GetWidth();
        const int dataY = (GetHeight() - mData->GetHeight()) / 2;
        mData->SetPosition(dataX, dataY);

        // BAR
        const float minBar = 0.f;
        mBar->SetMinMax(minBar, max);
        mBar->SetValue(val);
    }

private:
    sgl::sgui::Label * mHeader = nullptr;
    sgl::sgui::Label * mData = nullptr;
    ProgressBarObjectVisualStat * mBar = nullptr;
};

class ObjectExtendedVisualRank : public sgl::sgui::Widget
{
public:
    ObjectExtendedVisualRank(const char * strHeader, sgl::sgui::Widget * parent)
        : sgl::sgui::Widget(parent)
    {
        using namespace sgl;

        // HEADER
        auto fm = graphic::FontManager::Instance();

        const unsigned int colorHeader = 0xb3d4e5ff;
        const int sizeHeader = 18;

        auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader, graphic::Font::NORMAL);
        mHeader = new sgui::Label(strHeader, fontHeader, this);
        mHeader->SetColor(colorHeader);

        // DATA
        const unsigned int colorData = 0x70a7c2ff;
        const int sizeData = 18;

        auto fontData = fm->GetFont(WidgetsConstants::FontFileText, sizeData, graphic::Font::NORMAL);
        mData = new sgui::Label(fontData, this);
        mData->SetColor(colorData);

        // LEVEL
        mBar = new sgui::Image(this);

        // SIZE
        const int w = 450;
        const int h = mHeader->GetHeight();
        SetSize(w, h);
    }

    void SetValue(int val, int max)
    {
        // DATA
        std::ostringstream ss;
        ss << (val + 1) << " / " << max;
        mData->SetText(ss.str().c_str());

        // BAR
        const unsigned int barLvlTexId = ID_STARS_BAR_1 + val;
        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileUIShared, barLvlTexId);
        mBar->SetTexture(tex);

        // POSITION ELEMENTS
        const int w = GetWidth();
        const int h = GetHeight();

        mBar->SetPosition(w - mBar->GetWidth(), (h - mBar->GetHeight()) / 2);

        const int marginData = 62;
        const int dataX = mBar->GetX() - marginData - mData->GetWidth();
        const int dataY = (GetHeight() - mData->GetHeight()) / 2;
        mData->SetPosition(dataX, dataY);
    }

private:
    sgl::sgui::Label * mHeader = nullptr;
    sgl::sgui::Label * mData = nullptr;
    sgl::sgui::Image * mBar = nullptr;
};

// ========== DIALOG PANEL ==========
class PanelDialogObject : public sgl::sgui::Widget
{
public:
    PanelDialogObject(int w, sgl::sgui::Widget * parent)
        : sgl::sgui::Widget(parent)
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();
        graphic::Texture * tex;

        tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_PANEL_BG_L);
        mBgL = new graphic::Image(tex);
        RegisterRenderable(mBgL);

        const int wL = mBgL->GetWidth();
        const int h = mBgL->GetHeight();

        tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_PANEL_BG_R);
        mBgR = new graphic::Image(tex);
        RegisterRenderable(mBgR);

        const int wR = mBgR->GetWidth();

        tex = tm->GetSprite(SpriteFileDialogObjectExp, ID_DLG_OBJ_PANEL_BG_C);
        tex->SetScaleMode(graphic::TSCALE_NEAREST);
        mBgC = new graphic::Image(tex);
        RegisterRenderable(mBgC);

        const int wC = w - wL - wR;
        mBgC->SetWidth(wC);

        SetSize(w, h);
    }

    void HandlePositionChanged() override
    {
        const int y = GetScreenY();
        int x = GetScreenX();

        // BACKGROUND
        mBgL->SetPosition(x, y);
        x += mBgL->GetWidth();

        mBgC->SetPosition(x, y);
        x += mBgC->GetWidth();

        mBgR->SetPosition(x, y);
    }

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;
};

} // namespace

namespace game
{

// ===== DIALOG =====
DialogObject::DialogObject(const ObjectsDataRegistry * odr)
    : mObjDataReg(odr)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // -- BACKGROUND --
    const int w = 880;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetSprite(SpriteFileDialogObjectExp, ID_DLG_OBJ_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                            WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    mTitle = new sgui::Label(font, this);
    mTitle->SetColor(WidgetsConstants::colorDialogTitle);

    // PANEL IMAGE
    const int panelImgX = WidgetsConstants::MarginDialogContentL;
    const int panelImgY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
    const int panelImgW = 300;

    mPanelImg = new PanelDialogObject(panelImgW, this);
    mPanelImg->SetPosition(panelImgX, panelImgY);

    // PANEL STATS
    const int panelStatsX = panelImgX + panelImgW;
    const int panelStatsY = panelImgY;
    const int panelStatsW = 500;
    const int marginStatsL = 25;
    const int marginStatsT = 20;

    mPanelStats = new PanelDialogObject(panelStatsW, this);
    mPanelStats->SetPosition(panelStatsX, panelStatsY);

    // IMAGE
    mImg = new sgui::Image(this);

    // VISUAL STATS
    const int statX0 = panelStatsX + marginStatsL;
    const int statY0 = panelStatsY + marginStatsT;
    const int statBlockH = 50;

    int statY = statY0;
    mStatRank = new ObjectExtendedVisualRank(sm->GetCString("LEVEL"), this);
    mStatRank->SetPosition(statX0, statY);

    statY += statBlockH;

    mStatExperience = new ObjectExtendedVisualStat(ObjectExtendedVisualStat::VST_EXPERIENCE, this);
    mStatExperience->SetPosition(statX0, statY);

    statY += statBlockH;

    mStatEnergy = new ObjectExtendedVisualStat(ObjectExtendedVisualStat::VST_ENERGY, this);
    mStatEnergy->SetPosition(statX0, statY);

    statY += statBlockH;

    mStatHealth = new ObjectExtendedVisualStat(ObjectExtendedVisualStat::VST_HEALTH, this);
    mStatHealth->SetPosition(statX0, statY);

    // ATTRIBUTES
    const int attX0 = WidgetsConstants::MarginDialogContentL;
    const int attY0 = panelImgY + mPanelImg->GetHeight();

    int attX = attX0;
    int attY = attY0;
    int ind = 0;

    for(int r = 0; r < VIS_ATT_ROWS; ++r)
    {
        for(int c = 0; c < VIS_ATT_COLS; ++c)
        {
            mVisAtt[ind] = new ObjectVisualAttribute(this);
            mVisAtt[ind]->SetPosition(attX, attY);
            ++ind;

            attX += mVisAtt[0]->GetWidth();
        }

        attX = attX0;
        attY += mVisAtt[0]->GetHeight();
    }
}

void DialogObject::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogObject::SetObject(GameObject * obj)
{
    using namespace sgl;

    // UPDATE PANEL ELEMENTS
    const PlayerFaction faction = obj->GetFaction();
    const GameObjectTypeId type = obj->GetObjectType();

    auto sm = utilities::StringManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = nullptr;

    // TITLE
    mTitle->SetText(sm->GetCString(ObjectData::TITLES.at(type)));

    const int titleY = (WidgetsConstants::DialogTitleBarH - mTitle->GetHeight()) / 2;
    mTitle->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    // IMAGE
    const ObjectData & data = mObjDataReg->GetObjectData(type);
    const unsigned int texInd = data.GetIconTexId(faction, obj);
    tex = tm->GetSprite(data.GetIconTexFile(), texInd);

    mImg->SetTexture(tex);

    const int imgW = mImg->GetWidth();
    const int imgH = mImg->GetHeight();
    const int imgX = mPanelImg->GetX() + (mPanelImg->GetWidth() - imgW) / 2;
    const int imgY = mPanelImg->GetY() + (mPanelImg->GetHeight() - imgH) / 2;
    mImg->SetPosition(imgX, imgY);

    // VISUAL STATS
    static_cast<ObjectExtendedVisualRank *>(mStatRank)->SetValue(obj->GetExperienceLevel(),
                                                                 GameObject::MAX_LEVEL);
    static_cast<ObjectExtendedVisualStat *>(mStatExperience)->SetValue(obj->GetExperience(),
                                                                       obj->GetExperienceToNextLevel());
    static_cast<ObjectExtendedVisualStat *>(mStatEnergy)->SetValue(obj->GetEnergy(),
                                                                   obj->GetMaxEnergy());
    static_cast<ObjectExtendedVisualStat *>(mStatHealth)->SetValue(obj->GetHealth(),
                                                                   obj->GetMaxHealth());

    // ATTRIBUTES
    int attsAdded = 0;

    for(unsigned int i = 0; i < NUM_OBJ_ATTRIBUTES; ++i)
    {
        const int val = obj->GetAttribute(static_cast<ObjAttId>(i));

        if(val > 0)
        {
            mVisAtt[attsAdded]->SetData(sm->GetCString(ObjectData::STR_ATTRIBUTES[i]), val);
            mVisAtt[attsAdded]->SetTooltipData(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[i]));

            ++attsAdded;
        }
    }

    for(int i = attsAdded; i < NUM_VIS_ATT; ++i)
        mVisAtt[i]->ClearData();
}

void DialogObject::HandlePositionChanged()
{
    SetPositions();
}

void DialogObject::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    // BACKGROUND
    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

} // namespace game
