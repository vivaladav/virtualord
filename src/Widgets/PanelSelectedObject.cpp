#include "Widgets/PanelSelectedObject.h"

#include "GameUIData.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/DigitsDisplay.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/ProgressBarObjectVisualStat.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/TextArea.h>

#include <cmath>

namespace game
{

// ========== BUTTON CLOSE ==========
class ButtonClosePanel : public sgl::sgui::ImageButton
{
public:
    ButtonClosePanel(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                  ID_PAN_SELOBJ_BTN_CLOSE_NORMAL,
                                  ID_PAN_SELOBJ_BTN_CLOSE_NORMAL,
                                  ID_PAN_SELOBJ_BTN_CLOSE_OVER,
                                  ID_PAN_SELOBJ_BTN_CLOSE_PUSHED,
                                  ID_PAN_SELOBJ_BTN_CLOSE_NORMAL
                                 }, SpriteFilePanelSelectedObject, parent)
    {
    }

    void HandleMouseOver() override
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-03.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/dialog_close-02.ogg");
    }
};

// ========== OBJECT STAT ==========
class ObjectVisualStat : public sgl::sgui::Widget
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
    ObjectVisualStat(VisualStatType type, sgl::sgui::Widget * parent)
        : sgl::sgui::Widget(parent)
    {
        using namespace sgl;

        // ICON
        const unsigned int textIds[NUM_VISUAL_STAT_TYPES] =
        {
            ID_PAN_SELOBJ_ICON_ENERGY,
            ID_PAN_SELOBJ_ICON_HEALTH,
            ID_PAN_SELOBJ_ICON_EXPERIENCE
        };

        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, textIds[type]);

        // ICON
        const unsigned int colorIcon = 0x85a1adff;
        mIcon = new sgui::Image(tex, this);
        mIcon->SetColor(colorIcon);

        // BAR
        const float minBar = 0.f;
        const float maxBar = 100.f;
        mBar = new ProgressBarObjectVisualStat(minBar, maxBar, this);

        // DIGITS
        const int sizeFont = 16;
        const unsigned int colorDigits = 0x70a9c2ff;
        const unsigned int colorZeros = 0x35677dff;
        mDigits = new DigitsDisplay(3, sizeFont, "%", this);
        mDigits->SetColorDigits(colorDigits);
        mDigits->SetColorZeros(colorZeros);

        // SIZE
        const int w = 260;
        const int h = mIcon->GetHeight();
        SetSize(w, h);

        const int barX = (w - mBar->GetWidth()) / 2;
        mBar->SetPosition(barX, (h - mBar->GetHeight()) / 2);

        mDigits->SetPosition(w - mDigits->GetWidth(), (h - mDigits->GetHeight()) / 2);

        // TOOLTIP
        const char * tooltipText[NUM_VISUAL_STAT_TYPES] =
        {
            "Energy",
            "Health",
            "Experience"
        };

        const int ttDelay = 500;
        auto tt = new GameSimpleTooltip(tooltipText[type]);
        SetTooltip(tt);
        SetTooltipDelay(ttDelay);
    }

    void SetValue(float val, float max)
    {
        auto tm = sgl::graphic::TextureManager::Instance();

        const int perc = static_cast<int>(std::roundf(val * 100.f / max));

        // BAR
        mBar->SetValue(perc);

        // ICON
        const unsigned int colorIcon = 0x85a1adff;
        mIcon->SetColor(colorIcon);

        // DIGITS
        mDigits->SetValue(perc);
    }

private:
    sgl::sgui::Image * mIcon = nullptr;
    ProgressBarObjectVisualStat * mBar = nullptr;
    DigitsDisplay * mDigits = nullptr;
};

// ========== BUTTON OBJECT FUNCTION ==========
class ButtonObjectFunction : public sgl::sgui::ImageButton
{
public:
    enum ObjFunction : unsigned int
    {
        OBJFUN_AUTO_ATTACK,
        OBJFUN_AUTO_MOVE,
        OBJFUN_SHOW_INFO,
        OBJFUN_SHOW_UPGRADE,

