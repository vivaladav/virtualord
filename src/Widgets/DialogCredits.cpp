#include "Widgets/DialogCredits.h"

#include "Game.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/MouseWheelEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/ScrollArea.h>
#include <sgl/sgui/Scrollbar.h>
#include <sgl/utilities/StringManager.h>

// anonymous namespace for local "private" classes
namespace
{
using namespace game;

constexpr int contW = 1004;
constexpr int contH = 640;
constexpr int contX = 30;
constexpr int contY = 20;

// ===== CHANGELOG SCROLLBAR =====
class CreditsScrollbar : public sgl::sgui::Scrollbar
{
public:
    CreditsScrollbar(sgl::sgui::Widget * parent)
        :  sgl::sgui::Scrollbar(sgl::sgui::AbstractSlider::VERTICAL, parent)
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();

        mBg->SetTexture(tm->GetSprite(SpriteFileDialogCredits, ID_DLG_CRE_SCROLL_BG));
        mButton->SetTexture(tm->GetSprite(SpriteFileDialogCredits, ID_DLG_CRE_SCROLL_BTN));

        UpdatePositions();
        UpdateGraphics(GetState());
    }

private:
    static const int BORDER = 1;

    void OnStateChanged(sgl::sgui::AbstractSlider::VisualState state) override
    {
        UpdateGraphics(state);
    }

    void HandlePositionChanged() override
    {
        UpdatePositions();
    }

    void HandleValueChanged(int) override
    {
        UpdatePositions();
    }

    void UpdateGraphics(sgl::sgui::AbstractSlider::VisualState state)
    {
        // set sliding area
        const int areaW = mBg->GetWidth() - (BORDER * 2);
        const int areaH = mBg->GetHeight() - (BORDER * 2) - mButton->GetHeight();
        SetSlidingAreaSize(areaW, areaH);

        // update widget size
        SetSize(mButton->GetWidth(), mBg->GetHeight());
    }

    void UpdatePositions()
    {
        const int x0 = GetScreenX();
        const int y0 = GetScreenY();

        // BACKGROUND
        const int bgX = x0 + (mButton->GetWidth() - mBg->GetWidth()) / 2;
        const int bgY = y0;
        mBg->SetPosition(bgX, bgY);

        // BUTTON
        const int btnX = x0;
        const int btnY = y0 + (GetValuePerc() * GetSlidingAreaHeight() / 100) + BORDER;
        mButton->SetPosition(btnX, btnY);
        SetSlidingAreaPosition(bgX + BORDER, bgY + BORDER);
    }
};

// ===== CHANGELOG AREA =====
class CreditsContentArea : public sgl::sgui::ScrollArea
{
public:
    CreditsContentArea(sgl::sgui::Widget * parent)
        : sgl::sgui::ScrollArea(1124, 680, parent)
        , mScrollbar(new CreditsScrollbar(this))
    {
        mScrollbar->AddOnValueChanged([this](int val)
                                      {
                                          sgl::sgui::Widget * cont = GetContent();
                                          cont->SetY(contH - val);
                                          cont->SetVisibleArea(0, val, contW, contH);
                                      });

        const int sbX = GetWidth() - mScrollbar->GetWidth();
        const int sbY = 0;
        mScrollbar->SetPosition(sbX, sbY);
    }

    void HandleMouseWheel(sgl::core::MouseWheelEvent & event) override
    {
        if(!mScrollbar->IsEnabled())
            return ;

        const int val = mScrollbar->GetValue();
        const int inc = 20;

        if(event.ScrollingUp())
            mScrollbar->SetValue(val - inc);
        else if(event.ScrollingDown())
            mScrollbar->SetValue(val + inc);
    }

private:
    void HandleNewContent() override
    {
        auto cont = GetContent();
        cont->SetPosition(contX, contY);
        cont->SetVisibleArea(0, 0, contW, contH);

        if(cont->GetHeight() <= contH)
            mScrollbar->SetEnabled(false);
        else
        {
            mScrollbar->SetEnabled(true);
            mScrollbar->SetMinMax(0, cont->GetHeight() - contH);
        }
    }

private:
    CreditsScrollbar * mScrollbar = nullptr;
};

// ===== CREDITS CONTENT =====
class CreditsContent : public sgl::sgui::Widget
{
public:
    CreditsContent()
    {
        using namespace sgl;

        SetSize(contW, contH);

        auto fm = graphic::FontManager::Instance();
        auto sm = utilities::StringManager::Instance();
        auto tm = graphic::TextureManager::Instance();

        const unsigned int colorHeader = 0xcceeffff;
        const unsigned int colorText = 0xa3daf5ff;
        const unsigned int colorText2 = 0x5caed6ff;

        const int marginHeaderB = 10;
        const int marginTeamB = 80;
        const int marginExternalB = 40;
        const int marginMemberR = 10;
        const int marginMemberB = 10;

        const int sizeHeader = 24;
        const int sizeText = 20;
        auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader,
                                      graphic::Font::NORMAL);
        auto fontText = fm->GetFont(WidgetsConstants::FontFileText, sizeText,
                                      graphic::Font::NORMAL);

