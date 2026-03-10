#include "Widgets/DialogEndMission.h"

#include "Game.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON =====
class ButtonDialogEndMission : public GameButton
{
public:
    ButtonDialogEndMission(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileGameUIShared,
                     { ID_DLG_BTN_STD_NORMAL, ID_DLG_BTN_STD_DISABLED, ID_DLG_BTN_STD_OVER,
                       ID_DLG_BTN_STD_PUSHED, ID_DLG_BTN_STD_PUSHED },
                     { WidgetsConstants::colorDialogButtonNormal,
                      WidgetsConstants::colorDialogButtonDisabled,
                      WidgetsConstants::colorDialogButtonOver,
                      WidgetsConstants::colorDialogButtonPushed,
                      WidgetsConstants::colorDialogButtonChecked }, parent)
    {
        using namespace sgl;

        const int size = 24;

        auto fm = graphic::FontManager::Instance();
        auto sm = utilities::StringManager::Instance();

        auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(fnt);
        SetLabel(sm->GetCString("CONTINUE"));

        SetShortcutKey(sgl::core::KeyboardEvent::KEY_RETURN);
    }

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
        player->PlaySound("UI/button_click-01.ogg");
    }
};

} // namespace

namespace game
{

// ===== DIALOG =====
DialogEndMission::DialogEndMission(unsigned int time, unsigned int territoryConquered,
                                   unsigned int enemiesKilled, unsigned int casualties,
                                   unsigned int turns, bool victory)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogEndMission, IND_DIA_EM_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mButton = new ButtonDialogEndMission(this);

    const int buttonX = (w - mButton->GetWidth()) / 2;
    const int buttonY = h - 30 - mButton->GetHeight();
    mButton->SetPosition(buttonX, buttonY);

    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 32, graphic::Font::NORMAL);

    sgui::Label * title = nullptr;

    if(victory)
        title = new sgui::Label(sm->GetCString("VICTORY"), font, this);
    else
        title = new sgui::Label(sm->GetCString("DEFEAT"), font, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // -- CONTENT --
    const int limitR = 720;
    const int marginL = 40;
    const int marginT = 120;
    const int marginWidgetH = 30;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorData = 0x70a7c2ff;

    int widgetX = marginL;
    int widgetY = marginT;

    font = fm->GetFont(WidgetsConstants::FontFileText, 24, graphic::Font::NORMAL);

    // TIME DEPLOYED
    auto label = new sgui::Label(sm->GetCString("MISSION_TIME"), font, this);
    label->SetColor(colorHeader);
    label->SetPosition(widgetX, widgetY);

    const int secsInM = 60;
    const int secsInH = 60 * 60;
    const int timeH = time / secsInH;
    time -= timeH * secsInH;
    const int timeM = time / secsInM;
    time -= timeM * secsInM;
    const int timeS = time;

    const int fieldW = 2;
    const char fieldF = '0';
    std::ostringstream ss;
    ss.width(fieldW);
    ss.fill(fieldF);
    ss << timeH << ":";
    ss.width(fieldW);
    ss.fill(fieldF);
    ss << timeM << ":";
    ss.width(fieldW);
    ss.fill(fieldF);
    ss << timeS;

    label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);
    widgetX += limitR - label->GetWidth();
    label->SetPosition(widgetX, widgetY);

    widgetX = marginL;
    widgetY += marginWidgetH + label->GetHeight();

    ss.str(std::string());
    ss.clear();

    // TURNS PLAYED
    label = new sgui::Label(sm->GetCString("TURNS_PLAYED"), font, this);
    label->SetColor(colorHeader);
    label->SetPosition(widgetX, widgetY);

    ss << turns;

    label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);
    widgetX += limitR - label->GetWidth();
    label->SetPosition(widgetX, widgetY);

    widgetX = marginL;
    widgetY += marginWidgetH + label->GetHeight();

    ss.str(std::string());
    ss.clear();

    // TERRITORY CONQUERED
    label = new sgui::Label(sm->GetCString("TERRITORY_CONQUERED"), font, this);
    label->SetColor(colorHeader);
    label->SetPosition(widgetX, widgetY);

    ss << territoryConquered << "%";

    label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);
    widgetX += limitR - label->GetWidth();
    label->SetPosition(widgetX, widgetY);

    widgetX = marginL;
    widgetY += marginWidgetH + label->GetHeight();

    ss.str(std::string());
    ss.clear();

    // ENEMIES DESTROYED
    label = new sgui::Label(sm->GetCString("ENEMIES_DESTROYED"), font, this);
    label->SetColor(colorHeader);
    label->SetPosition(widgetX, widgetY);

    ss << enemiesKilled;

    label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);
    widgetX += limitR - label->GetWidth();
    label->SetPosition(widgetX, widgetY);

    widgetX = marginL;
    widgetY += marginWidgetH + label->GetHeight();

    ss.str(std::string());
    ss.clear();

    // CASUALTIES
    label = new sgui::Label(sm->GetCString("CASUALTIES"), font, this);
    label->SetColor(colorHeader);
    label->SetPosition(widgetX, widgetY);

    ss << casualties;

    label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);
    widgetX += limitR - label->GetWidth();
    label->SetPosition(widgetX, widgetY);

    widgetX = marginL;
    widgetY += marginWidgetH + label->GetHeight();
}

void DialogEndMission::SetFunctionOnClose(const std::function<void()> & f)
{
    mButton->AddOnClickFunction(f);
}

void DialogEndMission::HandlePositionChanged()
{
    SetPositions();
}

void DialogEndMission::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = mBg->GetWidth();

    // BACKGROUND
    mBg->SetPosition(x0, y0);
}

} // namespace game
