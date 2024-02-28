#include "Widgets/DialogNewElement.h"

#include "GameConstants.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/ButtonPanelTab.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ObjectVisualAttribute.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/DummyRenderable.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>

#include <cassert>

namespace game
{

constexpr int NUM_SLOTS = 6;

// ===== BUTTON LEFT =====
class ButtonLeft : public sgl::sgui::ImageButton
{
public:
    ButtonLeft(sgl::sgui::Widget * parent)
        : ImageButton({ IND_DLG_NEWE_LEFT_NORMAL,
                        IND_DLG_NEWE_LEFT_DISABLED,
                        IND_DLG_NEWE_LEFT_OVER,
                        IND_DLG_NEWE_LEFT_PUSHED,
                        IND_DLG_NEWE_LEFT_NORMAL },
                        SpriteFileDialogNewElement, parent)
    {
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-03.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-03.ogg");
    }
};

// ===== BUTTON RIGHT =====
class ButtonRight : public sgl::sgui::ImageButton
{
public:
    ButtonRight(sgl::sgui::Widget * parent)
        : ImageButton({ IND_DLG_NEWE_RIGHT_NORMAL,
                        IND_DLG_NEWE_RIGHT_DISABLED,
                        IND_DLG_NEWE_RIGHT_OVER,
                        IND_DLG_NEWE_RIGHT_PUSHED,
                        IND_DLG_NEWE_RIGHT_NORMAL },
                        SpriteFileDialogNewElement, parent)
    {
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-03.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-03.ogg");
    }
};

// ===== BUTTON CLOSE =====

class ButtonClose : public sgl::sgui::AbstractButton
{
public:
    ButtonClose(sgl::sgui::Widget * parent)
        : AbstractButton(parent)
        , mBody(new sgl::graphic::Image)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);

        // register graphic elements
        RegisterRenderable(mBody);

        // set initial visual state
        SetState(NORMAL);
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
        player->PlaySound("UI/dialog_close-01.ogg");
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            IND_DLG_NEWE_CLOSE_NORMAL,
            IND_DLG_NEWE_CLOSE_NORMAL,     // button can't be disabled
            IND_DLG_NEWE_CLOSE_OVER,
            IND_DLG_NEWE_CLOSE_PUSHED,
            IND_DLG_NEWE_CLOSE_NORMAL,     // button can't be checked
        };

        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileDialogNewElement, texIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());
    }

    void HandlePositionChanged() override
    {
        mBody->SetPosition(GetScreenX(), GetScreenY());
    }

private:
    sgl::graphic::Image * mBody = nullptr;
};

// ===== BUTTON BUILD =====

class ButtonBuild : public sgl::sgui::AbstractButton
{
public:
    ButtonBuild(sgl::sgui::Widget * parent)
        : sgl::sgui::AbstractButton(parent)
        , mBody(new sgl::graphic::Image)
    {
        using namespace sgl::graphic;

        SetShortcutKey(sgl::core::KeyboardEvent::KEY_B);

        auto fm = FontManager::Instance();
        auto font = fm->GetFont("Lato-Bold.ttf", 11, Font::NORMAL);
        mShortcut = new Text("B", font, true);
        mShortcut->SetColor(0xd5daddff);

        // register graphic elements
        RegisterRenderable(mBody);
        RegisterRenderable(mShortcut);

        // LABEL
        font = fm->GetFont("Lato-Regular.ttf", 19, Font::NORMAL);
        mLabel = new Text("BUILD", font);
        RegisterRenderable(mLabel);

        // set initial visual state
        SetState(NORMAL);
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click_long-02.ogg");
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        // BACKGROUND
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            IND_DLG_NEWE_BUILD_NORMAL,
            IND_DLG_NEWE_BUILD_DISABLED,
            IND_DLG_NEWE_BUILD_OVER,
            IND_DLG_NEWE_BUILD_PUSHED,
            IND_DLG_NEWE_BUILD_PUSHED,
        };

        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileDialogNewElement, texIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // LABEL
        const unsigned int colorLabel[NUM_VISUAL_STATES] =
        {
            0xe3e6e8ff,
            0x454f54ff,
            0xf1f2f4ff,
            0xabb4baff,
            0xc2c2a3ff
        };

