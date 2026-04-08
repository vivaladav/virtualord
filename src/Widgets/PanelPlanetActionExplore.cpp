#include "Widgets/PanelPlanetActionExplore.h"

#include "Player.h"
#include "Widgets/ButtonPlanetMap.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/StringManager.h>

namespace
{
    constexpr int textSize = 18;
}

namespace game
{

PanelPlanetActionExplore::PanelPlanetActionExplore(Player * player, int money,
                                                   int energy, int material)
    : sgl::sgui::Widget(nullptr)
    , mPlayer(player)
    , mCostMoney(money)
    , mCostenergy(energy)
    , mCostmaterial(material)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_ACTIONS);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelTitle,
                                      WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mTitle = new sgui::Label(sm->GetCString("EXPLORE"), fnt, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);

    // CONTENT
    CreateContentStart(money, energy, material);
    CreateContentFailure();
    CreateContentSuccess();

    mContentFailure->SetVisible(false);
    mContentSuccess->SetVisible(false);

    // BUTTONS
    mButtonOk = new ButtonPlanetMap(this);
    mButtonOk->SetLabel(sm->GetCString("PROCEED"));

    mButtonCancel = new SecondaryButtonPlanetMap(this);
    mButtonCancel->SetLabel(sm->GetCString("CANCEL"));

    // position elements
    UpdatePositions();
}

void PanelPlanetActionExplore::ShowAction()
{
    auto sm = sgl::utilities::StringManager::Instance();

    // show content
    mContentStart->SetVisible(true);
    mContentFailure->SetVisible(false);
    mContentSuccess->SetVisible(false);

    // update costs
    const bool enoughMoney = mPlayer->HasEnough(Player::Stat::MONEY, mCostMoney);
    const bool enoughEnergy = mPlayer->HasEnough(Player::Stat::ENERGY, mCostenergy);
    const bool enoughMaterial = mPlayer->HasEnough(Player::Stat::MATERIAL, mCostmaterial);
    const bool canExplore = enoughMoney && enoughEnergy && enoughMaterial;

    mLabelMoney->SetColor(enoughMoney ? WidgetsConstants::colorPlanetMapEnoughRes :
                                        WidgetsConstants::colorPlanetMapLackRes);
    mLabelEnergy->SetColor(enoughEnergy ? WidgetsConstants::colorPlanetMapEnoughRes :
                                          WidgetsConstants::colorPlanetMapLackRes);
    mLabelMaterial->SetColor(enoughMaterial ? WidgetsConstants::colorPlanetMapEnoughRes :
                                              WidgetsConstants::colorPlanetMapLackRes);

    // update buttons
    mButtonOk->SetVisible(true);
    mButtonOk->SetEnabled(canExplore);

    mButtonCancel->SetLabel(sm->GetCString("CANCEL"));
}

void PanelPlanetActionExplore::ShowResult(bool success)
{
    auto sm = sgl::utilities::StringManager::Instance();

    mContentStart->SetVisible(false);

    mContentFailure->SetVisible(!success);
    mContentSuccess->SetVisible(success);

    mButtonOk->SetVisible(false);

    mButtonCancel->SetLabel(sm->GetCString("CLOSE"));
}

void PanelPlanetActionExplore::AddOnButtonOkClickFunction(const std::function<void()> & f)
{
    mButtonOk->AddOnClickFunction(f);
}

void PanelPlanetActionExplore::AddOnButtonCancelClickFunction(const std::function<void()> & f)
{
    mButtonCancel->AddOnClickFunction(f);
}

sgl::sgui::AbstractButton * PanelPlanetActionExplore::GetButtonOk() const
{
    return mButtonOk;
}

sgl::sgui::AbstractButton * PanelPlanetActionExplore::GetButtonCancel() const
{
    return mButtonCancel;
}

