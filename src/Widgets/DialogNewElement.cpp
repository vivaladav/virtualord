#include "Widgets/DialogNewElement.h"

#include "GameConstants.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Widgets/ButtonDialogArrows.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonDialogOk.h"
#include "Widgets/ButtonPanelTab.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ObjectVisualAttribute.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/DummyRenderable.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
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
#include <sgl/utilities/StringManager.h>

#include <cassert>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

constexpr int NUM_SLOTS = 6;

// ===== BUTTON SLOT =====

class ButtonSlot : public sgl::sgui::AbstractButton
{
public:
    ButtonSlot(int index)
        : mBody(new sgl::graphic::Image)
    {
        using namespace sgl;

        SetShortcutKey(KEYS[index]);

        SetCheckable(true);

        auto fm = graphic::FontManager::Instance();

        // TITLE
        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWEL_PAN_IMG_NORMAL);
        auto font = fm->GetFont(WidgetsConstants::FontFileButton, 16, graphic::Font::NORMAL);
        mTitle = new sgui::TextArea(tex->GetWidth(), TITLE_H, font, false, this);
        mTitle->setTextAlignment(sgui::TextArea::ALIGN_H_CENTER, sgui::TextArea::ALIGN_V_CENTER);

        // IMAGE
        mImage = new graphic::DummyRenderable;

        // SHORTCUT
        font = fm->GetFont(WidgetsConstants::FontFileShortcut, 12, graphic::Font::NORMAL);
        mShortcut = new graphic::Text(SHORTCUTS[index], font);
        mShortcut->SetColor(0xd5daddff);

        // register graphic elements
        RegisterRenderable(mBody);
        RegisterRenderable(mImage);
        RegisterRenderable(mShortcut);

        // set initial visual state
        InitState(NORMAL);
        UpdateGraphics(NORMAL);
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

        // check if image needs to be scaled to fit body
        const int bodyPad = 0;
        const int bodyW = mBody->GetWidth() - (bodyPad * 2);
        const int bodyH = mBody->GetHeight() - (bodyPad * 2);
        const int imgW = mImage->GetWidth();
        const int imgH = mImage->GetHeight();

        if(imgW > bodyW || imgH > bodyH)
        {
            int newW;
            int newH;

            if(imgW > imgH)
            {
                const float ratio = static_cast<float>(imgH) / static_cast<float>(imgW);

                newW = bodyW;
                newH = static_cast<int>(bodyW * ratio);
            }
            // imgH >= imgW
            else
            {
                const float ratio = static_cast<float>(imgW) / static_cast<float>(imgH);

                newW = static_cast<int>(bodyH * ratio);
                newH = bodyH;
            }

            mImage->SetWidth(newW);
            mImage->SetHeight(newH);
        }

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
        sgl::sgui::AbstractButton::OnStateChanged(state);