        mLabel->SetColor(colorLabel[state]);

        // update shortcut label alpha
        const unsigned char alphaEn = 255;
        const unsigned char alphaDis = 128;
        const unsigned char alphaLabel = DISABLED == state ? alphaDis : alphaEn;
        mShortcut->SetAlpha(alphaLabel);
    }

    void HandlePositionChanged() override
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // position BG
        mBody->SetPosition(x0, y0);

        // SHORTCUT
        const int shortBgX0 = 182;
        const int shortBgY0 = 22;
        const int shortBgSize = 14;

        const int shortcutX = x0 + shortBgX0 + (shortBgSize - mShortcut->GetWidth()) * 0.5f;
        const int shortcutY = y0 + shortBgY0 + (shortBgSize - mShortcut->GetHeight()) * 0.5f;

        mShortcut->SetPosition(shortcutX, shortcutY);

        // LABEL
        const int labelX = x0 + (GetWidth() - mLabel->GetWidth()) * 0.5f;
        const int labelY = y0 + (GetHeight() - mLabel->GetHeight()) * 0.5f;

        mLabel->SetPosition(labelX, labelY);
    }

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::graphic::Text * mLabel = nullptr;
    sgl::graphic::Text * mShortcut = nullptr;
};

// ===== BUTTON SLOT =====

class ButtonSlot : public sgl::sgui::AbstractButton
{
public:
    ButtonSlot(int index)
        : mBody(new sgl::graphic::Image)
    {
        using namespace sgl::graphic;

        SetShortcutKey(KEYS[index]);

        SetCheckable(true);

        auto fm = FontManager::Instance();

        // TITLE
        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_PANEL_NORMAL);
        auto font = fm->GetFont("Lato-Regular.ttf", 16, Font::NORMAL);
        mTitle = new sgl::sgui::TextArea(tex->GetWidth(), TITLE_H, font, false, this);
        mTitle->setTextAlignment(sgl::sgui::TextArea::ALIGN_H_CENTER, sgl::sgui::TextArea::ALIGN_V_CENTER);

        // IMAGE
        mImage = new DummyRenderable;

        // SHORTCUT
        font = fm->GetFont("Lato-Bold.ttf", 12, Font::NORMAL);
        mShortcut = new Text(SHORTCUTS[index], font);
        mShortcut->SetColor(0xd5daddff);

        // register graphic elements
        RegisterRenderable(mBody);
        RegisterRenderable(mImage);
        RegisterRenderable(mShortcut);

        // set initial visual state
        SetState(NORMAL);
    }

    void ClearData()
    {
        // already no data -> exit
        if(!mHasData)
            return;

        // title
        mTitle->ClearText();

        // image
        UnregisterRenderable(mImage);
        delete mImage;

        mImage = new sgl::graphic::DummyRenderable;
        RegisterRenderable(mImage);

        // clear flag
        mHasData = false;
    }

    void SetData(const char * title, sgl::graphic::Texture * texImg)
    {
        using namespace sgl::graphic;

        // title
        mTitle->SetText(title);

        // image
        if(!mHasData)
        {
            UnregisterRenderable(mImage);
            delete mImage;

            mImage = new Image;
            RegisterRenderable(mImage);
        }

        static_cast<Image *>(mImage)->SetTexture(texImg);

        mHasData = true;

        // reset positions
        HandlePositionChanged();

        OnStateChanged(GetState());
    }

