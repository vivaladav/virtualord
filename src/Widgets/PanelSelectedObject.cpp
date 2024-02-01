#include "Widgets/PanelSelectedObject.h"

#include "GameUIData.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/ObjectsDataRegistry.h"

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
    // BUTTON CLOSE
    const int btnCloseX = 5;
    const int btnCloseY = 5;
    mButtonClose = new ButtonClosePanel(this);
    mButtonClose->SetPosition(btnCloseX, btnCloseY);

    // IMAGE
    mImg = new sgui::Image(this);

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

    PositionElements();
}

void PanelSelectedObject::AddFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void PanelSelectedObject::SetObject(GameObject * obj)
{
    using namespace sgl;

    // object already set -> exit
    if(obj == mObj)
        return ;

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
        const int visX = (imgW - imgAreaW) / 2 + borderImgArea;
        const int visY = (imgH - imgAreaH) / 2 + borderImgArea;
        const int visW = imgAreaW - borderTot;
        const int visH = imgAreaH - borderTot;

        mImg->SetVisibleArea(visX, visY, visW, visH);
    }
    else
        mImg->ClearVisibleArea();
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

} // namespace game
