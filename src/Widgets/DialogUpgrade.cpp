#include "Widgets/DialogUpgrade.h"

#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonDialogOk.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

class ButtonDec : public sgl::sgui::ImageButton
{
public:
    ButtonDec(ObjAttId attId, sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_UP_BTN_MINUS_NORMAL, ID_DLG_UP_BTN_MINUS_DISABLED,
                               ID_DLG_UP_BTN_MINUS_OVER, ID_DLG_UP_BTN_MINUS_PUSHED,
                               ID_DLG_UP_BTN_MINUS_PUSHED }, SpriteFileDialogUpgrade, parent)
    , mAttId(attId)
    {
    }

    void HandleMouseOver()
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown()
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }

    ObjAttId GetAttributeId() const { return mAttId; }

private:
    ObjAttId mAttId;
};

class ButtonInc : public sgl::sgui::ImageButton
{
public:
    ButtonInc(ObjAttId attId, sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_UP_BTN_PLUS_NORMAL, ID_DLG_UP_BTN_PLUS_DISABLED,
                               ID_DLG_UP_BTN_PLUS_OVER, ID_DLG_UP_BTN_PLUS_PUSHED,
                               ID_DLG_UP_BTN_PLUS_PUSHED }, SpriteFileDialogUpgrade, parent)
    , mAttId(attId)
    {
    }

    void HandleMouseOver()
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown()
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }

    ObjAttId GetAttributeId() const { return mAttId; }

private:
    ObjAttId mAttId;
};

} // namespace

namespace game
{

// ===== ValueUpgradeBar =====
ValueUpgradeBar::ValueUpgradeBar(int maxVal, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_VAL_BAR_PIP_OFF);

    int x = 0;

    for(int i = 0; i < maxVal; ++i)
    {
        auto img = new sgui::Image(tex, this);
        mImgs.emplace_back(img);
        img->SetX(x);

        x += img->GetWidth();
    }
}

void ValueUpgradeBar::SetValue(int val)
{
    using namespace sgl;

    if(val == mValue)
        return;

    mValue = val;

    auto tm = graphic::TextureManager::Instance();

    // ON
    auto tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_VAL_BAR_PIP_ON);

    for(int i = 0; i < val; ++i)
        mImgs[i]->SetTexture(tex);

    // OFF
    const unsigned int tot = mImgs.size();

    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_VAL_BAR_PIP_OFF);

    for(int i = val; i < tot; ++i)
        mImgs[i]->SetTexture(tex);
}

void ValueUpgradeBar::AddNew()
{
    if((mValue + mNumNew) == mImgs.size())
        return ;

    // update current graphics
    const unsigned int ind = mValue + mNumNew;

    auto tm = sgl::graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_VAL_BAR_PIP_NEW);
    mImgs[ind]->SetTexture(tex);

    // update value
    ++mNumNew;
}

void ValueUpgradeBar::RemNew()
{
    if(mNumNew == 0)
        return ;

    // update current graphics
    const unsigned int ind = mValue + mNumNew - 1;

    auto tm = sgl::graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_VAL_BAR_PIP_OFF);
    mImgs[ind]->SetTexture(tex);

    // update value
    --mNumNew;
}