private:
    void HandleButtonDown() override
    {
        if(IsChecked())
            return ;

        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/checkbox-02.ogg");
    }

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            IND_DLG_NEWE_PANEL_NORMAL,
            IND_DLG_NEWE_PANEL_DISABLED,
            IND_DLG_NEWE_PANEL_OVER,
            IND_DLG_NEWE_PANEL_SEL,
            IND_DLG_NEWE_PANEL_SEL,
        };

        auto tm = sgl::graphic::TextureManager::Instance();
        sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileDialogNewElement, texIds[state]);
        mBody->SetTexture(tex);

        SetSize(mBody->GetWidth(), mBody->GetHeight());

        // set title color
        const unsigned int colorTitle[NUM_VISUAL_STATES] =
        {
            0xc6cad2ff,
            0x687183ff,
            0xc9cdd4ff,
            0xc6c99cff,
            0xc6c99cff
        };

        mTitle->SetColor(colorTitle[state]);

        // update shortcut label alpha
        const unsigned char alphaEn = 255;
        const unsigned char alphaDis = 128;
        const unsigned char alphaLabel = DISABLED == state ? alphaDis : alphaEn;
        mShortcut->SetAlpha(alphaLabel);

        // image - same alpha as label
        mImage->SetAlpha(alphaLabel);
    }

    void HandlePositionChanged() override
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // position BG
        mBody->SetPosition(x0, y0);

        // IMAGE
        const int imageBlockH = GetHeight() - TITLE_H;
        const int imgX = x0 + (GetWidth() - mImage->GetWidth()) * 0.5f;
        const int imgY = y0 + TITLE_H + (imageBlockH - mImage->GetHeight()) * 0.5f;
        mImage->SetPosition(imgX, imgY);

        // SHORTCUT
        const int shortBgX0 = 182;
        const int shortBgY0 = 182;
        const int shortBgSize = 14;

        const int shortcutX = x0 + shortBgX0 + (shortBgSize - mShortcut->GetWidth()) * 0.5f;
        const int shortcutY = y0 + shortBgY0 + (shortBgSize - mShortcut->GetHeight()) * 0.5f;

        mShortcut->SetPosition(shortcutX, shortcutY);
    }

private:
    static const int KEYS[NUM_SLOTS];
    static const char * SHORTCUTS[NUM_SLOTS];
    static const int TITLE_H = 50;

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::sgui::TextArea * mTitle = nullptr;
    sgl::graphic::Renderable * mImage = nullptr;
    sgl::graphic::Text * mShortcut = nullptr;

    bool mHasData = false;
};

const int ButtonSlot::KEYS[NUM_SLOTS] = {
                                            sgl::core::KeyboardEvent::KEY_1,
                                            sgl::core::KeyboardEvent::KEY_2,
                                            sgl::core::KeyboardEvent::KEY_3,
                                            sgl::core::KeyboardEvent::KEY_4,
                                            sgl::core::KeyboardEvent::KEY_5,
                                            sgl::core::KeyboardEvent::KEY_6
                                        };


const char * ButtonSlot::SHORTCUTS[NUM_SLOTS] = { "1", "2", "3", "4", "5", "6" };