        UpdateGraphics(state);
    }

    void UpdateGraphics(sgl::sgui::AbstractButton::VisualState state)
    {
        const unsigned int texIds[NUM_VISUAL_STATES] =
        {
            ID_DLG_NEWEL_PAN_IMG_NORMAL,
            ID_DLG_NEWEL_PAN_IMG_DISABLED,
            ID_DLG_NEWEL_PAN_IMG_OVER,
            ID_DLG_NEWEL_PAN_IMG_NORMAL,
            ID_DLG_NEWEL_PAN_IMG_CHECKED,
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

} // namespace

namespace game
{

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
    auto sm = utilities::StringManager::Instance();

    const int paddingPanelH = 40;
    const int slotsX0 = WidgetsConstants::MarginDialogContentL + paddingPanelH;
    int slotsY0 = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT +
                  WidgetsConstants::PaddingPanelDialogT;

    if(ETYPE_UNITS_BARRACKS == type)
    {
        const std::vector<GameObjectTypeId> & allUnits = player->GetAvailableUnits();

        for(GameObjectTypeId t : allUnits)
        {
            const ObjectData & data = dataReg->GetObjectData(t);

            if(OCU_SOLDIER == data.GetClass())
                mTypes.push_back(t);
        }
    }
    else if(ETYPE_UNITS_BASE == type)
    {
        const std::vector<GameObjectTypeId> & allUnits = player->GetAvailableUnits();

        for(GameObjectTypeId t : allUnits)
        {
            const ObjectData & data = dataReg->GetObjectData(t);

            if(OCU_WORKER == data.GetClass() || OCU_SPAWNER == data.GetClass())
                mTypes.push_back(t);
        }
    }
    else if(ETYPE_UNITS_HOSPITAL == type)
    {
        const std::vector<GameObjectTypeId> & allUnits = player->GetAvailableUnits();

        for(GameObjectTypeId t : allUnits)
        {
            const ObjectData & data = dataReg->GetObjectData(t);

            if(OCU_MEDIC == data.GetClass())
                mTypes.push_back(t);
        }
    }

    // -- BACKGROUND --
    const int w = 1364;
    graphic::Texture * texL;
    graphic::Texture * texC;
    graphic::Texture * texR;

    if(type == ETYPE_STRUCTURES)
    {
        texL = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWE_S_BG_L);
        texR = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWE_S_BG_R);
        texC = tm->GetSprite(SpriteFileDialogNewElementExp, ID_DLG_NEWE_S_BG_C);
    }
    else
    {
        texL = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWE_U_BG_L);
        texR = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWE_U_BG_R);
        texC = tm->GetSprite(SpriteFileDialogNewElementExp, ID_DLG_NEWE_U_BG_C);
    }

    mBgL = new graphic::Image(texL);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    mBgR = new graphic::Image(texR);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    texC->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(texC);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // CLOSE BUTTON
    mBtnClose = new ButtonDialogClose(this);
    mBtnClose->SetX(GetWidth() - mBtnClose->GetWidth());

    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                            WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = nullptr;

    if(ETYPE_STRUCTURES == type)
        title = new sgui::Label(sm->GetCString("CREATE_NEW_STRUCTURE"), font, this);
    else
        title = new sgui::Label(sm->GetCString("CREATE_NEW_UNIT"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    // structures are grouped by categoy
    if(ETYPE_STRUCTURES == type)
    {
        mButtonsStructures = new sgl::sgui::ButtonsGroup(sgui::ButtonsGroup::HORIZONTAL, this);

        auto btn = new ButtonPanelTab(sm->GetCString("GENERIC_STRUCT"), this);
        mButtonsStructures->AddButton(btn);

        btn = new ButtonPanelTab(sm->GetCString("DEFENSE"), this);
        mButtonsStructures->AddButton(btn);

        btn = new ButtonPanelTab(sm->GetCString("RESOURCES"), this);
        mButtonsStructures->AddButton(btn);

        btn = new ButtonPanelTab(sm->GetCString("TECHNOLOGY"), this);
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
                                                            OCAT_GENERIC,
                                                            OCAT_DEFENSE,
                                                            OCAT_RESOURCES,
                                                            OCAT_TECHNOLOGY,
                                                        };

                                                    ShowStructuresByFamily(categories[ind]);
                                                });


        const int panelContentW = 1280;
        const int btnsX = WidgetsConstants::MarginDialogContentL +
                          (panelContentW - mButtonsStructures->GetWidth()) / 2;
        const int btnsY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;
        mButtonsStructures->SetPosition(btnsX, btnsY);

        slotsY0 += btn->GetHeight();
    }

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

    mSlots->SetPosition(slotsX0, slotsY0);

    const int marginButtonsLR = 10;

    const int numData = mTypes.size();

    mBtnLeft = new ButtonDialogArrowLeft(this);
    const int posLX = slotsX0 - mBtnLeft->GetWidth() - marginButtonsLR;
    const int posLY = slotsY0 + (mSlots->GetHeight() - mBtnLeft->GetHeight()) * 0.5f;
    mBtnLeft->SetPosition(posLX, posLY);
    mBtnLeft->SetEnabled(false);

    mBtnLeft->AddOnClickFunction([this]
    {
       mFirstElem -= NUM_SLOTS;
       UpdateSlots();

       mBtnLeft->SetEnabled(mFirstElem > 0);
       mBtnRight->SetEnabled(true);
    });

    mBtnRight = new ButtonDialogArrowRight(this);
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
    const int panelY0 = slotsY0 + mSlots->GetHeight();
    auto tex = tm->GetSprite(SpriteFileDialogNewElement, ID_DLG_NEWEL_PAN_INFO);
    auto panelInfo = new sgui::Image(tex, this);
    panelInfo->SetPosition(slotsX0, panelY0);

    const unsigned int colorHeader = 0xf1f2f4ff;
    const unsigned int colorText = 0xb6c0c9ff;

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, 18, sgl::graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 17, sgl::graphic::Font::NORMAL);

    const int marginPanelXY0 = 15;
    const int marginPanelBlock = 15;
    const int marginPanelIconH = 5;
    const int marginPanelIconV = 5;
    const int marginPanelIconBlock = 80;
    const int marginPanelDataV = 5;

    // header DESCRIPTION
    auto headerDesc = new sgui::Label(sm->GetCString("DESCRIPTION"), fontHeader, panelInfo);
    headerDesc->SetColor(colorHeader);
    headerDesc->SetPosition(marginPanelXY0, marginPanelXY0);

    int contY = headerDesc->GetY() + headerDesc->GetHeight() + marginPanelDataV;

    // description text area
    const int areaW = panelInfo->GetWidth() - (2 * marginPanelXY0);
    const int areaH = 80;
    mDescription = new sgui::TextArea(areaW, areaH, fontText, false, panelInfo);
    mDescription->SetColor(colorText);
    mDescription->SetPosition(marginPanelXY0, contY);

    contY += mDescription->GetHeight() + marginPanelBlock;

    // header category
    auto headerCat = new sgui::Label(sm->GetCString("CATEGORY"), fontHeader, panelInfo);
    headerCat->SetColor(colorHeader);
    headerCat->SetPosition(marginPanelXY0, contY);

    contY = headerCat->GetY() + headerCat->GetHeight() + marginPanelDataV;

    // text category
    mCategory = new sgui::Label("-", fontText, panelInfo);
    mCategory->SetColor(colorText);
    mCategory->SetPosition(marginPanelXY0, contY);

    contY += mCategory->GetHeight() + marginPanelBlock;

    // header cost
    headerCat = new sgui::Label(sm->GetCString("COST"), fontHeader, panelInfo);
    headerCat->SetColor(colorHeader);
    headerCat->SetPosition(marginPanelXY0, contY);

    contY = headerCat->GetY() + headerCat->GetHeight() + marginPanelDataV;

    // data cost 1
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_24);
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
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MATERIAL_24);
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
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_DIAMONDS_24);
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
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_BLOBS_24);
    imgIcon = new sgui::Image(tex, panelInfo);
    imgIcon->SetPosition(contX, contY);

    contX = imgIcon->GetX() + imgIcon->GetWidth() + marginPanelIconH;

    mLabelsCost[3] = new sgui::Label("-", fontText, panelInfo);
    mLabelsCost[3]->SetColor(colorText);
    contY = imgIcon->GetY() + (imgIcon->GetHeight() - mLabelsCost[3]->GetHeight()) * 0.5f;
    mLabelsCost[3]->SetPosition(contX, contY);

    // ATTRIBUTE PANELS
    const int panelsX0 = slotsX0 + panelInfo->GetWidth();
    const int panelsY0 = panelY0;
    int panelsX = panelsX0;
    int panelsY = panelsY0;

    for(int r = 0; r < VIS_ATT_ROWS; ++r)
    {
        const int ind0 = r * VIS_ATT_COLS;

        for(int c = 0; c < VIS_ATT_COLS; ++c)
        {
            const int ind = ind0 + c;

            auto panAtt = new ObjectVisualAttribute(this);
            panAtt->SetPosition(panelsX, panelsY);

            mVisAtt[ind] = panAtt;

            panelsX += panAtt->GetWidth();
        }

        panelsX = panelsX0;
        panelsY += mVisAtt[0]->GetHeight();
    }

    // BUTTON BUILD
    mBtnBuild = new ButtonDialogOk(sm->GetCString("BUILD"), this);
    const int btnX = GetWidth() - WidgetsConstants::MarginDialogContentR - mBtnBuild->GetWidth();
    const int marginButtonB = 20;
    const int btnY = GetHeight() - marginButtonB - mBtnBuild->GetHeight();
    mBtnBuild->SetPosition(btnX, btnY);

    if(type != ETYPE_STRUCTURES)
        UpdateSlots();
    else
        mButtonsStructures->SetButtonChecked(0, true);

    PositionElements();

    // play sound
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
    ap->PlaySound("UI/dialog_open-01.ogg");
}

