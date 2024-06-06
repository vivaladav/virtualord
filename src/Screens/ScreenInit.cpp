#include "Screens/ScreenInit.h"

#include "Game.h"
#include "GameData.h"
#include "GameTestData.h"
#include "States/StatesIds.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/DataPackage.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>

#include <cmath>
#include <sstream>

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

ScreenInit::ScreenInit(Game * game)
    : Screen(game)
{
    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    mTexPackages.assign(NUM_DATA_PACKAGES, nullptr);

    auto tm = sgl::graphic::TextureManager::Instance();

    // -- BACKGROUND --
    mTexPackages[PACKAGE_IMGS_BACKGROUNDS] = new sgl::core::DataPackage("data/img/backgrounds.bin");

    tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_BACKGROUNDS], "main_menu_bg.png");
    tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_BACKGROUNDS], "space_bg.jpg");

    auto tex = tm->GetTexture("space_bg.jpg");
    mBg = new sgl::graphic::Image(tex);

    // == SETUP JOBS ==
    SetupLoadPackages();
    SetupFonts();
    SetupMusic();
    SetupSFX();
    SetupTextures();

    // FINAL JOB - move to next screen
    // NOTE keep last
    mJobs.emplace_back([this]
    {
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);
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
}

ScreenInit::~ScreenInit()
{
    delete mBg;

    for(auto p : mTexPackages)
        delete p;

    sgl::sgui::Stage::Instance()->ClearWidgets();
}

void ScreenInit::Update(float update)
{
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

    std::ostringstream ss;
    ss << "LOADING... " << perc << "%";
    mLabelStatus->SetText(ss.str().c_str());

    // update position
    const int rendW = sgl::graphic::Renderer::Instance()->GetWidth();
    const int rendH = sgl::graphic::Renderer::Instance()->GetHeight();
    const int x = (rendW - mLabelStatus->GetWidth()) * 0.5f;
    const int y = rendH - (mLabelStatus->GetHeight() * 2.f);
    mLabelStatus->SetPosition(x, y);
}

void ScreenInit::SetupLoadPackages()
{
    auto am = sgl::media::AudioManager::Instance();

    // LOAD GAME PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_GAME] = new sgl::core::DataPackage("data/img/game.bin");
    });

    // LOAD TEST PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_TEST] = new sgl::core::DataPackage("data/img/test.bin");
    });

    // LOAD UI GAME PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_GAME] = new sgl::core::DataPackage("data/img/UI/UI-game.bin");
    });

    // LOAD UI OTHERS PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_OTHERS] = new sgl::core::DataPackage("data/img/UI/UI-others.bin");
    });

    // LOAD UI TUTORIAL PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_TUTORIAL] = new sgl::core::DataPackage("data/img/UI/UI-tutorial.bin");
    });

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

    // LOAD MUSIC TEST PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageMusicTest);
    });

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

    // LOAD SFX TEST PACKAGE
    mJobs.emplace_back([am]
    {
        am->RegisterDataPackage(packageSoundsTest);
    });
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

void ScreenInit::SetupMusic()
{
    auto am = sgl::media::AudioManager::Instance();

    // MENUS MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicMenus, "menus/menu_01.ogg");
    });

    // GAME MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicGame, "game/music_01.ogg");
        am->CreateMusic(packageMusicGame, "mission/music_01.ogg");
        am->CreateMusic(packageMusicGame, "mission/music_02.ogg");
    });

    // TEST MUSIC
    mJobs.emplace_back([this, am]
    {
        am->CreateMusic(packageMusicTest, "test/menu_01.ogg");

        am->CreateMusic(packageSoundsTest, "test/test.ogg");
    });
}

void ScreenInit::SetupSFX()
{
    auto am = sgl::media::AudioManager::Instance();

    // GAME SFX
    mJobs.emplace_back([this, am]
    {
        am->CreateSound(packageSoundsGame, "game/error_action_01.ogg");
    });

    // TEST SFX
    mJobs.emplace_back([this, am]
    {
        am->CreateSound(packageSoundsTest, "test/test.ogg");
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
    });
}