// ===== DIALOG NEW ELEMENT =====
DialogNewElement::DialogNewElement(ElemType type, Player * player,
                                   const ObjectsDataRegistry * dataReg)
    : mPlayer(player)
    , mDataReg(dataReg)
    , mElemType(type)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const int marginL = 40;
    const int marginT = 8;

    int midBgH = 470;
    int slotsY0 = 65;

    if(ETYPE_UNITS_BARRACKS == type)
    {
        const std::vector<GameObjectTypeId> & allUnits = player->GetAvailableUnits();

        for(GameObjectTypeId t : allUnits)
        {
            const ObjectBasicData & data = dataReg->GetObjectData(t);

            if(OCU_SOLDIER == data.objClass)
                mTypes.push_back(t);
        }
    }
    else if(ETYPE_UNITS_BASE == type)
    {
        const std::vector<GameObjectTypeId> & allUnits = player->GetAvailableUnits();

        for(GameObjectTypeId t : allUnits)
        {
            const ObjectBasicData & data = dataReg->GetObjectData(t);

            if(OCU_WORKER == data.objClass)
                mTypes.push_back(t);
        }
    }
    else
    {
        midBgH = 515;
        slotsY0 = 110;
    }

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogNewElementExp, IND_DLG_NEWE_BG_TOP);
    tex->SetScaleMode(0);
    mBgTop = new graphic::Image(tex);
    RegisterRenderable(mBgTop);

    tex = tm->GetSprite(SpriteFileDialogNewElementExp, IND_DLG_NEWE_BG_MID);
    tex->SetScaleMode(0);
    mBgMid = new graphic::Image(tex);
    mBgMid->SetHeight(midBgH);
    RegisterRenderable(mBgMid);

    tex = tm->GetSprite(SpriteFileDialogNewElementExp, IND_DLG_NEWE_BG_BTM);
    tex->SetScaleMode(0);
    mBgBtm = new graphic::Image(tex);
    RegisterRenderable(mBgBtm);

    const int w = mBgTop->GetWidth();
    const int h = mBgTop->GetHeight() + mBgMid->GetHeight() + mBgBtm->GetHeight();
    SetSize(w, h);

    // CLOSE BUTTON
    mBtnClose = new ButtonClose(this);
    mBtnClose->SetX(GetWidth() - mBtnClose->GetWidth());

    // TITLE
    auto font = fm->GetFont("Lato-Regular.ttf", 28, sgl::graphic::Font::NORMAL);

    if(ETYPE_STRUCTURES == type)
        mTitle = new sgui::Label("CREATE NEW STRUCTURE", font, this);
    else
        mTitle = new sgui::Label("CREATE NEW UNIT", font, this);

    mTitle->SetColor(0xf1f2f4ff);
    mTitle->SetPosition(marginL, marginT);

    // SLOTS
    mSlots = new sgui::ButtonsGroup(sgui::ButtonsGroup::HORIZONTAL, this);

    for(int i = 0; i < NUM_SLOTS; ++i)
    {
        auto slot = new ButtonSlot(i);

        slot->AddOnToggleFunction([this, i](bool checked)
        {
            if(checked)
            {
                const int ind = mFirstElem + i;
                ShowData(ind);
                CheckBuild(ind);
            }
        });

        mSlots->AddButton(slot);
    }

    mSlots->SetPosition(marginL, slotsY0);

    const int marginButtonsLR = 10;

    const int numData = mTypes.size();

    mBtnLeft = new ButtonLeft(this);
    const int posLX = mSlots->GetX() - mBtnLeft->GetWidth() - marginButtonsLR;
    const int posLY = mSlots->GetY() + (mSlots->GetHeight() - mBtnLeft->GetHeight()) * 0.5f;
    mBtnLeft->SetPosition(posLX, posLY);
    mBtnLeft->SetEnabled(false);

    mBtnLeft->AddOnClickFunction([this]
    {
       mFirstElem -= NUM_SLOTS;
       UpdateSlots();

       mBtnLeft->SetEnabled(mFirstElem > 0);
       mBtnRight->SetEnabled(true);
    });

    mBtnRight = new ButtonRight(this);
    const int posRX = mSlots->GetX() + mSlots->GetWidth() + marginButtonsLR;
    const int posRY = mSlots->GetY() + (mSlots->GetHeight() - mBtnRight->GetHeight()) * 0.5f;
    mBtnRight->SetPosition(posRX, posRY);
    mBtnRight->SetEnabled(numData > NUM_SLOTS);

    mBtnRight->AddOnClickFunction([this, numData]
    {
       mFirstElem += NUM_SLOTS;
       UpdateSlots();

       mBtnLeft->SetEnabled(true);
       mBtnRight->SetEnabled(mFirstElem + NUM_SLOTS < numData);
    });

    // INFO PANEL
    const int slotsMarginBottom = 15;

    const int panelY0 = slotsY0 + mSlots->GetHeight() + slotsMarginBottom;
    tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_INFO);
    auto panelInfo = new sgui::Image(tex, this);
    panelInfo->SetPosition(marginL, panelY0);

    const unsigned int colorHeader = 0xf1f2f4ff;
    const unsigned int colorText = 0xb6c0c9ff;

    auto fontHeader = fm->GetFont("Lato-Bold.ttf", 18, sgl::graphic::Font::NORMAL);
    auto fontText = fm->GetFont("Lato-Regular.ttf", 17, sgl::graphic::Font::NORMAL);

    const int marginPanelXY0 = 10;
    const int marginPanelBlock = 20;
    const int marginPanelIconH = 5;
    const int marginPanelIconV = 5;
    const int marginPanelIconBlock = 85;
    const int marginPanelDataV = 5;

    // header DESCRIPTION
    auto headerDesc = new sgui::Label("DESCRIPTION", fontHeader, panelInfo);
    headerDesc->SetColor(colorHeader);
    headerDesc->SetPosition(marginPanelXY0, marginPanelXY0);

    int contY = headerDesc->GetY() + headerDesc->GetHeight() + marginPanelDataV;

    // description text area
    const int areaW = panelInfo->GetWidth() - (2 * marginPanelXY0);
    const int areaH = 85;
    mDescription = new sgui::TextArea(areaW, areaH, fontText, false, panelInfo);
    mDescription->SetColor(colorText);
    mDescription->SetPosition(marginPanelXY0, contY);

    contY += mDescription->GetHeight() + marginPanelBlock;

    // header category
    auto headerCat = new sgui::Label("CATEGORY", fontHeader, panelInfo);
    headerCat->SetColor(colorHeader);
    headerCat->SetPosition(marginPanelXY0, contY);

    contY = headerCat->GetY() + headerCat->GetHeight() + marginPanelDataV;

    // text category
    mCategory = new sgui::Label("-", fontText, panelInfo);
    mCategory->SetColor(colorText);
    mCategory->SetPosition(marginPanelXY0, contY);

    contY += mCategory->GetHeight() + marginPanelBlock;

    // header cost
    headerCat = new sgui::Label("COST", fontHeader, panelInfo);
    headerCat->SetColor(colorHeader);
    headerCat->SetPosition(marginPanelXY0, contY);

    contY = headerCat->GetY() + headerCat->GetHeight() + marginPanelDataV;

    // data cost 1
    tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ICON_ENERGY);
    auto imgIcon = new sgui::Image(tex, panelInfo);
    imgIcon->SetPosition(marginPanelXY0, contY);

    int contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconH;

    mLabelsCost[0] = new sgui::Label("-", fontText, panelInfo);
    mLabelsCost[0]->SetColor(colorText);
    contY = imgIcon->GetY() + (imgIcon->GetHeight() - mLabelsCost[0]->GetHeight()) * 0.5f;
    mLabelsCost[0]->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconBlock;
    contY = imgIcon->GetY();

    // data cost 2
    tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ICON_MATERIAL);
    imgIcon = new sgui::Image(tex, panelInfo);
    imgIcon->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconH;

    mLabelsCost[1] = new sgui::Label("-", fontText, panelInfo);
    mLabelsCost[1]->SetColor(colorText);
    contY = imgIcon->GetY() + (imgIcon->GetHeight() - mLabelsCost[1]->GetHeight()) * 0.5f;
    mLabelsCost[1]->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconBlock;
    contY = imgIcon->GetY();

    // data cost 3
    tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ICON_DIAMOND);
    imgIcon = new sgui::Image(tex, panelInfo);
    imgIcon->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconH;

    mLabelsCost[2] = new sgui::Label("-", fontText, panelInfo);
    mLabelsCost[2]->SetColor(colorText);
    contY = imgIcon->GetY() + (imgIcon->GetHeight() - mLabelsCost[2]->GetHeight()) * 0.5f;
    mLabelsCost[2]->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconBlock;
    contY = imgIcon->GetY();

    // data cost 4
    tex = tm->GetSprite(SpriteFileDialogNewElement, IND_DLG_NEWE_ICON_BLOB);
    imgIcon = new sgui::Image(tex, panelInfo);
    imgIcon->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconH;

    mLabelsCost[3] = new sgui::Label("-", fontText, panelInfo);
    mLabelsCost[3]->SetColor(colorText);
    contY = imgIcon->GetY() + (imgIcon->GetHeight() - mLabelsCost[3]->GetHeight()) * 0.5f;
    mLabelsCost[3]->SetPosition(contX, contY);

    // ATTRIBUTE PANELS
    const int panelsX0 = marginL + panelInfo->GetWidth();
    const int panelsY0 = panelY0;
    const int panelBorder = 1;
    int panelsX = panelsX0;
    int panelsY = panelsY0;

    for(int c = 0; c < VIS_ATT_COLS; ++c)
    {
        const int ind0 = c * VIS_ATT_ROWS;

        for(int r = 0; r < VIS_ATT_ROWS; ++r)
        {
            const int ind = ind0 + r;

            auto panAtt = new ObjectVisualAttribute(this);
            panAtt->SetPosition(panelsX, panelsY);

            mVisAtt[ind] = panAtt;

            panelsY += panAtt->GetHeight() - panelBorder;
        }

        panelsX += mVisAtt[0]->GetWidth() - panelBorder;
        panelsY = panelsY0;
    }

    // BUTTON BUILD
    mBtnBuild = new ButtonBuild(this);
    const ObjectVisualAttribute * lastPanel = mVisAtt[NUM_VIS_ATT - 1];
    const int btnX = lastPanel->GetX() + lastPanel->GetWidth() - mBtnBuild->GetWidth();
    const int marginBtnTop = 15;
    const int btnY = lastPanel->GetY() + lastPanel->GetHeight() + marginBtnTop;
    mBtnBuild->SetPosition(btnX, btnY);

    // structures are grouped by categoy
    if(ETYPE_STRUCTURES == type)
    {
        mButtonsStructures = new sgl::sgui::AbstractButtonsGroup;

        const int btnY = 55;
        int btnX = marginL;

        auto btn = new ButtonPanelTab("RESOURCES", this);
        btn->SetPosition(btnX, btnY);
        mButtonsStructures->AddButton(btn);

        btnX += btn->GetWidth();

        btn = new ButtonPanelTab("DEFENSE", this);
        btn->SetPosition(btnX, btnY);
        mButtonsStructures->AddButton(btn);

        btnX += btn->GetWidth();

        btn = new ButtonPanelTab("TECHNOLOGY", this);
        btn->SetPosition(btnX, btnY);
        mButtonsStructures->AddButton(btn);

        btnX += btn->GetWidth();

        btn = new ButtonPanelTab("GENERIC", this);
        btn->SetPosition(btnX, btnY);
        mButtonsStructures->AddButton(btn);

        mButtonsStructures->SetFunctionOnToggle([this](unsigned int ind, bool checked)
        {
            if(!checked)
                return ;

            const unsigned int NUM_CAT = 4;

            if(ind >= NUM_CAT)
                return ;

            const ObjFamily categories[NUM_CAT] =
            {
                OCAT_RESOURCES,
                OCAT_DEFENSE,
                OCAT_TECHNOLOGY,
                OCAT_GENERIC
            };

            ShowStructuresByFamily(categories[ind]);
        });

        mButtonsStructures->SetButtonChecked(0, true);
    }
    else
        UpdateSlots();

    PositionElements();

    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/dialog_open-01.ogg");
}

