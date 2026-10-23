#include "Widgets/ButtonSaveSlot.h"

#include "GameConstants.h"
#include "Planet.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <sstream>

namespace game
{

ButtonSaveSlot::ButtonSaveSlot(sgl::sgui::Widget * parent)
    : ButtonSaveSlot(GM_NULL, NO_FACTION, PLANET_UNKNOWN, 0, -1, 0, parent)
{
}

ButtonSaveSlot::ButtonSaveSlot(GameMode mode, PlayerFaction f, PlanetId planet, unsigned int map,
                               int turn, std::time_t timestamp, sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_NEWG_SLOT_NORMAL, ID_DLG_NEWG_SLOT_DISABLED,
                              ID_DLG_NEWG_SLOT_OVER, ID_DLG_NEWG_SLOT_NORMAL,
                              ID_DLG_NEWG_SLOT_CHECKED },
                             SpriteFileUIShared, parent)
    , mFaction(f)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int sizeMode = 20;
    auto fontMode = fm->GetFont(WidgetsConstants::FontFilePanelHeader, sizeMode,
                                graphic::Font::NORMAL);

    // used slot
    if(mode != GM_NULL)
    {
        // GAME MODE
        const char * strModes[] =
        {
            "FCAMPAIGN",
            "SMISSION",
            "ENDLESSM",
        };

        const int paddingH = 20;
        const int spacing = 4;
        const int modeX0 = paddingH;
        const int modeY0 = 10;

        mLabelMode = new sgui::Label(sm->GetCString(strModes[mode]), fontMode, this);
        mLabelMode->SetPosition(modeX0, modeY0);

        const int sizeData = 18;
        auto fontData = fm->GetFont(WidgetsConstants::FontFilePanelText, sizeData,
                                    graphic::Font::NORMAL);

        mLabelFaction = new sgui::Label(FACTIONS_NAME[f], fontData, this);
        mLabelFaction->SetPosition(GetWidth() - mLabelFaction->GetWidth() -  paddingH,
                                   modeY0 + (mLabelMode->GetHeight() - mLabelFaction->GetHeight()) / 2);

        mLabelPlanet = new sgui::Label(Planet::PLANETS_NAME[planet], fontData, this);
        mLabelPlanet->SetPosition(modeX0, modeY0 + mLabelMode->GetHeight() + spacing);

        if(turn > 0)
        {
            const int marginL = 15;
            std::ostringstream os;

            os << "(" << sm->GetCString("MISSION") << " " << (map + 1) << " #"
               << std::to_string(turn) << ")";
            mLabelTurn = new sgui::Label(os.str().c_str(), fontData, this);
            mLabelTurn->SetPosition(mLabelPlanet->GetX() + mLabelPlanet->GetWidth() + marginL,
                                    mLabelPlanet->GetY());
        }

        const unsigned int maxSize = 32;
        char strTimestamp[maxSize];
        std::strftime(strTimestamp, maxSize, "%Y-%m-%d %H:%M", std::localtime(&timestamp));

        mLabelTimestamp = new sgui::Label(strTimestamp, fontData, this);
        mLabelTimestamp->SetPosition(GetWidth() - mLabelTimestamp->GetWidth() -  paddingH,
                                     mLabelPlanet->GetY());
    }
    // empty slot
    else
    {
        mLabelMode = new sgui::Label(sm->GetCString("EMPTY_SLOT"), fontMode, this);
        mLabelMode->SetPosition((GetWidth() - mLabelMode->GetWidth()) / 2,
                                (GetHeight() - mLabelMode->GetHeight()) / 2);
    }

    UpdateColors(GetState());

    SetCheckable(true);
}

#ifdef DEMO_MODE
void ButtonSaveSlot::MarkNotAvailable()
{
    using namespace sgl;

    delete mLabelMode;

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int sizeMode = 20;
    auto fontMode = fm->GetFont(WidgetsConstants::FontFilePanelHeader, sizeMode,
                                graphic::Font::NORMAL);

    mLabelMode = new sgui::Label(sm->GetCString("NOT_IN_DEMO"), fontMode, this);
    mLabelMode->SetPosition((GetWidth() - mLabelMode->GetWidth()) / 2,
                            (GetHeight() - mLabelMode->GetHeight()) / 2);

    SetEnabled(false);
}
#endif

void ButtonSaveSlot::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::ImageButton::OnStateChanged(state);

    UpdateColors(state);
}

void ButtonSaveSlot::UpdateColors(sgl::sgui::AbstractButton::VisualState state)
{
    if(state == sgl::sgui::AbstractButton::DISABLED)
    {
        const unsigned int colorDisabled = 0x6d7073ff;
        mLabelMode->SetColor(colorDisabled);

        if(mLabelPlanet != nullptr)
            mLabelPlanet->SetColor(colorDisabled);

        if(mLabelTurn != nullptr)
            mLabelTurn->SetColor(colorDisabled);

        if(mLabelFaction != nullptr)
            mLabelFaction->SetColor(colorDisabled);

        if(mLabelTimestamp != nullptr)
            mLabelTimestamp->SetColor(colorDisabled);
    }
    else
    {
        mLabelMode->SetColor(0xe6eef2ff);

        if(mLabelPlanet != nullptr)
            mLabelPlanet->SetColor(WidgetsConstants::colorPanelText);

        if(mLabelTurn != nullptr)
            mLabelTurn->SetColor(WidgetsConstants::colorPanelText);

        if(mLabelFaction != nullptr)
            mLabelFaction->SetColor(PLAYER_COLOR[mFaction]);

        if(mLabelTimestamp != nullptr)
            mLabelTimestamp->SetColor(WidgetsConstants::colorPanelText);
    }
}

void ButtonSaveSlot::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonSaveSlot::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}
}