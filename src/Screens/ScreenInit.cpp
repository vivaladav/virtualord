#include "Screens/ScreenInit.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameTestData.h"
#include "States/StatesIds.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/DataPackage.h>
#include <sgl/graphic/Cursor.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/ModuleGraphic.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

#include <cmath>
#include <sstream>

#ifdef DEV_MODE
#include <iostream>
#endif

namespace
{
const char * packageFontsGame = "data/fonts/game.bin";

const char * packageMusicGame = "data/music/game.bin";
const char * packageMusicMenus = "data/music/menus.bin";
const char * packageMusicTest = "data/music/test.bin";

const char * packageSoundsGame = "data/sfx/game.bin";
const char * packageSoundsTest = "data/sfx/test.bin";
const char * packageSoundsUI = "data/sfx/UI/UI.bin";
}

namespace game
{

ScreenInit::ScreenInit(Game * game, bool firstInit)
    : Screen(game)
{
    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    mTexPackages.assign(NUM_DATA_PACKAGES, nullptr);

    auto tm = sgl::graphic::TextureManager::Instance();

    // -- BACKGROUND --
    mTexPackages[PACKAGE_IMGS_BACKGROUNDS_SHARED] =
        new sgl::core::DataPackage("data/img/backgrounds-shared.bin");
    tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_BACKGROUNDS_SHARED], "space_bg.jpg");

    auto tex = tm->GetTexture("space_bg.jpg");
    mBg = new sgl::graphic::Image(tex);

    // == SETUP JOBS ==
    if(firstInit)
    {
        SetupFonts();
        SetupMusic();
        SetupSFX();
        SetupPermanentTextures();

        // NOTE always after creating the textures
        SetupMouseCursors();
    }

    SetupPregameTextures();

    // FINAL JOB - move to next screen
    // NOTE keep last
    mJobs.emplace_back([this]
    {
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);

        #ifdef DEV_MODE
        std::cout << "ScreenInit - LOADING TIME: " << mTimeLoad << " seconds" << std::endl;
        #endif
    });

    // INIT STATUS LABEL
    auto fm = sgl::graphic::FontManager::Instance();
    fm->RegisterDataPackage(packageFontsGame);
    fm->RegisterFont(packageFontsGame, "Lato-Regular.ttf");

    sgl::graphic::Font * font = fm->GetFont("Lato-Regular.ttf", 32, sgl::graphic::Font::NORMAL);
    mLabelStatus = new sgl::sgui::Label(font);
    mLabelStatus->SetColor(0xEEEEEEFF);
    UpdateStatus();

    // SET DEFAULT FONT FOR SGUI
    sgl::sgui::Stage::Instance()->SetDefaultFont(font);

    // hide system mouse
    sgl::graphic::ModuleGraphic::HideSystemCursor();
}

ScreenInit::~ScreenInit()
{
    delete mBg;

    for(auto p : mTexPackages)
        delete p;

    sgl::sgui::Stage::Instance()->ClearWidgets();

    GetGame()->SetCurrentCursor(CURSOR_DEFAULT);
}

void ScreenInit::Update(float delta)
{
    mTimeLoad += delta;

    mJobs[mCurrJob]();

    ++mCurrJob;

    UpdateStatus();
}

void ScreenInit::Render()
{
    mBg->Render();
}

void ScreenInit::UpdateStatus()
{
    // update status string
    const int perc = static_cast<int>(std::roundf(mCurrJob * 100.f / mJobs.size()));

    auto sm = sgl::utilities::StringManager::Instance();

    std::ostringstream ss;
    ss << sm->GetString("LOADING") <<"... " << perc << "%";
    mLabelStatus->SetText(ss.str().c_str());

    // update position
    const int rendW = sgl::graphic::Renderer::Instance()->GetWidth();
    const int rendH = sgl::graphic::Renderer::Instance()->GetHeight();
    const int x = (rendW - mLabelStatus->GetWidth()) * 0.5f;
    const int y = rendH - (mLabelStatus->GetHeight() * 2.f);
    mLabelStatus->SetPosition(x, y);
}

void ScreenInit::SetupFonts()
{
    // REGISTER FONTS
    auto fm = sgl::graphic::FontManager::Instance();

    mJobs.emplace_back([this, fm]
    {
        fm->RegisterFont(packageFontsGame, "Lato-Bold.ttf");
    });
}

void ScreenInit::SetupMouseCursors()
{
    mJobs.emplace_back([this]
    {
        using namespace sgl;

        auto tm = graphic::TextureManager::Instance();
        auto tex = tm->GetTexture(SpriteFileCursor);

        auto cursor = new graphic::Cursor(tex, 1, 1);

        GetGame()->RegisterCursor(CURSOR_DEFAULT, cursor);
    });
}

void ScreenInit::SetupMusic()
{
    auto am = sgl::media::AudioManager::Instance();

    // ===== INIT PACKAGES =====
    // LOAD MUSIC GAME PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageMusicGame);
    });

    // LOAD MUSIC MENUS PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageMusicMenus);
    });