void DialogNewElement::CheckBuild()
{
    const int ind = GetSelectedIndex();
    CheckBuild(ind);
}

void DialogNewElement::SetFunctionOnBuild(const std::function<void()> & f)
{
    mBtnBuild->AddOnClickFunction(f);
}

void DialogNewElement::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

int DialogNewElement::GetSelectedIndex() const
{
    return mFirstElem + mSlots->GetIndexChecked();
}

GameObjectTypeId DialogNewElement::GetSelectedType() const
{
    return mTypes[GetSelectedIndex()];
}

void DialogNewElement::UpdateSlots()
{
    const int numData = mTypes.size();
    const int leftData = numData - mFirstElem;
    const int limitData = leftData < NUM_SLOTS ? leftData : NUM_SLOTS;

    auto tm = sgl::graphic::TextureManager::Instance();

    for(int i = 0; i < limitData; ++i)
    {
        ButtonSlot * slot = static_cast<ButtonSlot *>(mSlots->GetButton(i));
        slot->SetEnabled(true);

        const PlayerFaction f = mPlayer->GetFaction();
        const GameObjectTypeId t = mTypes[mFirstElem + i];
        const ObjectFactionData & data = mDataReg->GetFactionData(f, t);

        auto tex = tm->GetSprite(data.iconFile, data.iconTexId);
        slot->SetData(GameObject::TITLES.at(t).c_str(), tex);

        // check first
        slot->SetChecked(false);
    }

    mSlots->GetButton(0)->SetChecked(true);

    for(int i = limitData; i < NUM_SLOTS; ++i)
    {
        ButtonSlot * slot = static_cast<ButtonSlot *>(mSlots->GetButton(i));
        slot->ClearData();
        slot->SetEnabled(false);
    }
}