        NUM_OBJECT_FUNCTIONS
    };

public:
    ButtonObjectFunction(ObjFunction f, sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_PAN_SELOBJ_BTN_ACTION_NORMAL,
                                     ID_PAN_SELOBJ_BTN_ACTION_DISABLED,
                                     ID_PAN_SELOBJ_BTN_ACTION_OVER,
                                     ID_PAN_SELOBJ_BTN_ACTION_PUSHED,
                                     ID_PAN_SELOBJ_BTN_ACTION_CHECKED
                                 }, SpriteFilePanelSelectedObject, parent)
    {
        using namespace sgl;

        SetCheckable(OBJFUN_AUTO_ATTACK == f || OBJFUN_AUTO_MOVE == f);

        // ICON
        const unsigned int textIds[NUM_OBJECT_FUNCTIONS] =
        {
            ID_PAN_SELOBJ_ICON_ACT_ATTACK,
            ID_PAN_SELOBJ_ICON_ACT_MOVE,
            ID_PAN_SELOBJ_ICON_ACT_INFO,
            ID_PAN_SELOBJ_ICON_ACT_UPGRADE,
        };

        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, textIds[f]);

        mIcon = new sgui::Image(tex, this);
        mIcon->SetPosition((GetWidth() - mIcon->GetWidth()) / 2,
                          (GetHeight() - mIcon->GetHeight()) / 2);

        // TOOLTIP
        const char * tooltipText[NUM_OBJECT_FUNCTIONS] =
            {
                "If checked enables automatic attack when spotting an enemy during their turn",
                "If checked enables automatic move when spotting an enemy during their turn",
                "Show the info panel of this object",
                "Show the upgrade panel of this object"
            };

        const int ttDelay = 500;
        const int ttTime = 3500;
        auto tt = new GameSimpleTooltip(tooltipText[f]);
        SetTooltip(tt);
        SetTooltipDelay(ttDelay);
        SetTooltipShowingTime(ttTime);
    }

private:
    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        sgl::sgui::ImageButton::OnStateChanged(state);

        const unsigned int colors[sgl::sgui::AbstractButton::NUM_VISUAL_STATES] =
        {
            0xe0e7eaff,
            0x737373ff,
            0xf0f3f5ff,
            0xd1dce0ff,
            0xf5e1a3ff,
        };

        mIcon->SetColor(colors[state]);
    }

    void HandleMouseOver() override
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

private:
    sgl::sgui::Image * mIcon = nullptr;
};

// ========== PANEL ==========