void PanelPlanetActionExplore::CreateContentStart(int money, int energy, int material)
{
    using namespace sgl;

    mContentStart = new sgui::Widget(this);

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int w = GetWidth();

    // DESCRIPTION
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileText, textSize, graphic::Font::NORMAL);

    const int marginL = 20;
    const int marginR = 20;
    const int contW = w - marginL - marginR;
    const int contH = 80;

    mTextDesc = new sgui::TextArea(contW, contH, sm->GetCString("PA_EXPLORE_DESC"),
                                   fnt, false, mContentStart);
    mTextDesc->SetColor(WidgetsConstants::colorPanelText);

    // COSTS
    auto contCosts = new sgui::Widget(mContentStart);

    const int valueBlockW = 130;
    const int spacingValue = 5;

    int x = 0;
    int y = 0;

    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_ICON_MONEY);
    auto icon = new sgui::Image(tex, contCosts);

    mLabelMoney = new sgui::Label(std::to_string(money).c_str(), fnt, contCosts);

    x = icon->GetWidth() + spacingValue;
    y = (icon->GetHeight() - mLabelMoney->GetHeight()) * 0.5f;
    mLabelMoney->SetPosition(x, y);

    x = valueBlockW;

    tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_ICON_ENERGY);
    icon = new sgui::Image(tex, contCosts);
    icon->SetX(x);

    mLabelEnergy = new sgui::Label(std::to_string(energy).c_str(), fnt, contCosts);

    x += icon->GetWidth() + spacingValue;
    y = (icon->GetHeight() - mLabelEnergy->GetHeight()) * 0.5f;
    mLabelEnergy->SetPosition(x, y);

    x = valueBlockW * 2;

    tex = tm->GetSprite(SpriteFilePlanetMap, IND_PM_ICON_MATERIAL);
    icon = new sgui::Image(tex, contCosts);
    icon->SetX(x);

    mLabelMaterial = new sgui::Label(std::to_string(material).c_str(), fnt, contCosts);

    x += icon->GetWidth() + spacingValue;
    y = (icon->GetHeight() - mLabelMaterial->GetHeight()) * 0.5f;
    mLabelMaterial->SetPosition(x, y);

    x = (w - contCosts->GetWidth()) * 0.5f - marginL;
    y = mTextDesc->GetHeight();
    contCosts->SetPosition(x, y);
}

void PanelPlanetActionExplore::CreateContentFailure()
{
    using namespace sgl;

    mContentFailure = new sgui::Widget(this);

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int w = GetWidth();

    // DESCRIPTION
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileText, textSize,
                                      graphic::Font::NORMAL);

    const int marginL = 20;
    const int marginR = 20;
    const int contW = w - marginL - marginR;
    const int contH = 100;

    mTextResFail = new sgui::TextArea(contW, contH, sm->GetCString("PA_EXPLORE_FAILED"),
                                      fnt, false, mContentFailure);
    mTextResFail->SetColor(WidgetsConstants::colorPanelText);
}

void PanelPlanetActionExplore::CreateContentSuccess()
{
    using namespace sgl;

    mContentSuccess = new sgui::Widget(this);

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int w = GetWidth();

    // DESCRIPTION
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileText, textSize,
                                      graphic::Font::NORMAL);

    const int marginL = 20;
    const int marginR = 20;
    const int contW = w - marginL - marginR;
    const int contH = 100;

    mTextResSuccess = new sgui::TextArea(contW, contH, sm->GetCString("PA_EXPLORE_SUCCESS"),
                                         fnt, false, mContentSuccess);
    mTextResSuccess->SetColor(WidgetsConstants::colorPanelText);
}

void PanelPlanetActionExplore::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelPlanetActionExplore::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = GetWidth();
    const int h = GetHeight();

    const int marginL = 20;
    const int marginT = 15;
    const int marginTextT = 20;

    int x;
    int y;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // TITLE
    x = marginL;
    y = marginT;

    mTitle->SetPosition(x, y);

    // CONTENT
    x = marginL;
    y = marginT + mTitle->GetHeight() + marginTextT;

    mContentStart->SetPosition(x, y);

    mContentFailure->SetPosition(x, y);
    mContentSuccess->SetPosition(x, y);

    // BUTTONS
    const int marginButtonsBottom = 50;
    const int spacingButtons = 20;

    x = (w - mButtonCancel->GetWidth()) * 0.5f;
    y = h - marginButtonsBottom - mButtonCancel->GetHeight();

    mButtonCancel->SetPosition(x, y);

    y -= spacingButtons + mButtonOk->GetHeight();

    mButtonOk->SetPosition(x, y);
}

void PanelPlanetActionExplore::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mTitle->SetText(sm->GetCString("EXPLORE"));
    mButtonOk->SetLabel(sm->GetCString("PROCEED"));

    if(mContentStart->IsVisible())
    {
        mButtonCancel->SetLabel(sm->GetCString("CANCEL"));
        mTextDesc->SetText(sm->GetCString("PA_EXPLORE_DESC"));
    }
    else
    {
        mButtonCancel->SetLabel(sm->GetCString("CLOSE"));

        if(mTextResFail != nullptr)
            mTextResFail->SetText(sm->GetCString("PA_EXPLORE_FAILED"));
        if(mTextResSuccess != nullptr)
            mTextResSuccess->SetText(sm->GetCString("PA_EXPLORE_SUCCESS"));
    }
}

} // namespace game
