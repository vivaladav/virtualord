#include "Widgets/DialogObject.h"

#include "Game.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ProgressBarObjectVisualStat.h"

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
#include <sgl/utilities/System.h>

#include <cmath>
#include <sstream>

namespace game
{

// ====== BUTTON CLOSE =====
class ButtonClose : public sgl::sgui::ImageButton
{
public:
    ButtonClose(sgl::sgui::Widget * parent)
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

        // HEADER
        const char * HEADERS[NUM_VISUAL_STAT_TYPES] =
        {
            "ENERGY",
            "HEALTH",
            "EXPERIENCE"
        };

        auto fm = graphic::FontManager::Instance();
        auto tm = graphic::TextureManager::Instance();
        //auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, textIds[type]);

        const char * fileFontHeader = "Lato-Regular.ttf";
        const unsigned int colorHeader = 0xb3d4e5ff;
        const int sizeHeader = 18;

        graphic::Font * fontHeader = fm->GetFont(fileFontHeader, sizeHeader, graphic::Font::NORMAL);
        mHeader = new sgui::Label(HEADERS[type], fontHeader, this);
        mHeader->SetColor(colorHeader);

        // DATA
        const char * fileFontData = "Lato-Regular.ttf";
        const unsigned int colorData = 0x70a7c2ff;
        const int sizeData = 18;

        graphic::Font * fontData = fm->GetFont(fileFontData, sizeData, graphic::Font::NORMAL);
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

        const char * fileFontHeader = "Lato-Regular.ttf";
        const unsigned int colorHeader = 0xb3d4e5ff;
        const int sizeHeader = 18;

        graphic::Font * fontHeader = fm->GetFont(fileFontHeader, sizeHeader, graphic::Font::NORMAL);
        mHeader = new sgui::Label(strHeader, fontHeader, this);
        mHeader->SetColor(colorHeader);

        // DATA
        const char * fileFontData = "Lato-Regular.ttf";
        const unsigned int colorData = 0x70a7c2ff;
        const int sizeData = 18;

        graphic::Font * fontData = fm->GetFont(fileFontData, sizeData, graphic::Font::NORMAL);
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
        const unsigned int barLvlTexId = ID_DLG_OBJ_SBAR_0 + val;
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

// ===== DIALOG =====
DialogObject::DialogObject(const ObjectsDataRegistry * odr)
    : mObjDataReg(odr)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    const int marginL = 30;

    // TITLE
    const unsigned int colorTitle = 0xf0f3f5ff;
    const int marginTitleT = 14;

    auto font = fm->GetFont("Lato-Regular.ttf", 28, sgl::graphic::Font::NORMAL);
    mTitle = new sgui::Label(font, this);
    mTitle->SetColor(colorTitle);
    mTitle->SetPosition(marginL, marginTitleT);

    // IMAGE
    mImg = new sgui::Image(this);

    // VISUAL STATS
    const int statX0 = 355;
    const int statY0 = 95;
    const int statBlockH = 50;

    int statY = statY0;
    mStatRank = new ObjectExtendedVisualRank("RANK", this);
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

    auto tm = graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = nullptr;

    // TITLE
    mTitle->SetText(GameObject::TITLES.at(type).c_str());

    // IMAGE
    if(faction == NO_FACTION)
    {
        const ObjectBasicData & data = mObjDataReg->GetObjectData(type);
        tex = tm->GetSprite(data.noFactionIconFile, data.noFactionIconTexId);
    }
    else
    {
        const ObjectFactionData & data = mObjDataReg->GetFactionData(faction, type);
        tex = tm->GetSprite(data.iconFile, data.iconTexId);
    }

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
    mStatRank->SetValue(obj->GetExperienceLevel(), obj->GetMaxExperienceLevel());
    mStatExperience->SetValue(obj->GetExperience(), obj->GetExperienceToNextLevel());
    mStatEnergy->SetValue(obj->GetEnergy(), obj->GetMaxEnergy());
    mStatHealth->SetValue(obj->GetHealth(), obj->GetMaxHealth());
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