#ifdef DEV_MODE
    // LOAD MUSIC TEST PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageMusicTest);
    });
#endif

    // ===== CREATE MUSIC =====
    // GAME MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicGame, "game/music_01.ogg");
        am->CreateMusic(packageMusicGame, "mission/music_01.ogg");
        am->CreateMusic(packageMusicGame, "mission/music_02.ogg");
    });

    // MENUS MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicMenus, "menus/menu_01.ogg");
    });

#ifdef DEV_MODE
    // TEST MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicTest, "test/menu_01.ogg");
        am->CreateMusic(packageSoundsTest, "test/test.ogg");
    });
#endif
}

void ScreenInit::SetupSFX()
{
    auto am = sgl::media::AudioManager::Instance();

    // ===== INIT PACKAGES =====
    // LOAD SFX GAME PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageSoundsGame);
    });

    // LOAD SFX UI PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageSoundsUI);
    });

#ifdef DEV_MODE
    // LOAD SFX TEST PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageSoundsTest);
    });
#endif

    // ===== CREATE SFX =====
    // GAME SFX
    mJobs.emplace_back([this, am]
    {
        am->CreateSound(packageSoundsGame, "game/build-01.ogg");
        am->CreateSound(packageSoundsGame, "game/build-02.ogg");
        am->CreateSound(packageSoundsGame, "game/build-03.ogg");
        am->CreateSound(packageSoundsGame, "game/build-04.ogg");
        am->CreateSound(packageSoundsGame, "game/collect-01.ogg");
        am->CreateSound(packageSoundsGame, "game/collect-02.ogg");
        am->CreateSound(packageSoundsGame, "game/collect-03.ogg");
        am->CreateSound(packageSoundsGame, "game/conquer-01.ogg");
        am->CreateSound(packageSoundsGame, "game/conquer-02.ogg");
        am->CreateSound(packageSoundsGame, "game/conquer-03.ogg");
        am->CreateSound(packageSoundsGame, "game/error_action_01.ogg");
        am->CreateSound(packageSoundsGame, "game/explosion-01.ogg");
        am->CreateSound(packageSoundsGame, "game/laser-01.ogg");
        am->CreateSound(packageSoundsGame, "game/miniunit_move-01.ogg");
        am->CreateSound(packageSoundsGame, "game/selection-01.ogg");
        am->CreateSound(packageSoundsGame, "game/selection-02.ogg");
        am->CreateSound(packageSoundsGame, "game/selection-03.ogg");
        am->CreateSound(packageSoundsGame, "game/unit_move-01.ogg");
        am->CreateSound(packageSoundsGame, "game/unit_move-02.ogg");
        am->CreateSound(packageSoundsGame, "game/upgrade-01.ogg");
        am->CreateSound(packageSoundsGame, "game/upgrade_notification-01.ogg");
    });

    // UI SFX
    mJobs.emplace_back([this, am]
    {
        am->CreateSound(packageSoundsUI, "UI/button_click_cancel-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click_cancel-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click_long-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click_long-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_click-03.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_over-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_over-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/button_over-03.ogg");
        am->CreateSound(packageSoundsUI, "UI/checkbox-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/checkbox-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/dialog_close-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/dialog_close-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/dialog_open-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/dialog_open-02.ogg");
        am->CreateSound(packageSoundsUI, "UI/goal_completed.ogg");
        am->CreateSound(packageSoundsUI, "UI/panel-close-01.ogg");
        am->CreateSound(packageSoundsUI, "UI/panel-open-01.ogg");
    });

#ifdef DEV_MODE
    // TEST SFX
    mJobs.emplace_back([this, am]
    {
        am->CreateSound(packageSoundsTest, "test/test.ogg");
    });
#endif
}

void ScreenInit::SetupPregameTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // ===== INIT PACKAGES =====
    // BACKGROUNDS PREGAME
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_BACKGROUNDS_PREGAME] =
            new sgl::core::DataPackage("data/img/backgrounds-pregame.bin");
    });

#ifdef DEV_MODE
    // TEST
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_TEST] = new sgl::core::DataPackage("data/img/test.bin");
    });
