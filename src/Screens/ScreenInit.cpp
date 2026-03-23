#include "Screens/ScreenInit.h"

#include "Game.h"
#include "GameConstants.h"
#include "States/StatesIds.h"
#include "Widgets/GameUIData.h"

#ifdef DEV_MODE
#include "GameTestData.h"
#endif

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
    auto fm = sgl::graphic::FontManager::Instance();

    // == SETUP JOBS ==
    if(firstInit)
    {
        // BACKGROUND
        mTexPackages[PACKAGE_IMGS_BACKGROUNDS_PERM] =
            new sgl::core::DataPackage("data/img/backgrounds-perm.bin");
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_BACKGROUNDS_PERM], "space_bg.jpg");

        // FONT
        fm->RegisterDataPackage(packageFontsGame);
        fm->RegisterFont(packageFontsGame, "Lato-Regular.ttf");

        // SETUP JOBS
        SetupFonts();
        SetupMusic();
        SetupSFX();
        SetupPermanentTextures();

        // NOTE always after creating the textures
        SetupMouseCursors();
    }

    SetupPregameTextures();

    // BACKGROUND
    auto tex = tm->GetTexture("space_bg.jpg");
    mBg = new sgl::graphic::Image(tex);

    // FINAL JOB - move to next screen
    // NOTE keep last
    mJobs.emplace_back([this]
    {
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);

        #ifdef DEV_MODE
        std::cout << "ScreenInit - LOADING TIME: " << mTimeLoad << " seconds" << std::endl;
        #endif
    });

    // STATUS LABEL
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
        am->CreateSound(packageSoundsGame, "game/upgrade_unlocked-01.ogg");
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
            { 0, 0, 525, 450 },
            { 526, 0, 30, 30 },
            { 526, 31, 24, 30 },
            { 526, 62, 20, 360 },
            // BUTTON UPDATES
            { 0, 451, 475, 35 },
            { 0, 487, 475, 35 },
            { 0, 523, 475, 35 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PREGAME], SpriteFileDialogChangelog, rects);

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
            // PROGRESS BAR
            { 862, 101, 52, 8 },
            { 862, 110, 50, 6 },
            { 862, 117, 50, 6 },
            { 862, 124, 50, 6 },
            { 862, 131, 50, 6 },
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
    // UI PERMANENT PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_PERM] = new sgl::core::DataPackage("data/img/UI/UI-perm.bin");
    });

    // ===== REGISTER TEXTURES =====
    // CURSOR
    mJobs.emplace_back([this, tm]
    {
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileCursor);
    });

    // SETTINGS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 800 },
            { 91, 0, 110, 800 },

            // COMBO BOX
            { 201, 0, 310, 38 },
            { 201, 39, 310, 38 },
            { 201, 78, 310, 38 },
            { 201, 117, 310, 38 },
            { 201, 156, 310, 38 },
            { 201, 195, 310, 38 },
            { 201, 234, 310, 38 },
            { 201, 273, 310, 38 },

            // CHECK BOX
            { 201, 312, 45, 39 },
            { 247, 312, 45, 39 },
            { 293, 312, 45, 39 },
            { 339, 312, 45, 39 },
            { 385, 312, 45, 39 },

            // SLIDER
            { 201, 352, 304, 18 },
            { 201, 371, 300, 14 },
            { 201, 386, 30, 30 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileDialogSettings, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileDialogSettingsExp);
    });

    // TOOLTIPS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // RESOURCE BAR TOOLTIP
            { 0, 0, 304, 214 },
            // GAME TOOLTIP
            { 305, 0, 50, 50 },
            { 305, 51, 50, 50 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileTooltips, rects);

        // EXPANDABLE TEXTURE
        const std::vector<sgl::core::Rectd> rectsExp
        {
            // GAME TOOLTIP
            { 0, 0, 10, 50 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileTooltipsExp, rectsExp);
    });

    // UI SHARED GRAPHICS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // ICONS WHITE RESOURCES 24x24
            { 0, 0, 24, 24 },
            { 25, 0, 24, 24 },
            { 50, 0, 24, 24 },
            { 75, 0, 24, 24 },
            { 100, 0, 24, 24 },
            { 125, 0, 24, 24 },
            // ICONS COLOR RESOURCES 24x24
            { 0, 25, 24, 24 },
            { 25, 25, 24, 24 },
            { 50, 25, 24, 24 },
            { 75, 25, 24, 24 },
            { 100, 25, 24, 24 },
            { 125, 25, 24, 24 },
            // ICONS WHITE RESOURCES 16x16
            { 150, 0, 16, 16 },
            { 167, 0, 16, 16 },
            { 184, 0, 16, 16 },
            { 201, 0, 16, 16 },
            { 218, 0, 16, 16 },
            { 235, 0, 16, 16 },
            // ICONS COLOR RESOURCES 16x16
            { 150, 17, 16, 16 },
            { 167, 17, 16, 16 },
            { 184, 17, 16, 16 },
            { 201, 17, 16, 16 },
            { 218, 17, 16, 16 },
            { 235, 17, 16, 16 },
            // WARNING ICON
            { 252, 0, 36, 32 },
            // BUTTON DIALOG CLOSE
            { 0, 50, 94, 40 },
            { 95, 50, 94, 40 },
            { 190, 50, 94, 40 },
            { 285, 50, 94, 40 },
            // SLIDER DIALOG
            { 0, 91, 204, 16 },
            { 0, 108, 200, 12 },
            { 205, 91, 24, 24 },
            // VALUE BAR
            { 230, 91, 154, 16 },
            { 230, 108, 150, 12 },
            { 230, 121, 154, 16 },
            // CHECKBOX
            { 289, 0, 24, 24 },
            { 314, 0, 24, 24 },
            { 363, 0, 24, 24 },
            // BUTTON DIALOG STANDARD
            { 0, 138, 300, 44 },
            { 0, 183, 300, 44 },
            { 0, 228, 300, 44 },
            { 0, 273, 300, 44 },
            // STARS BAR
            { 301, 138, 112, 16 },
            { 301, 155, 112, 16 },
            { 301, 172, 112, 16 },
            { 301, 189, 112, 16 },
            { 301, 206, 112, 16 },
            { 301, 223, 112, 16 },
            { 301, 240, 112, 16 },
            { 301, 257, 112, 16 },
            { 301, 274, 112, 16 },
            { 301, 291, 112, 16 },
            { 301, 308, 112, 16 },
            // BUTTON DIALOG OK
            { 414, 0, 220, 40 },
            { 414, 41, 220, 40 },
            { 414, 82, 220, 40 },
            { 414, 123, 220, 40 },
            // ATTRIBUTES PANEL
            { 0, 325, 400, 38 },
            { 0, 364, 400, 38 },
            // ATTRIBUTES BAR
            { 414, 164, 140, 18 },
            { 414, 183, 140, 18 },
            { 414, 202, 140, 18 },
            { 414, 221, 140, 18 },
            { 414, 240, 140, 18 },
            { 414, 259, 140, 18 },
            { 414, 278, 140, 18 },
            { 414, 297, 140, 18 },
            { 414, 316, 140, 18 },
            { 414, 335, 140, 18 },
            { 414, 354, 140, 18 },
            // BUTTON TAB
            { 555, 164, 260, 44 },
            { 555, 209, 260, 44 },
            { 555, 254, 260, 44 },
            { 555, 299, 260, 44 },
            { 555, 344, 260, 44 },
            // BUTTON DIALOG BACK
            { 635, 0, 94, 40 },
            { 730, 0, 94, 40 },
            { 635, 41, 94, 40 },
            { 730, 41, 94, 40 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_PERM], SpriteFileUIShared, rects);
    });
}

} // namespace game
