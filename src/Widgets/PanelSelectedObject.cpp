#include "Widgets/PanelSelectedObject.h"

#include "GameData.h"
#include "GameUIData.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/DigitsDisplay.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/ProgressBarObjectVisualStat.h"
#include "Widgets/WidgetsConstants.h"

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
#include <sgl/utilities/StringManager.h>

#include <array>

#include <cmath>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

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
class ObjectVisualStat : public sgl::sgui::Widget,
                         public sgl::utilities::StringsChangeListener
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
        , mType(type)
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
        mDigits = new DigitsDisplay(5, sizeFont, std::string(), this);
        mDigits->SetColorDigits(colorDigits);
        mDigits->SetColorZeros(colorZeros);

        // SIZE
        const int w = 260;
        const int h = mIcon->GetHeight();
        SetSize(w, h);

        const int digitsX = w - mDigits->GetWidth();
        mDigits->SetPosition(digitsX, (h - mDigits->GetHeight()) / 2);

        const int barMargin = 10;
        const int barX = digitsX - barMargin - mBar->GetWidth();
        mBar->SetPosition(barX, (h - mBar->GetHeight()) / 2);

        // TOOLTIP
        auto sm = utilities::StringManager::Instance();
        sm->AddListener(this);

        const char * tooltipText[NUM_VISUAL_STAT_TYPES] =
        {
            sm->GetCString("TT_ENERGY"),
            sm->GetCString("TT_HEALTH"),
            sm->GetCString("TT_EXPERIENCE"),
        };

        mTooltip = new GameSimpleTooltip(tooltipText[type]);
        SetTooltip(mTooltip);
        SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
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
        mDigits->SetValue(val);
    }

private:
    void OnStringsChanged() override
    {
        auto sm = sgl::utilities::StringManager::Instance();

        const char * tooltipText[NUM_VISUAL_STAT_TYPES] =
        {
            sm->GetCString("TT_ENERGY"),
            sm->GetCString("TT_HEALTH"),
            sm->GetCString("TT_EXPERIENCE"),
        };

        mTooltip->SetText(tooltipText[mType]);
    }

private:
    sgl::sgui::Image * mIcon = nullptr;
    ProgressBarObjectVisualStat * mBar = nullptr;
    DigitsDisplay * mDigits = nullptr;
    GameSimpleTooltip * mTooltip = nullptr;
    VisualStatType mType;
};

// ========== BUTTON OBJECT FUNCTION ==========
class ButtonObjectFunction : public sgl::sgui::ImageButton,
                             public sgl::utilities::StringsChangeListener
{
public:
    enum ObjFunction : unsigned int
    {
        // OBJFUN_AUTO_ATTACK,
        // OBJFUN_AUTO_MOVE,
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
        , mType(f)
    {
        using namespace sgl;

        //SetCheckable(OBJFUN_AUTO_ATTACK == f || OBJFUN_AUTO_MOVE == f);

        // ICON
        const unsigned int textIds[NUM_OBJECT_FUNCTIONS] =
        {
            // ID_PAN_SELOBJ_ICON_ACT_ATTACK,
            // ID_PAN_SELOBJ_ICON_ACT_MOVE,
            ID_PAN_SELOBJ_ICON_ACT_INFO,
            ID_PAN_SELOBJ_ICON_ACT_UPGRADE,
        };

        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFilePanelSelectedObject, textIds[f]);

        mIcon = new sgui::Image(tex, this);
        mIcon->SetPosition((GetWidth() - mIcon->GetWidth()) / 2,
                          (GetHeight() - mIcon->GetHeight()) / 2);

        // TOOLTIP
        auto sm = utilities::StringManager::Instance();
        sm->AddListener(this);

        const char * tooltipText[NUM_OBJECT_FUNCTIONS] =
        {
            // "If checked enables automatic attack when spotting an enemy during their turn",
            // "If checked enables automatic move when spotting an enemy during their turn",
            sm->GetCString("TT_SHOW_INFO"),
            sm->GetCString("TT_SHOW_UPGRADE"),
        };

        const int ttTime = 3500;
        mTooltip = new GameSimpleTooltip(tooltipText[f]);
        SetTooltip(mTooltip);
        SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
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

    void OnStringsChanged() override
    {
        auto sm = sgl::utilities::StringManager::Instance();

        const char * tooltipText[NUM_OBJECT_FUNCTIONS] =
        {
                // "If checked enables automatic attack when spotting an enemy during their turn",
                // "If checked enables automatic move when spotting an enemy during their turn",
                sm->GetCString("TT_SHOW_INFO"),
                sm->GetCString("TT_SHOW_UPGRADE"),
        };

        mTooltip->SetText(tooltipText[mType]);
    }

private:
    sgl::sgui::Image * mIcon = nullptr;
    GameSimpleTooltip * mTooltip = nullptr;
    ObjFunction mType;
};

} // namespace