void DialogNewElement::ShowStructuresByFamily(ObjFamily fam)
 {
    // get data by category
    const std::vector<GameObjectTypeId> & structures = mPlayer->GetAvailableStructures();

    mTypes.clear();

    for(const GameObjectTypeId s : structures)
    {
        const ObjectBasicData & data = mDataReg->GetObjectData(s);

        if(data.objFamily == fam)
            mTypes.emplace_back(s);
    }

    // reset first element index
    mFirstElem = 0;

    // update slots
    UpdateSlots();
 }

void DialogNewElement::ShowData(int ind)
{
    assert(ind < mTypes.size());

    const GameObjectTypeId t = mTypes[ind];
    const PlayerFaction f = mPlayer->GetFaction();
    const ObjectBasicData & data = mDataReg->GetObjectData(t);
    const ObjectFactionData & fData = mDataReg->GetFactionData(f, t);

    // DESCRIPTION
    mDescription->SetText(GameObject::DESCRIPTIONS.at(t).c_str());

    // CLASS
    mCategory->SetText(ObjectBasicData::STR_CLASS[data.objClass]);

    // COSTS
    for(int i = 0; i < NUM_COSTS; ++i)
        mLabelsCost[i]->SetText(std::to_string(fData.costs[i]).c_str());

    // ATTRIBUTES
    const int numStats = fData.stats.size();
    int statsAdded = 0;

    for(int i = 0; i < numStats; ++i)
    {
        const int val = fData.stats[i];

        if(val > 0)
        {
            mVisAtt[statsAdded]->SetData(ObjectFactionData::STR_STAT[i], val);
            ++statsAdded;
        }
    }

    for(int i = statsAdded; i < NUM_VIS_ATT; ++i)
        mVisAtt[i]->ClearData();
}