#endif

    // UI PREGAME
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_PREGAME] =
            new sgl::core::DataPackage("data/img/UI/UI-pregame.bin");
    });

    // ===== REGISTER TEXTURES - BACKGROUNDS PREGAME =====
    mJobs.emplace_back([this, tm]
    {
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_BACKGROUNDS_PREGAME], "main_menu_bg.png");
    });

    // ===== REGISTER TEXTURES - UI PREGAME =====
    // MAIN MENU
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // CHANGELOG DIALOG
            { 0, 0, 35, 450 },
            { 537, 0, 30, 30 },
            { 537, 31, 24, 30 },
            { 36, 0, 500, 450 },
            { 537, 62, 20, 360 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PREGAME], SpriteFileMainMenu, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_PREGAME], "UI/main_menu_build_badge.png");

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_PREGAME], "UI/main_menu_warning_bg.png");
    });

    // MAIN MENU BUTTONS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PRIMARY BUTTON
            { 0, 0, 360, 70 },
            { 0, 71, 360, 70 },
            { 0, 142, 360, 70 },
            { 0, 213, 360, 70 },
            { 0, 284, 360, 70 },
            // WISHLIST BUTTON
            { 0, 355, 285, 50 },
            { 0, 406, 285, 50 },
            { 0, 457, 285, 50 },
            // SOCIAL BUTTON
            { 286, 355, 90, 50 },
            { 286, 406, 90, 50 },
            { 286, 457, 90, 50 },
            // ICONS
            { 0, 508, 32, 24 },
            { 33, 508, 32, 24 },
            { 66, 508, 27, 24 },
            { 94, 508, 34, 24 },
            { 129, 508, 24, 24 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PREGAME], SpriteFileMainMenuButtons, rects);
    });

    // ===== REGISTER TEXTURES - TEST =====
#ifdef DEV_MODE
    // TEST SPRITE
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            { 0, 0, 40, 40 },
            { 40, 0, 40, 40 },
            { 0, 40, 40, 40 },
            { 40, 40, 40, 40 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_TEST], SpriteFileTestSprite, rects);
    });

    // TEST UI
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // COMBOBOX MAIN BUTTON BG
            { 0, 0, 200, 50 },
            { 200, 0, 200, 50 },
            { 400, 0, 200, 50 },
            { 600, 0, 200, 50 },
            { 800, 0, 200, 50 },
            // COMBOBOX ITEM BUTTON BG
            { 0, 50, 200, 50 },
            { 200, 50, 200, 50 },
            { 400, 50, 200, 50 },
            { 600, 50, 200, 50 },
            { 800, 50, 200, 50 },
            // SLIDER HORIZ
            { 0, 101, 304, 20 },
            { 305, 101, 300, 16 },
            { 606, 101, 30, 30 },
            // MULTI SELECTION
            { 637, 101, 40, 40 },
            { 678, 101, 40, 40 },
            { 719, 101, 40, 40 },
            { 760, 101, 40, 40 },
            { 801, 101, 40, 40 },
            // TEST CURSORS
            { 842, 101, 19, 26 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_TEST], SpriteFileTestUI, rects);
    });

    // TEST IMAGES
    mJobs.emplace_back([this, tm]
    {
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/obj_null.png");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/red_dot4.png");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/square100.png");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/test-bar-bg.png");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/test-bar-nobg.png");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], "test/text_area.png");

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_TEST], SpriteFileTestSprite);
    });
#endif
}

void ScreenInit::SetupPermanentTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // ===== INIT PACKAGES =====
    // UI SHARED PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_SHARED] = new sgl::core::DataPackage("data/img/UI/UI-shared.bin");
    });

    // ===== REGISTER TEXTURES =====
    // CURSOR
    mJobs.emplace_back([this, tm]
    {
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_SHARED], SpriteFileCursor);
    });

    // SETTINGS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PANELS
            { 0, 0, 1300, 800 },

            // BUTTON CLOSE
            { 0, 801, 100, 40 },
            { 101, 801, 100, 40 },
            { 202, 801, 100, 40 },
            { 303, 801, 100, 40 },

            // SECONDARY BUTTON
            { 404, 801, 250, 2 },

            // COMBO BOX
            { 0, 842, 300, 38 },
            { 301, 842, 300, 38 },
            { 602, 842, 300, 38 },
            { 903, 842, 300, 38 },
            { 0, 881, 300, 38 },
            { 301, 881, 300, 38 },
            { 602, 881, 300, 38 },
            { 903, 881, 300, 38 },

            // CHECK BOX
            { 655, 801, 45, 39 },
            { 701, 801, 45, 39 },
            { 747, 801, 45, 39 },
            { 793, 801, 45, 39 },
            { 839, 801, 45, 39 },
            { 885, 801, 45, 39 },
            { 931, 801, 45, 39 },
            { 977, 801, 45, 39 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_SHARED], SpriteFileSettings, rects);
    });

    // SETTINGS - EXPANDABLE TEXTURES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PANELS
            { 0, 0, 1200, 20 },
            { 0, 21, 1200, 10 },
            { 0, 32, 1200, 20 },

            // HORIZONTAL SLIDER
            { 0, 53, 304, 18 },
            { 305, 53, 300, 14 },
            { 606, 53, 30, 30 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_SHARED], SpriteFileSettingsExp, rects);
    });

    // TOOLTIPS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // RESOURCE BAR TOOLTIP
            { 0, 0, 250, 155 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_SHARED], SpriteFileTooltips, rects);

        // EXPANDABLE TEXTURE
        const std::vector<sgl::core::Rectd> rectsExp
        {
            // SIMPLE TOOLTIP
            { 0, 0, 20, 32 },
            { 0, 33, 20, 32 },
            { 0, 66, 20, 32 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_SHARED], SpriteFileTooltipsExp, rectsExp);
    });
}

} // namespace game
