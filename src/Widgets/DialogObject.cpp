#include "Widgets/DialogObject.h"

#include "Game.h"
#include "GameData.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ObjectVisualAttribute.h"
#include "Widgets/ProgressBarObjectVisualStat.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
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

// ====== BUTTON CLOSE =====
class ButtonCloseDO : public sgl::sgui::ImageButton
{
public:
    ButtonCloseDO(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL,
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL,
                                    ID_DLG_OBJ_BTN_CLOSE_OVER,
                                    ID_DLG_OBJ_BTN_CLOSE_PUSHED,
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL
                                 },
                                 SpriteFileDialogObject, parent)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

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
        auto tm = graphic::TextureManager::Instance();
        //auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, textIds[type]);

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
        const unsigned int barLvlTexId = ID_DLG_OBJ_SBAR_1 + val;
        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileDialogObject, barLvlTexId);
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

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonCloseDO(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    const int marginL = 30;

    // TITLE
    const int marginTitleT = 10;

    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 28, sgl::graphic::Font::NORMAL);
    mTitle = new sgui::Label(font, this);
    mTitle->SetColor(WidgetsConstants::colorDialogTitle);
    mTitle->SetPosition(marginL, marginTitleT);

    // IMAGE
    mImg = new sgui::Image(this);

    // VISUAL STATS
    const int statX0 = 355;
    const int statY0 = 95;
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
    const int attX0 = 30;
    const int attY0 = 280;

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

    // IMAGE
    const ObjectData & data = mObjDataReg->GetObjectData(type);
    const unsigned int texInd = data.GetIconTexId(faction, obj);
    tex = tm->GetSprite(data.GetIconTexFile(), texInd);

    mImg->SetTexture(tex);

    const int imgAreaX = 30;
    const int imgAreaY = 70;
    const int imgAreaW = 300;
    const int imgAreaH = 210;
    const int imgW = mImg->GetWidth();
    const int imgH = mImg->GetHeight();
    const int imgX = imgAreaX + (imgAreaW - imgW) / 2;
    const int imgY = imgAreaY + (imgAreaH - imgH) / 2;
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
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0, y0);
}

} // namespace game