DialogNewElement::~DialogNewElement()
{
    delete mButtonsStructures;
}

void DialogNewElement::SetFunctionOnBuild(const std::function<void()> & f)
{
    mBtnBuild->AddOnClickFunction(f);
}

void DialogNewElement::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

sgl::sgui::AbstractButton * DialogNewElement::GetButtonBuild() const
{
    return mBtnBuild;
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
    auto sm = sgl::utilities::StringManager::Instance();

    for(int i = 0; i < limitData; ++i)
    {
        ButtonSlot * slot = static_cast<ButtonSlot *>(mSlots->GetButton(i));
        slot->SetEnabled(true);

        const PlayerFaction f = mPlayer->GetFaction();
        const GameObjectTypeId t = mTypes[mFirstElem + i];
        const ObjectData & data = mDataReg->GetObjectData(t);

        auto tex = tm->GetSprite(data.GetIconTexFile(), data.GetIconTexId(f));
        slot->SetData(sm->GetCString(ObjectData::TITLES.at(t)), tex);

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
        const ObjectData & data = mDataReg->GetObjectData(s);

        if(data.GetFamily() == fam)
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
    const ObjectData & data = mDataReg->GetObjectData(t);

    auto sm = sgl::utilities::StringManager::Instance();

    // DESCRIPTION
    mDescription->SetText(sm->GetCString(ObjectData::DESCRIPTIONS.at(t)));

    // CLASS
    mCategory->SetText(sm->GetCString(ObjectData::STR_CLASS[data.GetClass()]));

    // COSTS
    const auto & costs = data.GetCosts();

    for(int i = 0; i < NUM_COSTS; ++i)
        mLabelsCost[i]->SetText(std::to_string(costs[i]).c_str());

    // ATTRIBUTES
    int attsAdded = 0;

    for(unsigned int i = 0; i < NUM_OBJ_ATTRIBUTES; ++i)
    {
        const int val = data.GetAttribute(static_cast<ObjAttId>(i));

        if(val > 0)
        {
            mVisAtt[attsAdded]->SetData(sm->GetCString(ObjectData::STR_ATTRIBUTES[i]), val);
            mVisAtt[attsAdded]->SetTooltipData(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[i]));

            ++attsAdded;
        }
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
                mVisAtt[attsAdded]->SetData(sm->GetCString(ObjectData::STR_ATTRIBUTES[attId]), val);
                mVisAtt[attsAdded]->SetTooltipData(sm->GetCString(ObjectData::STR_ATTRIBUTE_TOOLTIPS[attId]));

                ++attsAdded;
            }
        }
    }

    // clear remaining slots
    for(int i = attsAdded; i < NUM_VIS_ATT; ++i)
        mVisAtt[i]->ClearData();
}

void DialogNewElement::CheckBuild(int ind)
{
    const GameObjectTypeId t = mTypes[ind];
    const ObjectData & data = mDataReg->GetObjectData(t);

    const auto & costs = data.GetCosts();

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
    const int y = GetScreenY();
    int x = GetScreenX();

    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

} // namespace game