PanelSelectedObject::PanelSelectedObject(const ObjectsDataRegistry * odr, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mObjDataReg(odr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();

    // BACKGROUND
    auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, ID_PAN_SELOBJ_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // -- ELEMENTS --
    const int contentW = 300;
    const int contentH = 300;
    const int contentX0 = 20;
    const int contentY0 = 20;

    // BUTTON CLOSE
    const int btnCloseX = 5;
    const int btnCloseY = 5;
    mButtonClose = new ButtonClosePanel(this);
    mButtonClose->SetPosition(btnCloseX, btnCloseY);

    // IMAGE
    mImg = new graphic::Image;
    RegisterRenderable(mImg);

    // TITLE
    const int titleW = 130;
    const int titleH = 0;
    const int titleX = 180;
    const int titleY = 30;

    const char * fileFontTile = "Lato-Bold.ttf";
    const unsigned int colorTitle = 0xe9f7fbcc;
    const int sizeTitle = 16;

    graphic::Font * fntTitle = fm->GetFont(fileFontTile, sizeTitle, graphic::Font::NORMAL);

    mTitle = new sgui::TextArea(titleW, titleH, fntTitle, true, this);
    mTitle->SetColor(colorTitle);
    mTitle->setTextAlignmentHorizontal(sgui::TextArea::ALIGN_H_CENTER);
    mTitle->SetPosition(titleX, titleY);

    // LEVEL
    mBarLvl = new sgui::Image(this);

    const int ttDelay = 500;
    auto tt = new GameSimpleTooltip("Experience level");
    mBarLvl->SetTooltip(tt);
    mBarLvl->SetTooltipDelay(ttDelay);

    // STATS
    mStatEnergy = new ObjectVisualStat(ObjectVisualStat::VST_ENERGY, this);
    mStatHealth = new ObjectVisualStat(ObjectVisualStat::VST_HEALTH, this);
    mStatExperience = new ObjectVisualStat(ObjectVisualStat::VST_EXPERIENCE, this);

    const int statX = contentX0 + (contentW - mStatEnergy->GetWidth()) / 2;
    const int statY0 = 155;
    const int marginStatsV = 20;

    int statY = statY0;
    mStatEnergy->SetPosition(statX, statY);

    statY += mStatEnergy->GetHeight() + marginStatsV;
    mStatHealth->SetPosition(statX, statY);

    statY += mStatHealth->GetHeight() + marginStatsV;
    mStatExperience->SetPosition(statX, statY);

    // BUTTONS FUNCTION
    mButtonAutoAttack = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_AUTO_ATTACK, this);
    mButtonAutoAttack->AddOnToggleFunction([this](bool checked)
    {
        // TODO
    });

    mButtonAutoMove = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_AUTO_MOVE, this);
    mButtonAutoMove->AddOnToggleFunction([this](bool checked)
    {
        // TODO
    });

    mButtonShowInfo = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_SHOW_INFO, this);
    mButtonShowInfo->AddOnClickFunction([this]
    {
        // TODO
    });

    mButtonShowUpgrade = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_SHOW_UPGRADE, this);
    mButtonShowUpgrade->AddOnClickFunction([this]
    {
        // TODO
    });

    const int marginButtonFun = 5;
    const int buttonFunX0 = contentX0 + marginButtonFun;
    const int buttonFunY0 = contentY0 + contentH - mButtonAutoAttack->GetHeight() - marginButtonFun;

    int buttonFunX = buttonFunX0;
    int buttonFunY = buttonFunY0;
    mButtonAutoAttack->SetPosition(buttonFunX, buttonFunY);

    buttonFunX += mButtonAutoAttack->GetWidth();
    mButtonAutoMove->SetPosition(buttonFunX, buttonFunY);

    buttonFunX = contentX0 + contentW - mButtonShowInfo->GetWidth() - marginButtonFun;
    mButtonShowInfo->SetPosition(buttonFunX, buttonFunY);
    mButtonShowUpgrade->SetPosition(buttonFunX, buttonFunY);
}

void PanelSelectedObject::AddFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void PanelSelectedObject::AddFunctionOnShowInfo(const std::function<void()> & f)
{
    mButtonShowInfo->AddOnClickFunction(f);
}