void DialogNewElement::CheckBuild(int ind)
{
    const GameObjectTypeId t = mTypes[ind];
    const PlayerFaction f = mPlayer->GetFaction();
    const ObjectFactionData & fData = mDataReg->GetFactionData(f, t);

    const std::vector<int> & costs = fData.costs;

    const bool CAN_SPEND[NUM_COSTS] =
    {
        mPlayer->HasEnough(Player::ENERGY,  costs[RES_ENERGY]),
        mPlayer->HasEnough(Player::MATERIAL, costs[RES_MATERIAL1]),
        mPlayer->HasEnough(Player::DIAMONDS, costs[RES_DIAMONDS]),
        mPlayer->HasEnough(Player::BLOBS, costs[RES_BLOBS])
    };

    const bool allowed =  CAN_SPEND[0] && CAN_SPEND[1] && CAN_SPEND[2] && CAN_SPEND[3];

    // enable BUILD button
    mBtnBuild->SetEnabled(allowed);

    // set color of costs
    for(unsigned int i = 0; i < NUM_COSTS; ++i)
    {
        const unsigned int COLORS[] = { 0xe08585ff, 0x5cd666ff};
        const int ind = static_cast<int>(CAN_SPEND[i]);

        mLabelsCost[i]->SetColor(COLORS[ind]);
    }
}

void DialogNewElement::HandlePositionChanged()
{
    PositionElements();
}

void DialogNewElement::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    int y = y0;

    mBgTop->SetPosition(x0, y);

    y += mBgTop->GetHeight();
    mBgMid->SetPosition(x0, y);

    y += mBgMid->GetHeight();
    mBgBtm->SetPosition(x0, y);
}

} // namespace game