namespace game
{

// ========== PANEL ==========
PanelSelectedObject::PanelSelectedObject(const ObjectsDataRegistry * odr, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mObjDataReg(odr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

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

    const int sizeTitle = 16;

    auto fntTitle = fm->GetFont(WidgetsConstants::FontFilePanelTitle, sizeTitle,
                                graphic::Font::NORMAL);

    mTitle = new sgui::TextArea(titleW, titleH, fntTitle, true, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);
    mTitle->setTextAlignmentHorizontal(sgui::TextArea::ALIGN_H_CENTER);
    mTitle->SetPosition(titleX, titleY);

    // LEVEL
    mBarLvl = new sgui::Image(this);

    mTooltipExp = new GameSimpleTooltip(sm->GetCString("TT_EXP_LVL"));
    mBarLvl->SetTooltip(mTooltipExp);
    mBarLvl->SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);

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
    // mButtonAutoAttack = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_AUTO_ATTACK, this);
    // mButtonAutoAttack->AddOnToggleFunction([this](bool checked)
    // {
    // });

    // mButtonAutoMove = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_AUTO_MOVE, this);
    // mButtonAutoMove->AddOnToggleFunction([this](bool checked)
    // {
    // });

    mButtonInfo = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_SHOW_INFO, this);

    mButtonUpgrade = new ButtonObjectFunction(ButtonObjectFunction::OBJFUN_SHOW_UPGRADE, this);

    const int marginButtonFun = 5;
    const int buttonFunX0 = contentX0 + marginButtonFun;
    const int buttonFunY0 = contentY0 + contentH - mButtonInfo->GetHeight() - marginButtonFun;

    // mButtonAutoAttack->SetPosition(buttonFunX, buttonFunY);

    // buttonFunX += mButtonAutoAttack->GetWidth();
    // mButtonAutoMove->SetPosition(buttonFunX, buttonFunY);

    int buttonFunX = contentX0 + contentW - mButtonInfo->GetWidth() - marginButtonFun;
    int buttonFunY = buttonFunY0;

    mButtonInfo->SetPosition(buttonFunX, buttonFunY);

    buttonFunX -= marginButtonFun + mButtonUpgrade->GetWidth();
    mButtonUpgrade->SetPosition(buttonFunX, buttonFunY);
}

void PanelSelectedObject::AddFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void PanelSelectedObject::AddFunctionOnShowInfo(const std::function<void()> & f)
{
    mButtonInfo->AddOnClickFunction(f);
}

void PanelSelectedObject::AddFunctionOnShowUpgrade(const std::function<void()> & f)
{
    mButtonUpgrade->AddOnClickFunction(f);
}

void PanelSelectedObject::ClearObject()
{
    if(nullptr == mObj)
        return ;

    mObj->RemoveFunctionOnValueChanged(mFuncValuesChangedId);
    mFuncValuesChangedId = 0;

    mObj = nullptr;
}

void PanelSelectedObject::SetObject(GameObject * obj)
{
    using namespace sgl;

    // object already set -> exit
    if(obj == mObj)
        return ;

    auto sm = utilities::StringManager::Instance();

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
    mTitle->SetText(sm->GetCString(ObjectData::TITLES.at(type)));

    // BAR LEVEL
    UpdateBarLevel();

    const int marginBarLvlV = 10;
    const int barLvlX = mTitle->GetX() + (mTitle->GetWidth() - mBarLvl->GetWidth()) / 2;
    const int barLvlY = mTitle->GetY() + mTitle->GetHeight() + marginBarLvlV;
    mBarLvl->SetPosition(barLvlX, barLvlY);

    // SET IMAGE
    const ObjectData & data = mObjDataReg->GetObjectData(type);
    const unsigned int texInd = data.GetIconTexId(faction, obj);
    tex = tm->GetSprite(data.GetIconTexFile(), texInd);

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

    // STATS
    UpdateStats();

    // BUTTONS FUNCTION
    const bool showAutoActions = obj->GetObjectCategory() == ObjectData::CAT_UNIT;

    // mButtonAutoAttack->SetVisible(showAutoActions);
    // mButtonAutoMove->SetVisible(showAutoActions);

    // TODO get values from object when implementing auto action
    // mButtonAutoAttack->SetChecked(false);
    // mButtonAutoMove->SetChecked(false);
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

    // LEVEL
    UpdateBarLevel();

    // STAT BARS
    static_cast<ObjectVisualStat *>(mStatEnergy)->SetValue(mObj->GetEnergy(), mObj->GetMaxEnergy());
    static_cast<ObjectVisualStat *>(mStatHealth)->SetValue(mObj->GetHealth(), mObj->GetMaxHealth());
    static_cast<ObjectVisualStat *>(mStatExperience)->SetValue(exp, maxExp);

    // UPGRADE BUTTON
    const bool showUpgrade = exp >= maxExp &&
                             (mObj->GetExperienceLevel() + 1) < GameObject::MAX_LEVEL;

    mButtonUpgrade->SetVisible(showUpgrade);
}

void PanelSelectedObject::UpdateBarLevel()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    const unsigned int barLvlTexId = ID_STARS_BAR_1 + mObj->GetExperienceLevel();
    auto tex = tm->GetSprite(SpriteFileUIShared, barLvlTexId);
    mBarLvl->SetTexture(tex);
}

void PanelSelectedObject::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    // TITLE
    if(mObj != nullptr)
    {
        const GameObjectTypeId type = mObj->GetObjectType();
        mTitle->SetText(sm->GetCString(ObjectData::TITLES.at(type)));
    }

    // tooltip exp level
    mTooltipExp->SetText(sm->GetCString("TT_EXP_LVL"));
}

} // namespace game