void PanelSelectedObject::SetObject(GameObject * obj)
{
    using namespace sgl;

    // object already set -> exit
    if(obj == mObj)
        return ;

    // UPDATE SELECTED OBJECT
    if(mObj != nullptr)
        mObj->RemoveFunctionOnValueChanged(mFuncValuesChangedId);

    mObj = obj;

    mFuncValuesChangedId = mObj->AddFunctionOnValueChanged([this]
    {
        UpdateStats();
    });

    // UPDATE PANEL ELEMENTS
    const PlayerFaction faction = obj->GetFaction();
    const GameObjectTypeId type = obj->GetObjectType();

    auto tm = graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = nullptr;

    // TITLE
    mTitle->SetText(GameObject::TITLES.at(type).c_str());

    // BAR LEVEL
    const int maxLvl = 10;

    const unsigned int barLvlTexId = ID_PAN_SELOBJ_SBAR_0 + obj->GetExperienceLevel();
    tex = tm->GetSprite(SpriteFilePanelSelectedObject, barLvlTexId);
    mBarLvl->SetTexture(tex);

    const int marginBarLvlV = 10;
    const int barLvlX = mTitle->GetX() + (mTitle->GetWidth() - mBarLvl->GetWidth()) / 2;
    const int barLvlY = mTitle->GetY() + mTitle->GetHeight() + marginBarLvlV;
    mBarLvl->SetPosition(barLvlX, barLvlY);

    // SET IMAGE
    const ObjectData & data = mObjDataReg->GetObjectData(type);
    tex = tm->GetSprite(data.GetIconTexFile(), data.GetIconTexId(faction));

    mImg->SetTexture(tex);

    const int imgAreaX = 20;
    const int imgAreaY = 20;
    const int imgAreaW = 150;
    const int imgAreaH = 120;
    const int borderImgArea = 5;
    const int borderTot = borderImgArea * 2;
    const int imgW = mImg->GetWidth();
    const int imgH = mImg->GetHeight();
    const int imgX = imgAreaX + (imgAreaW - imgW) / 2;
    const int imgY = imgAreaY + (imgAreaH - imgH) / 2;
    mImg->SetPosition(imgX, imgY);

    // image too big for the button
    if(imgW >= (imgAreaW - borderTot) || imgH >= (imgAreaH - borderTot))
    {
        // const int visX = (imgW - imgAreaW) / 2 + borderImgArea;
        // const int visY = (imgH - imgAreaH) / 2 + borderImgArea;
        // const int visW = imgAreaW - borderTot;
        // const int visH = imgAreaH - borderTot;

        // mImg->SetVisibleArea(visX, visY, visW, visH);

        int newW;
        int newH;

        if(imgW > imgH)
        {
            const float ratio = static_cast<float>(imgH) / static_cast<float>(imgW);

            newW = imgAreaW - borderTot;
            newH = static_cast<int>(newW * ratio);
        }
        // imgH >= imgW
        else
        {
            const float ratio = static_cast<float>(imgW) / static_cast<float>(imgH);

            newH = imgAreaH - borderTot;
            newW = static_cast<int>(newH * ratio);
        }

        mImg->SetWidth(newW);
        mImg->SetHeight(newH);

        const int imgX2 = imgAreaX + (imgAreaW - newW) / 2;
        const int imgY2 = imgAreaY + (imgAreaH - newH) / 2;
        mImg->SetPosition(imgX2, imgY2);
    }
    // else
    //     mImg->ClearVisibleArea();

    // STATS
    UpdateStats();

    // BUTTONS FUNCTION
    const bool showAutoActions = obj->GetObjectCategory() == GameObject::CAT_UNIT;

    mButtonAutoAttack->SetVisible(showAutoActions);
    mButtonAutoMove->SetVisible(showAutoActions);

    // TODO get values from object when implementing auto action
    mButtonAutoAttack->SetChecked(false);
    mButtonAutoMove->SetChecked(false);
}

void PanelSelectedObject::HandlePositionChanged()
{
    PositionElements();
}

void PanelSelectedObject::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0 ,y0);
}

void PanelSelectedObject::UpdateStats()
{
    const int exp = mObj->GetExperience();
    const int maxExp = mObj->GetExperienceToNextLevel();

    // STAT BARS
    mStatEnergy->SetValue(mObj->GetEnergy(), mObj->GetMaxEnergy());
    mStatHealth->SetValue(mObj->GetHealth(), mObj->GetMaxHealth());
    mStatExperience->SetValue(exp, maxExp);

    // INFO / UPGRADE BUTTON
    const bool showInfo = exp < maxExp;
    const bool showUpgrade = !showInfo;

    mButtonShowInfo->SetVisible(showInfo);
    mButtonShowUpgrade->SetVisible(showUpgrade);
}

} // namespace game