void ScreenInit::SetupTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // DIALOG EXIT
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 600, 400 },

            // BUTTON
            { 601, 0, 300, 44 },
            { 601, 45, 300, 44 },
            { 601, 90, 300, 44 },
            { 601, 135, 300, 44 },
            // BUTTON 2
            { 601, 180, 300, 44 },
            { 601, 225, 300, 44 },
            { 601, 270, 300, 44 },
            { 601, 315, 300, 44 },
            // BUTTON CLOSE
            { 902, 0, 95, 41 },
            { 902, 42, 95, 41 },
            { 902, 84, 95, 41 },
            { 902, 126, 95, 41 },

            // ICONS
            { 902, 168, 32, 24 },
            { 935, 168, 32, 24 },
            { 902, 193, 32, 24 },
            { 935, 193, 32, 24 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileDialogExit, rects);
    });

    // DIALOG EXPLORE TEMPLE
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 850, 500 },
            { 0, 501, 770, 2 },
            { 851, 0, 2, 270 },

            // BUTTON
            { 854, 0, 260, 44 },
            { 854, 45, 260, 44 },
            { 854, 90, 260, 44 },
            { 854, 135, 260, 44 },

            // ICONS
            { 854, 180, 24, 24 },
            { 879, 180, 24, 24 },
            { 904, 180, 24, 24 },
            { 929, 180, 24, 24 },

            // SLIDER
            { 854, 205, 204, 16 },
            { 854, 222, 200, 12 },
            { 1059, 205, 24, 24 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogExploreTemple, rects);
    });

    // DIALOG OBJECT
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // DIALOG
            { 0, 0, 860, 525 },
            { 0, 526, 400, 38 },
            { 401, 526, 400, 38 },

            // ATTRIBUTES BAR
            { 861, 0, 140, 18 },
            { 861, 19, 140, 18 },
            { 861, 38, 140, 18 },
            { 861, 57, 140, 18 },
            { 861, 76, 140, 18 },
            { 861, 95, 140, 18 },
            { 861, 114, 140, 18 },
            { 861, 133, 140, 18 },
            { 861, 152, 140, 18 },
            { 861, 171, 140, 18 },
            { 861, 190, 140, 18 },

            // STARS BAR
            { 861, 209, 112, 16 },
            { 861, 226, 112, 16 },
            { 861, 243, 112, 16 },
            { 861, 260, 112, 16 },
            { 861, 277, 112, 16 },
            { 861, 294, 112, 16 },
            { 861, 311, 112, 16 },
            { 861, 328, 112, 16 },
            { 861, 345, 112, 16 },
            { 861, 362, 112, 16 },
            { 861, 379, 112, 16 },

            // BUTTON CLOSE
            { 861, 396, 105, 40 },
            { 861, 396, 105, 40 },
            { 861, 396, 105, 40 },

            // PROGRESS BAR
            { 0, 565, 154, 16 },
            { 155, 565, 150, 12 },
            { 155, 565, 150, 12 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogObject, rects);
    });

    // DIALOG TRADING
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // DIALOG
            { 0, 0, 1340, 560 },

            // BUTTON CLOSE
            { 0, 561, 100, 40 },
            { 101, 561, 100, 40 },
            { 202, 561, 100, 40 },

            // ICONS
            { 303, 561, 24, 24 },
            { 328, 561, 26, 26 },
            { 355, 561, 26, 26 },
            { 382, 561, 26, 26 },
            { 409, 561, 26, 26 },

            // BUTTON ACTION
            { 436, 561, 161, 40 },
            { 598, 561, 161, 40 },
            { 760, 561, 161, 40 },
            { 922, 561, 161, 40 },

            // BUTTON MINUS
            { 1341, 0, 40, 34 },
            { 1341, 35, 40, 34 },
            { 1341, 70, 40, 34 },
            { 1341, 105, 40, 34 },

            // BUTTON PLUS
            { 1341, 140, 40, 34 },
            { 1341, 175, 40, 34 },
            { 1341, 210, 40, 34 },
            { 1341, 245, 40, 34 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogTrading, rects);
    });

    // MAIN MENU
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // CHANGELOG DIALOG
            { 0, 0, 35, 450 },
            { 537, 0, 30, 30 },
            { 537, 31, 24, 30 },
            { 36, 0, 500, 450 },
            { 537, 62, 20, 360 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileMainMenu, rects);
    });

    // MAIN MENU BUTTONS
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // PRIMARY BUTTON
            { 0, 0, 360, 70 },
            { 0, 70, 360, 70 },
            { 0, 140, 360, 70 },
            { 0, 210, 360, 70 },
            { 0, 280, 360, 70 },
            // WISHLIST BUTTON
            { 0, 350, 285, 50 },
            { 0, 400, 285, 50 },
            { 0, 450, 285, 50 },
            // SOCIAL BUTTON
            { 285, 350, 90, 50 },
            { 285, 400, 90, 50 },
            { 285, 450, 90, 50 },
            // ICONS
            { 0, 500, 32, 24 },
            { 32, 500, 32, 24 },
            { 64, 500, 30, 24 },
            { 94, 500, 34, 24 },
            { 128, 500, 24, 24 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileMainMenuButtons, rects);
    });

    // FACTION SELECTION
    mJobs.emplace_back([this, tm]
    {
        std::vector<sgl::core::Rectd> rects
        {
            // PANELS
            { 0, 0, 1102, 970 },
            { 1103, 0, 300, 260 },
            { 1103, 261, 300, 260 },
            { 1103, 522, 300, 260 },
            { 1103, 783, 300, 110 },
            { 1404, 0, 300, 370 },
            // ATTRIBUTES BAR
            { 1103, 894, 187, 24 },
            { 1103, 918, 187, 24 },
            { 1103, 942, 187, 24 },
            { 1103, 966, 187, 24 },
            { 1103, 990, 187, 24 },
            { 1404, 819, 187, 24 },
            { 1404, 843, 187, 24 },
            { 1404, 867, 187, 24 },
            { 1404, 891, 187, 24 },
            { 1404, 915, 187, 24 },
            { 1404, 939, 187, 24 },
            // BACK BUTTON
            { 0, 971, 320, 50 },
            { 321, 971, 320, 50 },
            { 642, 971, 320, 50 },
            // SELECT BUTTON
            { 1404, 615, 250, 50 },
            { 1404, 666, 250, 50 },
            { 1404, 717, 250, 50 },
            { 1404, 768, 250, 50 },
            // CONTINUE BUTTON
            { 1404, 371, 300, 60 },
            { 1404, 432, 300, 60 },
            { 1404, 493, 300, 60 },
            { 1404, 554, 300, 60 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileFactionSelection, rects);
    });

    // CELLS
    mJobs.emplace_back([this, tm]
    {
        const int indRows = 4;
        const int indCols = 6;
        const int cellW = 96;
        const int cellH = 48;
        const int blockW = cellW + 1;
        const int blockH = cellH + 1;
        int x = 0;
        int y = 0;

        std::vector<sgl::core::Rectd> rects;
        rects.reserve(indRows * indCols);

        for(int r = 0; r < indRows; ++r)
        {
            for(int c = 0; c < indCols; ++c)
            {
                rects.emplace_back(x, y, cellW, cellH);

                x += blockW;
            }

            x = 0;
            y += blockH;
        }

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileCells, rects);
    });

    // MAP UI
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // MINIMAP
            { 0, 0, 340, 340 },
            // element
            { 341, 341, 10, 10 },
            // button close
            { 159, 341, 20, 20 },
            { 180, 341, 20, 20 },
            { 201, 341, 20, 20 },
            // button up
            { 341, 0, 60, 16 },
            { 341, 17, 60, 16 },
            { 341, 34, 60, 16 },
            { 341, 51, 60, 16 },
            // button down
            { 341, 68, 60, 16 },
            { 341, 85, 60, 16 },
            { 341, 102, 60, 16 },
            { 341, 119, 60, 16 },
            // button right
            { 341, 136, 16, 60 },
            { 357, 136, 16, 60 },
            { 341, 198, 16, 60 },
            { 357, 198, 16, 60 },
            // button left
            { 374, 136, 16, 60 },
            { 390, 136, 16, 60 },
            { 374, 198, 16, 60 },
            { 390, 198, 16, 60 },
            // camera corners
            { 224, 341, 14, 26 },
            { 239, 341, 14, 26 },
            { 254, 341, 26, 14 },
            { 254, 356, 26, 14 },
            // MINIMAP BUTTON
            { 0, 341, 52, 52 },
            { 53, 341, 52, 52 },
            { 106, 341, 52, 52 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelMinimap, rects);
    });

    // PANEL SELECTED OBJECT
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 340, 340 },
            // VALUE BAR
            { 0, 341, 154, 16 },
            { 155, 341, 150, 12 },
            { 341, 187, 154, 16 },
            // STARS BAR
            { 341, 0, 112, 16 },
            { 341, 17, 112, 16 },
            { 341, 34, 112, 16 },
            { 341, 51, 112, 16 },
            { 341, 68, 112, 16 },
            { 341, 85, 112, 16 },
            { 341, 102, 112, 16 },
            { 341, 119, 112, 16 },
            { 341, 136, 112, 16 },
            { 341, 153, 112, 16 },
            { 341, 170, 112, 16 },
            // ICONS
            { 454, 66, 22, 22 },
            { 477, 66, 22, 22 },
            { 500, 66, 22, 22 },
            // BUTTON PANEL
            { 341, 210, 52, 52 },
            { 394, 210, 52, 52 },
            { 448, 210, 52, 52 },
            // BUTTON CLOSE PANEL
            { 454, 89, 20, 20 },
            { 475, 89, 20, 20 },
            { 496, 89, 20, 20 },
            // BUTTON ACTION
            { 341, 263, 60, 40 },
            { 402, 263, 60, 40 },
            { 463, 263, 60, 40 },
            { 341, 304, 60, 40 },
            { 402, 304, 60, 40 },
            // ICONS ACTION
            { 454, 0, 32, 32 },
            { 487, 0, 32, 32 },
            { 454, 33, 32, 32 },
            { 454, 33, 32, 32 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelSelectedObject, rects);
    });

    // PANEL TURN CONTROL
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 788, 60 },
            // PROGRESS BAR
            { 0, 61, 404, 16 },
            { 0, 78, 400, 12 },
            { 0, 91, 400, 16 },
            // BUTTON
            { 405, 61, 60, 40 },
            { 466, 61, 60, 40 },
            { 527, 61, 60, 40 },
            { 588, 61, 60, 40 },
            // ICONS
            { 649, 61, 32, 32 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelTurnControl, rects);
    });

    // MAP UI
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // cell progress bar
            { 0, 0, 52, 8 },
            { 0, 9, 50, 6 },
            { 0, 16, 50, 6 },
            { 0, 23, 50, 6 },
            { 0, 30, 50, 6 },
            // icons cost
            { 0, 37, 8, 14 },
            { 9, 37, 11, 12 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileMapUI, rects);
    });

    // TUTORIAL
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // AREA CORNERS
            { 0, 0, 25, 25 },
            { 26, 0, 25, 25 },
            { 52, 0, 25, 25 },
            { 78, 0, 25, 25 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_TUTORIAL], SpriteFileTutorial, rects);
    });

    // TUTORIAL EXP
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // INFO PANEL
            { 0, 0, 20, 20 },
            { 21, 0, 20, 20 },
            { 42, 0, 20, 20 },
            { 63, 0, 20, 20 },
            { 84, 0, 20, 20 },
            { 105, 0, 20, 20 },
            { 126, 0, 20, 20 },
            { 147, 0, 20, 20 },
            { 168, 0, 20, 20 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_TUTORIAL], SpriteFileTutorialExp, rects);
    });

    // COLLECTIBLES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // diamonds
            { 0, 0, 96, 51 },
            { 96, 0, 96, 51 },
            { 192, 0, 96, 51 },
            { 288, 0, 96, 70 },

            // blobs
            { 0, 70, 96, 48 },
            { 96, 70, 96, 48 },
            { 192, 70, 96, 48 },
            { 288, 70, 96, 58 },

            // loot box
            { 385, 0, 96, 60 },
            { 385, 61, 96, 60 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteCollectiblesFile, rects);
    });

    // INDICATORS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // wall indicators
            { 0, 0, 96, 48 },
            { 97, 0, 96, 48 },
            { 194, 0, 96, 48 },
            { 291, 0, 96, 48 },
            { 388, 0, 96, 48 },
            { 485, 0, 96, 48 },
            { 0, 49, 96, 48 },
            { 97, 49, 96, 48 },
            { 194, 49, 96, 48 },
            { 291, 49, 96, 48 },
            { 388, 49, 96, 48 },
            { 485, 49, 96, 48 },
            { 0, 98, 96, 48 },
            { 97, 98, 96, 48 },
            { 194, 98, 96, 48 },
            { 291, 98, 96, 48 },
            { 388, 98, 96, 48 },
            { 485, 98, 96, 48 },
            { 0, 147, 96, 48 },
            // move indicator
            { 97, 147, 96, 48 },
            // conquest indicator
            { 194, 147, 96, 48 },
            { 291, 147, 96, 48 },
            { 388, 147, 96, 48 },
            // attack range indicator
            { 485, 147, 96, 48 },
            { 0, 196, 96, 48 },
            { 97, 196, 96, 48 },
            // HEALING RANGE INDICATOR
            { 582, 0, 96, 48 },
            { 582, 49, 96, 48 },
            { 582, 98, 96, 48 },
            // INFLUENCE AREA F1
            { 194, 200, 96, 48 },
            { 291, 200, 96, 48 },
            { 388, 200, 96, 48 },
            { 485, 200, 96, 48 },
            { 0, 249, 96, 48 },
            { 97, 249, 96, 48 },
            { 194, 249, 96, 48 },
            { 291, 249, 96, 48 },
            { 388, 249, 96, 48 },
            { 485, 249, 96, 48 },
            { 0, 298, 96, 48 },
            { 97, 298, 96, 48 },
            // INFLUENCE AREA F2
            { 194, 298, 96, 48 },
            { 291, 298, 96, 48 },
            { 388, 298, 96, 48 },
            { 485, 298, 96, 48 },
            { 0, 347, 96, 48 },
            { 97, 347, 96, 48 },
            { 194, 347, 96, 48 },
            { 291, 347, 96, 48 },
            { 388, 347, 96, 48 },
            { 485, 347, 96, 48 },
            { 0, 396, 96, 48 },
            { 97, 396, 96, 48 },
            // INFLUENCE AREA F3
            { 194, 396, 96, 48 },
            { 291, 396, 96, 48 },
            { 388, 396, 96, 48 },
            { 485, 396, 96, 48 },
            { 0, 445, 96, 48 },
            { 97, 445, 96, 48 },
            { 194, 445, 96, 48 },
            { 291, 445, 96, 48 },
            { 388, 445, 96, 48 },
            { 485, 445, 96, 48 },
            { 0, 494, 96, 48 },
            { 97, 494, 96, 48 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileIndicators, rects);
    });

    // PARTICLES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            { 0, 0, 4, 4 },

            // RESOURCE ICONS
            { 5, 0, 24, 24 },
            { 30, 0, 24, 24 },
            { 55, 0, 24, 24 },
            { 80, 0, 24, 24 },
            { 105, 0, 24, 24 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileParticles, rects);
    });

    // ROCKS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // row 0
            { 0, 0, 96, 51 },
            { 96, 0, 96, 54 },
            { 192, 0, 96, 54 },
            { 288, 0, 96, 51 },
            { 384, 0, 96, 56 },
            { 480, 0, 96, 55 },
            { 576, 0, 96, 58 },
            { 672, 0, 96, 56 },
            { 768, 0, 96, 55 },
            { 864, 0, 96, 58 },
            // row 1
            { 0, 58, 96, 60 },
            { 96, 58, 96, 57 },
            { 192, 58, 96, 53 },
            { 288, 58, 96, 60 },
            { 384, 58, 96, 59 },
            { 480, 58, 96, 59 },
            { 576, 58, 96, 59 },
            { 672, 58, 96, 59 },
            { 768, 58, 96, 59 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteRocksFile, rects);
    });

    // SCENE ELEMENTS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // mountains
            { 0, 0, 192, 134 },
            { 386, 0, 192, 108 },
            { 772, 0, 192, 131 },
            { 1158, 0, 192, 130 },
            { 0, 135, 192, 134 },
            { 386, 135, 192, 130 },
            { 772, 135, 192, 132 },
            { 1158, 135, 192, 134 },
            { 0, 270, 192, 132 },
            { 386, 270, 192, 134 },
            { 772, 270, 192, 131 },
            { 1158, 270, 192, 134 },
            // mountains selected
            { 193, 0, 192, 134 },
            { 579, 0, 192, 108 },
            { 965, 0, 192, 131 },
            { 1351, 0, 192, 130 },
            { 193, 135, 192, 134 },
            { 579, 135, 192, 130 },
            { 965, 135, 192, 132 },
            { 1351, 135, 192, 134 },
            { 193, 270, 192, 132 },
            { 579, 270, 192, 134 },
            { 965, 270, 192, 131 },
            { 1351, 270, 192, 134 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileSceneElements, rects);
    });

    // STRUCTURES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // base lvl 1
            { 0, 0, 288, 163 },
            { 289, 0, 288, 163 },
            { 576, 0, 288, 163 },
            { 865, 0, 288, 163 },
            { 0, 164, 288, 163 },
            { 289, 164, 288, 163 },
            { 576, 164, 288, 163 },
            { 865, 164, 288, 163 },
            // def tower lvl 1
            { 0, 328, 96, 65 },
            { 97, 328, 96, 65 },
            { 194, 328, 96, 65 },
            { 291, 328, 96, 65 },
            { 388, 328, 96, 65 },
            { 485, 328, 96, 65 },
            { 582, 328, 96, 65 },
            { 679, 328, 96, 65 },
            // def tower lvl 2
            { 0, 394, 96, 78 },
            { 97, 394, 96, 78 },
            { 194, 394, 96, 78 },
            { 291, 394, 96, 78 },
            { 388, 394, 96, 78 },
            { 485, 394, 96, 78 },
            { 582, 394, 96, 78 },
            { 679, 394, 96, 78 },
            // def tower lvl 3
            { 0, 473, 96, 84 },
            { 97, 473, 96, 84 },
            { 194, 473, 96, 84 },
            { 291, 473, 96, 84 },
            { 388, 473, 96, 84 },
            { 485, 473, 96, 84 },
            { 582, 473, 96, 84 },
            { 679, 473, 96, 84 },
            // practice target
            { 776, 1280, 96, 48 },
            { 776, 1329, 96, 48 },
            // radar
            { 0, 558, 192, 119 },
            { 193, 558, 192, 119 },
            { 386, 558, 192, 119 },
            { 579, 558, 192, 119 },
            { 0, 678, 192, 119 },
            { 193, 678, 192, 119 },
            { 386, 678, 192, 119 },
            { 579, 678, 192, 119 },
            // RADAR TOWER
            { 776, 328, 96, 96 },
            { 776, 425, 96, 96 },
            { 873, 328, 96, 96 },
            { 873, 425, 96, 96 },
            { 970, 328, 96, 96 },
            { 970, 425, 96, 96 },
            { 1067, 328, 96, 96 },
            { 1067, 425, 96, 96 },
            // solar panel
            { 776, 522, 96, 55 },
            { 776, 578, 96, 55 },
            { 873, 522, 96, 55 },
            { 873, 578, 96, 55 },
            { 970, 522, 96, 55 },
            { 970, 578, 96, 55 },
            { 1067, 522, 96, 55 },
            { 1067, 578, 96, 55 },
            // energy generator
            { 0, 798, 192, 136 },
            { 0, 935, 192, 136 },
            { 193, 798, 192, 136 },
            { 193, 935, 192, 136 },
            { 386, 798, 192, 136 },
            { 386, 935, 192, 136 },
            { 579, 798, 192, 136 },
            { 579, 935, 192, 136 },
            // material generator
            { 0, 1072, 192, 134 },
            { 0, 1207, 192, 134 },
            { 193, 1072, 192, 134 },
            { 193, 1207, 192, 134 },
            { 386, 1072, 192, 134 },
            { 386, 1207, 192, 134 },
            { 579, 1072, 192, 134 },
            { 579, 1207, 192, 134 },
            // storage energy
            { 776, 634, 96, 95 },
            { 776, 730, 96, 95 },
            { 873, 634, 96, 95 },
            { 873, 730, 96, 95 },
            { 970, 634, 96, 95 },
            { 970, 730, 96, 95 },
            { 1067, 634, 96, 95 },
            { 1067, 730, 96, 95 },
            // storage material
            { 776, 826, 96, 80 },
            { 776, 907, 96, 80 },
            { 873, 826, 96, 80 },
            { 873, 907, 96, 80 },
            { 970, 826, 96, 80 },
            { 970, 907, 96, 80 },
            { 1067, 826, 96, 80 },
            { 1067, 907, 96, 80 },
            // storage diamonds
            { 776, 988, 96, 88 },
            { 776, 1077, 96, 88 },
            { 873, 988, 96, 88 },
            { 873, 1077, 96, 88 },
            { 970, 988, 96, 88 },
            { 970, 1077, 96, 88 },
            { 1067, 988, 96, 88 },
            { 1067, 1077, 96, 88 },
            // storage blobs
            { 776, 1166, 96, 56 },
            { 776, 1223, 96, 56 },
            { 873, 1166, 96, 56 },
            { 873, 1223, 96, 56 },
            { 970, 1166, 96, 56 },
            { 970, 1223, 96, 56 },
            { 1067, 1166, 96, 56 },
            { 1067, 1223, 96, 56 },
            // material extractor
            { 0, 1342, 192, 96 },
            { 0, 1439, 192, 96 },
            { 193, 1342, 192, 96 },
            { 193, 1439, 192, 96 },
            { 386, 1342, 192, 96 },
            { 386, 1439, 192, 96 },
            { 579, 1342, 192, 96 },
            { 579, 1439, 192, 96 },
            // temple
            { 1164, 336, 288, 167 },
            { 1164, 504, 288, 167 },
            { 1164, 672, 288, 167 },
            { 1164, 840, 288, 167 },
            { 1164, 1008, 288, 167 },
            { 1164, 1176, 288, 167 },
            { 1164, 0, 288, 167 },
            { 1164, 168, 288, 167 },
            // BARRACKS
            { 1453, 0, 192, 96 },
            { 1453, 97, 192, 96 },
            { 1453, 194, 192, 96 },
            { 1453, 291, 192, 96 },
            { 1453, 388, 192, 96 },
            { 1453, 485, 192, 96 },
            { 1453, 582, 192, 96 },
            { 1453, 679, 192, 96 },
            // HOSPITAL
            { 1067, 1344, 192, 117 },
            { 1260, 1344, 192, 117 },
            { 1453, 775, 192, 117 },
            { 1453, 893, 192, 117 },
            { 1453, 1011, 192, 117 },
            { 1453, 1129, 192, 117 },
            { 1453, 1247, 192, 117 },
            { 1453, 1365, 192, 117 },
            // RESEARCH CENTER
            { 0, 1536, 192, 107 },
            { 193, 1536, 192, 107 },
            { 386, 1536, 192, 107 },
            { 579, 1536, 192, 107 },
            { 776, 1378, 192, 107 },
            { 776, 1486, 192, 107 },
            { 969, 1462, 192, 107 },
            { 1162, 1462, 192, 107 },
            // BUNKER
            { 1358, 1594, 96, 55 },
            { 1455, 1594, 96, 55 },
            { 776, 1594, 96, 55 },
            { 873, 1594, 96, 55 },
            { 970, 1594, 96, 55 },
            { 1067, 1594, 96, 55 },
            { 1164, 1594, 96, 55 },
            { 1261, 1594, 96, 55 },
            // TRADING POST
            { 1646, 0, 288, 144 },
            { 1646, 145, 288, 144 },
            { 1646, 290, 288, 144 },
            { 1646, 435, 288, 144 },
            { 1646, 580, 288, 144 },
            { 1646, 725, 288, 144 },
            { 1646, 870, 288, 144 },
            { 1646, 1015, 288, 144 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileStructures, rects);
    });

    // TREES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // TREE TYPE 1
            // 1 tree
            { 0, 0, 96, 48 },
            { 97, 0, 96, 51 },
            { 194, 0, 96, 52 },
            { 291, 0, 96, 54 },
            { 388, 0, 96, 57 },
            { 485, 0, 96, 59 },
            { 582, 0, 96, 62 },
            { 679, 0, 96, 65 },
            // 2 trees
            { 0, 66, 96, 63 },
            { 97, 66, 96, 60 },
            { 194, 66, 96, 55 },
            { 291, 66, 96, 62 },
            { 388, 66, 96, 61 },
            { 485, 66, 96, 61 },
            { 582, 66, 96, 60 },
            { 679, 66, 96, 58 },
            // 3 trees
            { 0, 130, 96, 62 },
            { 97, 130, 96, 65 },
            { 194, 130, 96, 65 },
            { 291, 130, 96, 68 },
            { 388, 130, 96, 68 },
            { 485, 130, 96, 58 },
            { 582, 130, 96, 63 },
            { 679, 130, 96, 60 },
            // 4 trees
            { 0, 199, 96, 65 },
            { 97, 199, 96, 61 },
            { 194, 199, 96, 67 },
            { 291, 199, 96, 69 },
            { 388, 199, 96, 67 },
            { 485, 199, 96, 66 },
            { 582, 199, 96, 66 },
            { 679, 199, 96, 63 },
            // TREE TYPE 1 - SELECTED
            // 1 tree
            { 0, 268, 96, 48 },
            { 97, 268, 96, 51 },
            { 194, 268, 96, 52 },
            { 291, 268, 96, 54 },
            { 388, 268, 96, 57 },
            { 485, 268, 96, 59 },
            { 582, 268, 96, 62 },
            { 679, 268, 96, 65 },
            // 2 trees
            { 0, 334, 96, 63 },
            { 97, 334, 96, 60 },
            { 194, 334, 96, 55 },
            { 291, 334, 96, 62 },
            { 388, 334, 96, 61 },
            { 485, 334, 96, 61 },
            { 582, 334, 96, 60 },
            { 679, 334, 96, 58 },
            // 3 trees
            { 0, 398, 96, 62 },
            { 97, 398, 96, 65 },
            { 194, 398, 96, 65 },
            { 291, 398, 96, 68 },
            { 388, 398, 96, 68 },
            { 485, 398, 96, 58 },
            { 582, 398, 96, 63 },
            { 679, 398, 96, 60 },
            // 4 trees
            { 0, 467, 96, 65 },
            { 97, 467, 96, 61 },
            { 194, 467, 96, 67 },
            { 291, 467, 96, 69 },
            { 388, 467, 96, 67 },
            { 485, 467, 96, 66 },
            { 582, 467, 96, 66 },
            { 679, 467, 96, 63 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileTrees, rects);
    });

    // UNITS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // unit 1
            { 0, 0, 96, 58 },
            { 97, 0, 96, 58 },
            { 194, 0, 96, 58 },
            { 291, 0, 96, 58 },
            { 388, 0, 96, 58 },
            { 485, 0, 96, 58 },
            // unit 2
            { 0, 58, 96, 54 },
            { 96, 58, 96, 54 },
            { 194, 58, 96, 54 },
            { 291, 58, 96, 54 },
            { 388, 58, 96, 54 },
            { 485, 58, 96, 54 },
            // unit 3
            { 0, 112, 96, 54 },
            { 96, 112, 96, 54 },
            { 194, 112, 96, 54 },
            { 291, 112, 96, 54 },
            { 388, 112, 96, 54 },
            { 485, 112, 96, 54 },
            // unit 4
            { 0, 169, 96, 54 },
            { 96, 169, 96, 54 },
            { 194, 169, 96, 54 },
            { 291, 169, 96, 54 },
            { 388, 169, 96, 54 },
            { 485, 169, 96, 54 },
            // unit 5
            { 0, 224, 96, 52 },
            { 96, 224, 96, 52 },
            { 194, 224, 96, 52 },
            { 291, 224, 96, 52 },
            { 388, 224, 96, 52 },
            { 485, 224, 96, 52 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileUnits, rects);
    });

    // UNIT PARTICLES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rectsUnitsPart
        {
            // laser
            { 0, 0, 10, 2 },
            { 0, 3, 10, 2 },
            { 0, 6, 10, 2 },
            // healing
            { 11, 0, 12, 12 },
            { 24, 0, 12, 12 },
            { 37, 0, 12, 12 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileUnitsParticles, rectsUnitsPart);
    });

    // WALLS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // wall level 1 faction 1
            { 0, 0, 96, 48 },
            { 96, 0, 96, 48 },
            { 192, 0, 96, 48 },
            { 288, 0, 96, 48 },
            { 384, 0, 96, 48 },
            { 480, 0, 96, 48 },
            { 576, 0, 96, 48 },
            { 672, 0, 96, 48 },
            { 768, 0, 96, 48 },
            { 864, 0, 96, 48 },
            { 960, 0, 96, 48 },
            // wall level 1 faction 2
            { 0, 48, 96, 48 },
            { 96, 48, 96, 48 },
            { 192, 48, 96, 48 },
            { 288, 48, 96, 48 },
            { 384, 48, 96, 48 },
            { 480, 48, 96, 48 },
            { 576, 48, 96, 48 },
            { 672, 48, 96, 48 },
            { 768, 48, 96, 48 },
            { 864, 48, 96, 48 },
            { 960, 48, 96, 48 },
            // wall level 1 faction 3
            { 0, 96, 96, 48 },
            { 96, 96, 96, 48 },
            { 192, 96, 96, 48 },
            { 288, 96, 96, 48 },
            { 384, 96, 96, 48 },
            { 480, 96, 96, 48 },
            { 576, 96, 96, 48 },
            { 672, 96, 96, 48 },
            { 768, 96, 96, 48 },
            { 864, 96, 96, 48 },
            { 960, 96, 96, 48 },
            // gate level 1 faction 1
            { 0, 144, 96, 48 },
            { 96, 144, 96, 48 },
            { 192, 144, 96, 48 },
            { 288, 144, 96, 48 },
            { 384, 144, 96, 48 },
            { 480, 144, 96, 48 },
            { 576, 144, 96, 48 },
            { 672, 144, 96, 48 },
            // gate level 1 faction 2
            { 0, 192, 96, 48 },
            { 96, 192, 96, 48 },
            { 192, 192, 96, 48 },
            { 288, 192, 96, 48 },
            { 384, 192, 96, 48 },
            { 480, 192, 96, 48 },
            { 576, 192, 96, 48 },
            { 672, 192, 96, 48 },
            // gate level 1 faction 3
            { 0, 240, 96, 48 },
            { 96, 240, 96, 48 },
            { 192, 240, 96, 48 },
            { 288, 240, 96, 48 },
            { 384, 240, 96, 48 },
            { 480, 240, 96, 48 },
            { 576, 240, 96, 48 },
            { 672, 240, 96, 48 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileWalls, rects);
    });

    // END MISSION DIALOG
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 800, 500 },

            // BUTTON
            { 0, 501, 300, 44 },
            { 301, 501, 300, 44 },
            { 0, 546, 300, 44 },
            { 301, 546, 300, 44 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogEndMission, rects);
    });

    // NEW ELEMENT DIALOG
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PANELS
            { 0, 0, 400, 38 },
            { 401, 0, 400, 38 },
            { 804, 0, 400, 260 },
            { 0, 40, 200, 200 },
            { 201, 40, 200, 200 },
            { 402, 40, 200, 200 },
            { 603, 40, 200, 200 },
            // ATTRIBUTES BAR
            { 0, 283, 140, 18 },
            { 141, 283, 140, 18 },
            { 282, 283, 140, 18 },
            { 423, 283, 140, 18 },
            { 564, 283, 140, 18 },
            { 705, 283, 140, 18 },
            { 0, 302, 140, 18 },
            { 141, 302, 140, 18 },
            { 282, 302, 140, 18 },
            { 423, 302, 140, 18 },
            { 564, 302, 140, 18 },
            // BUTTON CLOSE
            { 846, 261, 100, 40 },
            { 947, 261, 100, 40 },
            { 1048, 261, 100, 40 },
            // BUTTON BUILD
            { 0, 242, 200, 40 },
            { 201, 242, 200, 40 },
            { 402, 242, 200, 40 },
            { 603, 242, 200, 40 },
            // ICONS
            { 1205, 0, 16, 16 },
            { 1222, 0, 16, 16 },
            { 1239, 0, 16, 16 },
            { 1256, 0, 16, 16 },
            // BUTTON LEFT
            { 1205, 17, 20, 80 },
            { 1226, 17, 20, 80 },
            { 1247, 17, 20, 80 },
            { 1268, 17, 20, 80 },
            // BUTTON RIGHT
            { 1205, 98, 20, 80 },
            { 1226, 98, 20, 80 },
            { 1247, 98, 20, 80 },
            { 1268, 98, 20, 80 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewElement, rects);

        // NEW ELEMENT DIALOG - EXPANDABLE GRAPHICS
        const std::vector<sgl::core::Rectd> rectsExp
        {
            // BACKGROUND
            { 0, 0, 1280, 50 },
            { 0, 51, 1280, 10 },
            { 0, 62, 1280, 90 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewElementExp, rectsExp);
    });

    // OBJECT ACTION BUTTON
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BUTTON
            { 0, 0, 73, 73 },
            { 74, 0, 73, 73 },
            { 148, 0, 73, 73 },
            { 222, 0, 73, 73 },
            { 296, 0, 73, 73 },
            // ICONS
            { 0, 74, 52, 52 },
            { 53, 74, 52, 52 },
            { 106, 74, 52, 52 },
            { 159, 74, 52, 52 },
            { 212, 74, 52, 52 },
            { 265, 74, 52, 52 },
            { 0, 127, 52, 52 },
            { 53, 127, 52, 52 },
            { 106, 127, 52, 52 },
            { 159, 127, 52, 52 },
            { 212, 127, 52, 52 },
            { 265, 127, 52, 52 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileObjActionButton, rects);
    });

    // PLANET MAP
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PLANET
            { 0, 0, 800, 800 },
            { 801, 0, 84, 72 },
            { 886, 360, 84, 72 },
            { 801, 73, 84, 72 },
            { 801, 146, 84, 72 },
            { 801, 219, 84, 72 },
            { 801, 292, 84, 72 },
            { 801, 365, 84, 72 },
            { 801, 438, 84, 72 },
            { 801, 511, 84, 72 },
            { 801, 584, 84, 72 },
            { 801, 657, 84, 72 },
            // STARS BAR
            { 0, 801, 166, 28 },
            { 167, 801, 166, 28 },
            { 334, 801, 166, 28 },
            { 501, 801, 166, 28 },
            { 668, 801, 166, 28 },
            { 835, 801, 166, 28 },
            // PIPS BAR
            { 0, 830, 140, 18 },
            { 141, 830, 140, 18 },
            { 282, 830, 140, 18 },
            { 423, 830, 140, 18 },
            { 564, 830, 140, 18 },
            { 705, 830, 140, 18 },
            { 846, 830, 140, 18 },
            { 0, 849, 140, 18 },
            { 141, 849, 140, 18 },
            { 282, 849, 140, 18 },
            { 423, 849, 140, 18 },
            { 564, 849, 140, 18 },
            // BUTTON
            { 886, 0, 250, 44 },
            { 886, 45, 250, 44 },
            { 886, 90, 250, 44 },
            { 886, 135, 250, 44 },
            // BUTTON 2
            { 886, 180, 250, 44 },
            { 886, 225, 250, 44 },
            { 886, 270, 250, 44 },
            { 886, 315, 250, 44 },
            // ICONS
            { 885, 730, 32, 32 },
            { 918, 730, 32, 32 },
            { 951, 730, 32, 32 },
            { 984, 730, 32, 32 },
            { 1017, 730, 32, 32 },
            { 885, 763, 32, 32 },
            { 918, 763, 32, 32 },
            { 951, 763, 32, 32 },
            { 984, 763, 32, 32 },
            { 1017, 763, 32, 32 },
            // BUTTON LEAVE
            { 0, 868, 350, 44 },
            { 0, 913, 350, 44 },
            { 351, 868, 350, 44 },
            { 351, 913, 350, 44 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFilePlanetMap, rects);
    });

    // PLANET MAP 2
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PANELS
            { 0, 0, 400, 50 },
            { 401, 0, 400, 50 },
            { 0, 51, 475, 200 },
            { 0, 252, 475, 400 },
            { 476, 52, 475, 500 },
            { 476, 552, 475, 85 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFilePlanetMap2, rects);
    });

    // RESOURCES BAR
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BAR
            { 0, 0, 822, 50 },
            // MINI BAR
            { 0, 51, 54, 8 },
            { 0, 60, 50, 4 },
            // ICONS
            { 55, 51, 22, 32 },
            { 78, 51, 18, 32 },
            { 97, 51, 24, 26 },
            { 122, 51, 18, 30 },
            { 141, 51, 26, 26 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileResourcesBar, rects);
    });

    // TOOLTIPS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // RESOURCE BAR TOOLTIP
            { 0, 0, 250, 155 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileTooltips, rects);

        // EXPANDABLE TEXTURE
        const std::vector<sgl::core::Rectd> rectsExp
            {
                // SIMPLE TOOLTIP
                { 0, 0, 20, 32 },
                { 0, 33, 20, 32 },
                { 0, 66, 20, 32 },
            };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileTooltipsExp, rectsExp);
    });

    // QUICK UNIT SELECTION
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BUTTON BG
            { 0, 0, 197, 60 },
            { 198, 0, 197, 60 },
            { 0, 61, 197, 60 },
            { 198, 61, 197, 60 },
            { 0, 122, 197, 60 },
            // ICONS
            { 198, 122, 24, 24 },
            { 223, 122, 26, 26 },
            // UNITS
            { 396, 0, 24, 40 },
            { 421, 0, 40, 40 },
            { 462, 0, 40, 40 },
            { 503, 0, 32, 32 },
            { 536, 0, 49, 40 },
            { 396, 41, 24, 40 },
            { 421, 41, 40, 40 },
            { 462, 41, 40, 40 },
            { 503, 41, 32, 32 },
            { 536, 41, 49, 40 },
            { 396, 82, 24, 40 },
            { 421, 82, 40, 40 },
            { 462, 82, 40, 40 },
            { 503, 82, 32, 32 },
            { 536, 82, 49, 40 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileUnitQuickSel, rects);
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

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileSettings, rects);
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

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_OTHERS], SpriteFileSettingsExp, rects);
    });

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
            { 801, 101, 40, 40 }
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
}

} // namespace game