        int contX = 0;
        int contY = 0;

        // -- A GAME BY --
        auto header = new sgui::Label(sm->GetCString("GAME_BY"), fontHeader, this);
        header->SetColor(colorHeader);

        contX = (contW - header->GetWidth()) / 2;
        header->SetPosition(contX, contY);

        contY += header->GetHeight() + marginHeaderB;

        // VOXELAY LOGO
        auto tex = tm->GetTexture(SpriteFileLogoVoxelay);
        auto imgLogo = new sgui::Image(tex, this);

        contX = (contW - imgLogo->GetWidth()) / 2;
        imgLogo->SetPosition(contX, contY);

        const int marginImageB = 10;
        contY += imgLogo->GetHeight() + marginImageB;

        // VOXELAY MEMBERS
        auto label = new sgui::Label("Davide \"vivaladav\" Coppola", fontText, this);
        label->SetColor(colorText);

        contX = (contW - label->GetWidth()) / 2;
        label->SetPosition(contX, contY);

        contY += label->GetHeight() + marginTeamB;

        // -- TESTING --
        header = new sgui::Label(sm->GetCString("TESTING"), fontHeader, this);
        header->SetColor(colorHeader);

        contX = 0;
        header->SetPosition(contX, contY);

        contY += header->GetHeight() + marginHeaderB;

        // TESTING MEMBERS
        label = new sgui::Label("\"Donutello\"", fontText, this);
        label->SetColor(colorText);
        label->SetPosition(contX, contY);

        contX += label->GetWidth() + marginMemberR;

        auto label2 = new sgui::Label("(alpha)", fontText, this);
        label2->SetColor(colorText2);
        label2->SetPosition(contX, contY);

        contX = 0;
        contY += label->GetHeight() + marginExternalB;

        // -- LOCALIZATION --
        header = new sgui::Label(sm->GetCString("LOCALIZATION"), fontHeader, this);
        header->SetColor(colorHeader);

        contX = 0;
        header->SetPosition(contX, contY);

        contY += header->GetHeight() + marginHeaderB;

        // LOCALIZATION MEMBER
        label = new sgui::Label("Pascal \"Hyperpas\" Biedermann", fontText, this);
        label->SetColor(colorText);
        label->SetPosition(contX, contY);

        contX += label->GetWidth() + marginMemberR;

        label2 = new sgui::Label("(Deutsch, Français)", fontText, this);
        label2->SetColor(colorText2);
        label2->SetPosition(contX, contY);

        // LOCALIZATION MEMBER
        contX = 0;
        contY += label->GetHeight() + marginMemberB;

        label = new sgui::Label("Felipe \"krtflp\" Costa", fontText, this);
        label->SetColor(colorText);
        label->SetPosition(contX, contY);

        contX += label->GetWidth() + marginMemberR;

        label2 = new sgui::Label("(Português - Brasil)", fontText, this);
        label2->SetColor(colorText2);
        label2->SetPosition(contX, contY);

        // LOCALIZATION MEMBER
        contX = 0;
        contY += label->GetHeight() + marginMemberB;

        label = new sgui::Label("Rosario \"Leureko\" Iovino", fontText, this);
        label->SetColor(colorText);
        label->SetPosition(contX, contY);

        contX += label->GetWidth() + marginMemberR;

        label2 = new sgui::Label("(Italiano)", fontText, this);
        label2->SetColor(colorText2);
        label2->SetPosition(contX, contY);
    }
};

} // namespace

namespace game
{

// ====== DIALOG CREDITS ======
DialogCredits::DialogCredits(Game * game)
    : mGame(game)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    const int w = 1202;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogCredits, ID_DLG_CRE_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogCredits, ID_DLG_CRE_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogCreditsExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogClose(this);
    mButtonClose->SetX(w - mButtonClose->GetWidth());

    mButtonClose->AddOnClickFunction([this]
        {
            mGame->SaveSettings();
        });

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    auto title = new sgui::Label(sm->GetCString("CREDITS"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // CONTENT AREA
    auto contArea = new CreditsContentArea(this);

    const int contX = 40;
    const int contY = 90;
    contArea->SetPosition(contX, contY);

    auto content = new CreditsContent;
    contArea->SetContent(content);
}

void DialogCredits::AddOnCloseClickedFunction(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void DialogCredits::HandlePositionChanged()
{
    SetPositions();
}

int DialogCredits::GetMouseButtonIndex(int buttonId) const
{
    return buttonId - 1;
}

void DialogCredits::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    // BACKGROUND
    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

} // namespace game
