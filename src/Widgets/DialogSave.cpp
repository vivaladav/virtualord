#include "Widgets/DialogSave.h"

#include "Game.h"
#include "Player.h"
#include "GameConstants.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/ButtonDialogOk.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

const int marginSide = 40;
const int marginButtonsB = 20;

DialogSave::DialogSave(Game * game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogSave, ID_DLG_SAVE_BG_TOP);
    mBgTop = new graphic::Image(tex);
    RegisterRenderable(mBgTop);

    tex = tm->GetSprite(SpriteFileDialogSave, ID_DLG_SAVE_BG_BOT);
    mBgBot = new graphic::Image(tex);
    RegisterRenderable(mBgBot);

    tex = tm->GetTexture(SpriteFileDialogSaveExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgMid = new graphic::Image(tex);
    RegisterRenderable(mBgMid);

    const int w = mBgTop->GetWidth();
    const int h = 400;

    SetSize(w, h);

    const int contentH = h - mBgTop->GetHeight() - mBgBot->GetHeight();
    mBgMid->SetHeight(contentH);

    // BUTTON CLOSE
    mBtnClose = new ButtonDialogClose(this);
    mBtnClose->SetX(w - mBtnClose->GetWidth());

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("SAVE_PROGRESS"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // -- CONTENT --
    const int marginTextB = 25;
    const int marginIconR = 10;
    const int marginButtonB = 50;

    auto font = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);

    const std::unordered_map<ExtendedResource, int> & cost = game->GetCostSaveGame();

    const auto player = game->GetLocalPlayer();

    // TEXT
    auto text = new sgui::Label(sm->GetCString("SAVE_COST"), font, this);
    text->SetColor(WidgetsConstants::colorDialogText);
    text->SetPosition(x, y);

    y += text->GetHeight() + marginTextB;

    // MONEY COST
    // MONEY
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MONEY_24);
    auto icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    const bool hasMoney = player->HasEnough(Player::MONEY, cost.at(ER_MONEY));

    text = new sgui::Label(std::to_string(cost.at(ER_MONEY)).c_str(), font, this);
    text->SetColor(hasMoney ? WidgetsConstants::colorDialogGood : WidgetsConstants::colorDialogBad);
    text->SetPosition(x, y);

    y += icon->GetHeight() + marginTextB;

    // ENERGY
    x = WidgetsConstants::MarginDialogContentL;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    const bool hasEnergy = player->HasEnough(Player::ENERGY, cost.at(ER_ENERGY));

    text = new sgui::Label(std::to_string(cost.at(ER_ENERGY)).c_str(), font, this);
    text->SetColor(hasEnergy ? WidgetsConstants::colorDialogGood : WidgetsConstants::colorDialogBad);
    text->SetPosition(x, y);

    y += icon->GetHeight() + marginTextB;

    // MATERIAL
    x = WidgetsConstants::MarginDialogContentL;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MATERIAL_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    const bool hasMaterial = player->HasEnough(Player::MATERIAL, cost.at(ER_MATERIAL));

    text = new sgui::Label(std::to_string(cost.at(ER_MATERIAL)).c_str(), font, this);
    text->SetColor(hasMaterial ? WidgetsConstants::colorDialogGood : WidgetsConstants::colorDialogBad);
    text->SetPosition(x, y);

    // BUTTON SAVE
    auto btnSave = new ButtonDialogOk(sm->GetCString("SAVE"), this);

    x = (w - btnSave->GetWidth()) / 2;
    y = h - marginButtonB - btnSave->GetHeight();
    btnSave->SetPosition(x, y);

    // check if local Player can afford to save
    const bool canSave = hasMoney && hasEnergy && hasMaterial;
    btnSave->SetEnabled(canSave);

    // handle click
    btnSave->AddOnClickFunction([this, game]
    {
        // pay costs
        const std::unordered_map<ExtendedResource, int> & cost = game->GetCostSaveGame();
        auto player = game->GetLocalPlayer();

        player->SumResource(Player::MONEY, -cost.at(ER_MONEY));
        player->SumResource(Player::ENERGY, -cost.at(ER_ENERGY));
        player->SumResource(Player::MATERIAL, -cost.at(ER_MATERIAL));

        // save game
        game->SaveGame();

        // close dialog
        mBtnClose->Click();
    });
}

void DialogSave::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogSave::HandlePositionChanged()
{
    SetPositions();
}

void DialogSave::SetPositions()
{
    const int x = GetScreenX();
    int y = GetScreenY();

    mBgTop->SetPosition(x, y);

    y += mBgTop->GetHeight();
    mBgMid->SetPosition(x, y);

    y += mBgMid->GetHeight();
    mBgBot->SetPosition(x, y);
}

} // namespace game