// ===== DIALOG =====
DialogUpgrade::DialogUpgrade(GameObject * obj, const ObjectsDataRegistry * odr)
    : mObj(obj)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // init changes vector
    mChangesToApply.assign(NUM_BASIC_ATTRIBUTES, 0);

    // BACKGROUND
    const int w = 984;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogUpgradeExp);
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
    auto title = new sgui::Label(sm->GetCString("UPGRADE"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    // PANELS
    const int panelsX = WidgetsConstants::MarginDialogContentL;
    const int panelsY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
    const int panelsW = 300;
    const int panelImgH = 220;
    const int panelDataY = panelsY + panelImgH;
    const int panelDataH = 260;

    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_PANELS_BG);
    auto panels = new sgui::Image(tex, this);
    panels->SetPosition(panelsX, panelsY);

    // IMAGE
    const ObjectData & data = odr->GetObjectData(obj->GetObjectType());
    const unsigned int texInd = data.GetIconTexId(obj->GetFaction(), obj);
    tex = tm->GetSprite(data.GetIconTexFile(), texInd);

    auto img = new sgui::Image(tex, this);
    img->SetPosition(panelsX + (panelsW - img->GetWidth()) / 2,
                     panelsY + (panelImgH - img->GetHeight()) / 2);

    // -- DATA --
    const int paddingDataL = 20;
    const int paddingDataT = 20;
    const int marginHeaderV = 10;
    const int marginHeaderV2 = 5;
    const int blockDataH = 85;

    const int sizeHeader = 18;
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader,
                                  sizeHeader, graphic::Font::NORMAL);

    const int sizeText = 18;
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText,
                                sizeText, graphic::Font::NORMAL);

    // LEVEL
    const int dataX = panelsX + paddingDataL;
    int dataY = panelDataY + paddingDataT;

    auto header = new sgui::Label(sm->GetCString("LEVEL"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader2);
    header->SetPosition(dataX, dataY);

    dataY += header->GetHeight() + marginHeaderV;

    const int level = obj->GetExperienceLevel();
    const unsigned int barLvlTexId = ID_STARS_BAR_1 + level;
    tex = tm->GetSprite(SpriteFileUIShared, barLvlTexId);
    auto levelBar = new sgui::Image(tex, this);
    levelBar->SetPosition(dataX, dataY);

    // UPGRADE POINTS
    dataY = header->GetY() + blockDataH;

    header = new sgui::Label(sm->GetCString("UP_POINTS"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader2);
    header->SetPosition(dataX, dataY);

    dataY += header->GetHeight() + marginHeaderV2;

    mPointsToAssign = GameObject::UPGRADE_POINTS[level];

    mLabelPoints = new sgui::Label(std::to_string(mPointsToAssign).c_str(), fontText, this);
    mLabelPoints->SetColor(WidgetsConstants::colorDialogText);
    mLabelPoints->SetPosition(dataX, dataY);

    // -- ATTRIBUTES --
    const int areaAttX = panelsX + panelsW;
    const int areaAttY = panelsY;
    const int paddingAttL = 10;
    const int barX = 390;
    const int marginButton = 15;

    const unsigned int maxAtt = 12;

    unsigned int attUsed = 0;

    int attY = areaAttY;

    // ASSIGNED ATTRIBUTES
    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_ATT_BG);

    for(unsigned int i = 0; i < NUM_BASIC_ATTRIBUTES; ++i)
    {
        const auto attId = static_cast<ObjAttId>(i);
        const int val = obj->GetAttribute(attId);

        if(val <= 0)
            continue;

        // attribute bg
        auto bg = new sgui::Image(tex, this);
        bg->SetPosition(areaAttX, attY);

        // attribute header
        header = new sgui::Label(sm->GetCString(ObjectData::STR_ATTRIBUTES[i]),
                                 fontHeader, bg);
        header->SetColor(WidgetsConstants::colorDialogHeader2);
        header->SetPosition(paddingAttL, (bg->GetHeight() - header->GetHeight()) / 2);

        const int timeShowing = 3000;
        auto tt = new GameSimpleTooltip(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[i]));
        header->SetTooltip(tt);
        header->SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
        header->SetTooltipShowingTime(timeShowing);

        // attribute bar
        auto bar = new ValueUpgradeBar(MAX_STAT_IVAL, bg);
        bar->SetValue(val);
        bar->SetPosition(barX, (bg->GetHeight() - bar->GetHeight()) / 2);

        // decrement
        auto btnDec = new ButtonDec(attId, bg);
        mButtonsDec.emplace_back(btnDec);
        btnDec->SetEnabled(false);
        btnDec->SetPosition(barX - btnDec->GetWidth() - marginButton,
                            (bg->GetHeight() - btnDec->GetHeight()) / 2);

        btnDec->AddOnClickFunction([this, bar, btnDec, i]
        {
            bar->RemNew();

            --mChangesToApply[i];
            ++mPointsToAssign;

            if(0 == mChangesToApply[i])
                btnDec->SetEnabled(false);

            OnPointsChanged();
        });

        // increment
        auto btnInc = new ButtonInc(attId, bg);
        mButtonsInc.emplace_back(btnInc);
        btnInc->SetPosition(barX + bar->GetWidth() + marginButton,
                            (bg->GetHeight() - btnInc->GetHeight()) / 2);

        const bool enable = val < MAX_STAT_IVAL;
        btnInc->SetEnabled(enable);

        btnInc->AddOnClickFunction([this, bar, btnDec, i]
        {
            bar->AddNew();

            btnDec->SetEnabled(true);

            ++mChangesToApply[i];
            --mPointsToAssign;

            OnPointsChanged();
        });

        // move to next
        attY += bg->GetHeight();

        ++attUsed;

        if(attUsed == maxAtt)
            break;
    }

    // OTHER SLOTS
    tex = tm->GetSprite(SpriteFileDialogUpgrade, ID_DLG_UP_ATT_DISABLED_BG);

    for(unsigned int i = attUsed; i < maxAtt; ++i)
    {
        auto bg = new sgui::Image(tex, this);
        bg->SetPosition(areaAttX, attY);

        attY += bg->GetHeight();
    }

    // BUTTON UPGRADE
    const int marginButtonT = 25;
    const int btnX1 = areaAttX + tex->GetWidth();
    const int btnY = attY + marginButtonT;
    mBtnUpgrade = new ButtonDialogOk(sm->GetCString("UPGRADE"), this);
    mBtnUpgrade->SetEnabled(false);
    mBtnUpgrade->SetPosition(btnX1 - mBtnUpgrade->GetWidth(), btnY);

    mBtnUpgrade->AddOnClickFunction([this]
    {
        mObj->UpgradeLevel(mChangesToApply);

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("game/upgrade-01.ogg");

        mBtnClose->Click();
    });

    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/dialog_open-01.ogg");
}

void DialogUpgrade::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogUpgrade::HandlePositionChanged()
{
    SetPositions();
}

void DialogUpgrade::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

void DialogUpgrade::OnPointsChanged()
{
    // no more points to assign
    if(0 == mPointsToAssign)
    {
        // disable buttons INC
        for(auto b : mButtonsInc)
            b->SetEnabled(false);

        // enable button UPGRADE
        mBtnUpgrade->SetEnabled(true);
    }
    else
    {
        // enable buttons INC
        const int MAX_SLOTS = MAX_STAT_IVAL;
        const unsigned int numButtons = mButtonsInc.size();

        for(unsigned int i = 0; i < numButtons; ++i)
        {
            const ObjAttId attId = static_cast<ButtonInc *>(mButtonsInc[i])->GetAttributeId();
            const int used = mObj->GetAttribute(attId) + mChangesToApply[i];
            const bool enable = used < MAX_SLOTS;

            mButtonsInc[i]->SetEnabled(enable);
        }

        // disable button UPGRADE
        mBtnUpgrade->SetEnabled(false);
    }

    mLabelPoints->SetText(std::to_string(mPointsToAssign).c_str());
}

} // namespace game
