#include "Screens/ScreenInitGame.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
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
#include <chrono>
#include <iostream>
#endif

namespace game
{

ScreenInitGame::ScreenInitGame(Game * game)
    : Screen(game)
{
    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    mTexPackages.assign(NUM_DATA_PACKAGES, nullptr);

    auto tm = sgl::graphic::TextureManager::Instance();

    // -- BACKGROUND --
    auto tex = tm->GetTexture("space_bg.jpg");
    mBg = new sgl::graphic::Image(tex);

    // == SETUP JOBS ==
    SetupLoadPackages();
    SetupTextures();

    // FINAL JOB - move to next screen
    // NOTE keep last
    mJobs.emplace_back([this]
    {
        GetGame()->RequestNextActiveState(StateId::FACTION_SEL);

        #ifdef DEV_MODE
        std::cout << "ScreenInitGame - LOADING TIME: " << mTimeLoad << " seconds" << std::endl;
        #endif
    });

    // INIT STATUS LABEL
    auto fm = sgl::graphic::FontManager::Instance();
    sgl::graphic::Font * font = fm->GetFont("Lato-Regular.ttf", 32, sgl::graphic::Font::NORMAL);
    mLabelStatus = new sgl::sgui::Label(font);
    mLabelStatus->SetColor(0xEEEEEEFF);
    UpdateStatus();

    // SET DEFAULT FONT FOR SGUI
    sgl::sgui::Stage::Instance()->SetDefaultFont(font);

    // hide system mouse
    sgl::graphic::ModuleGraphic::HideSystemCursor();
}

ScreenInitGame::~ScreenInitGame()
{
    delete mBg;

    for(auto p : mTexPackages)
        delete p;

    sgl::sgui::Stage::Instance()->ClearWidgets();

    GetGame()->SetCurrentCursor(CURSOR_DEFAULT);
}

void ScreenInitGame::Update(float delta)
{
    mTimeLoad += delta;

    mJobs[mCurrJob]();

    ++mCurrJob;

    UpdateStatus();
}

void ScreenInitGame::Render()
{
    mBg->Render();
}

void ScreenInitGame::UpdateStatus()
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

void ScreenInitGame::SetupLoadPackages()
{
    // LOAD GAME PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_GAME] = new sgl::core::DataPackage("data/img/game.bin");
    });

    // LOAD UI GAME PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_GAME] = new sgl::core::DataPackage("data/img/UI/UI-game.bin");
    });

    // LOAD UI TUTORIAL PACKAGE
    mJobs.emplace_back([this]
    {
        mTexPackages[PACKAGE_IMGS_UI_TUTORIAL] = new sgl::core::DataPackage("data/img/UI/UI-tutorial.bin");
    });
}

void ScreenInitGame::SetupTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // ===== GAME =====

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

    // CITY
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // FACTION 1
            { 0, 0, 192, 109 },
            { 0, 110, 192, 111 },
            { 0, 222, 192, 123 },
            { 0, 346, 192, 119 },
            { 0, 466, 192, 119 },
            { 0, 586, 192, 136 },
            { 0, 723, 192, 118 },
            { 0, 842, 192, 119 },
            { 0, 962, 192, 120 },
            { 0, 1083, 192, 123 },
            { 0, 1207, 192, 128 },
            { 0, 1336, 192, 131 },
            { 0, 1468, 192, 118 },
            { 0, 1587, 192, 124 },
            { 0, 1712, 192, 122 },
            { 0, 1835, 192, 120 },
            // FACTION 1 - SELECTED
            { 193, 0, 192, 109 },
            { 193, 110, 192, 111 },
            { 193, 222, 192, 123 },
            { 193, 346, 192, 119 },
            { 193, 466, 192, 119 },
            { 193, 586, 192, 136 },
            { 193, 723, 192, 118 },
            { 193, 842, 192, 119 },
            { 193, 962, 192, 120 },
            { 193, 1083, 192, 123 },
            { 193, 1207, 192, 128 },
            { 193, 1336, 192, 131 },
            { 193, 1468, 192, 118 },
            { 193, 1587, 192, 124 },
            { 193, 1712, 192, 122 },
            { 193, 1835, 192, 120 },
            // FACTION 2
            { 386, 0, 192, 109 },
            { 386, 110, 192, 111 },
            { 386, 222, 192, 123 },
            { 386, 346, 192, 119 },
            { 386, 466, 192, 119 },
            { 386, 586, 192, 136 },
            { 386, 723, 192, 118 },
            { 386, 842, 192, 119 },
            { 386, 962, 192, 120 },
            { 386, 1083, 192, 123 },
            { 386, 1207, 192, 128 },
            { 386, 1336, 192, 131 },
            { 386, 1468, 192, 118 },
            { 386, 1587, 192, 124 },
            { 386, 1712, 192, 122 },
            { 386, 1835, 192, 120 },
            // FACTION 2 - SELECTED
            { 579, 0, 192, 109 },
            { 579, 110, 192, 111 },
            { 579, 222, 192, 123 },
            { 579, 346, 192, 119 },
            { 579, 466, 192, 119 },
            { 579, 586, 192, 136 },
            { 579, 723, 192, 118 },
            { 579, 842, 192, 119 },
            { 579, 962, 192, 120 },
            { 579, 1083, 192, 123 },
            { 579, 1207, 192, 128 },
            { 579, 1336, 192, 131 },
            { 579, 1468, 192, 118 },
            { 579, 1587, 192, 124 },
            { 579, 1712, 192, 122 },
            { 579, 1835, 192, 120 },
            // FACTION 3
            { 772, 0, 192, 109 },
            { 772, 110, 192, 111 },
            { 772, 222, 192, 123 },
            { 772, 346, 192, 119 },
            { 772, 466, 192, 119 },
            { 772, 586, 192, 136 },
            { 772, 723, 192, 118 },
            { 772, 842, 192, 119 },
            { 772, 962, 192, 120 },
            { 772, 1083, 192, 123 },
            { 772, 1207, 192, 128 },
            { 772, 1336, 192, 131 },
            { 772, 1468, 192, 118 },
            { 772, 1587, 192, 124 },
            { 772, 1712, 192, 122 },
            { 772, 1835, 192, 120 },
            // FACTION 3 - SELECTED
            { 965, 0, 192, 109 },
            { 965, 110, 192, 111 },
            { 965, 222, 192, 123 },
            { 965, 346, 192, 119 },
            { 965, 466, 192, 119 },
            { 965, 586, 192, 136 },
            { 965, 723, 192, 118 },
            { 965, 842, 192, 119 },
            { 965, 962, 192, 120 },
            { 965, 1083, 192, 123 },
            { 965, 1207, 192, 128 },
            { 965, 1336, 192, 131 },
            { 965, 1468, 192, 118 },
            { 965, 1587, 192, 124 },
            { 965, 1712, 192, 122 },
            { 965, 1835, 192, 120 },
            // NO FACTION
            { 1158, 0, 192, 109 },
            { 1158, 110, 192, 111 },
            { 1158, 222, 192, 123 },
            { 1158, 346, 192, 119 },
            { 1158, 466, 192, 119 },
            { 1158, 586, 192, 136 },
            { 1158, 723, 192, 118 },
            { 1158, 842, 192, 119 },
            { 1158, 962, 192, 120 },
            { 1158, 1083, 192, 123 },
            { 1158, 1207, 192, 128 },
            { 1158, 1336, 192, 131 },
            { 1158, 1468, 192, 118 },
            { 1158, 1587, 192, 124 },
            { 1158, 1712, 192, 122 },
            { 1158, 1835, 192, 120 },
            // NO FACTION - SELECTED
            { 1351, 0, 192, 109 },
            { 1351, 110, 192, 111 },
            { 1351, 222, 192, 123 },
            { 1351, 346, 192, 119 },
            { 1351, 466, 192, 119 },
            { 1351, 586, 192, 136 },
            { 1351, 723, 192, 118 },
            { 1351, 842, 192, 119 },
            { 1351, 962, 192, 120 },
            { 1351, 1083, 192, 123 },
            { 1351, 1207, 192, 128 },
            { 1351, 1336, 192, 131 },
            { 1351, 1468, 192, 118 },
            { 1351, 1587, 192, 124 },
            { 1351, 1712, 192, 122 },
            { 1351, 1835, 192, 120 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileCity, rects);
    });

    // COLLECTIBLES
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // diamonds
            { 0, 0, 96, 51 },
            { 0, 52, 96, 51 },
            { 0, 104, 96, 51 },
            { 0, 156, 96, 70 },

            // blobs
            { 97, 0, 96, 48 },
            { 97, 49, 96, 48 },
            { 97, 98, 96, 48 },
            { 97, 147, 96, 58 },

            // loot box
            { 194, 0, 96, 54 },
            { 194, 55, 96, 54 },
            // loot box lvl 2
            { 194, 110, 96, 54 },
            { 194, 165, 96, 54 },
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
            // conquest indicator
            { 97, 147, 96, 48 },
            { 194, 147, 96, 48 },
            { 291, 147, 96, 48 },
            // attack range indicator
            { 0, 196, 96, 48 },
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
            { 97, 494, 96, 48 },

            // MOVE PATH
            { 582, 147, 96, 48 },
            { 582, 196, 96, 48 },
            { 582, 245, 96, 48 },
            { 582, 294, 96, 48 },
            { 582, 343, 96, 48 },
            { 582, 392, 96, 48 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileMapIndicators, rects);
    });

    // MINI UNITS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // unit 1 - FACTION 1
            { 0, 0, 96, 48 },
            { 97, 0, 96, 48 },
            { 0, 49, 96, 48 },
            { 97, 49, 96, 48 },
            { 0, 98, 96, 48 },
            { 97, 98, 96, 48 },
            { 0, 147, 96, 54 },
            { 97, 147, 96, 54 },
            { 0, 202, 96, 54 },
            { 97, 202, 96, 54 },

            // unit 1 - FACTION 2
            { 194, 0, 96, 48 },
            { 291, 0, 96, 48 },
            { 194, 49, 96, 48 },
            { 291, 49, 96, 48 },
            { 194, 98, 96, 48 },
            { 291, 98, 96, 48 },
            { 194, 147, 96, 54 },
            { 291, 147, 96, 54 },
            { 194, 202, 96, 54 },
            { 291, 202, 96, 54 },

            // unit 1 - FACTION 3
            { 388, 0, 96, 48 },
            { 485, 0, 96, 48 },
            { 388, 49, 96, 48 },
            { 485, 49, 96, 48 },
            { 388, 98, 96, 48 },
            { 485, 98, 96, 48 },
            { 388, 147, 96, 54 },
            { 485, 147, 96, 54 },
            { 388, 202, 96, 54 },
            { 485, 202, 96, 54 },

            // unit 2 - FACTION 1
            { 0, 257, 96, 48 },
            { 97, 257, 96, 48 },
            { 0, 306, 96, 48 },
            { 97, 306, 96, 48 },
            { 0, 355, 96, 48 },
            { 97, 355, 96, 48 },
            { 0, 404, 96, 54 },
            { 97, 404, 96, 54 },
            { 0, 459, 96, 54 },
            { 97, 459, 96, 54 },

            // unit 2 - FACTION 2
            { 194, 257, 96, 48 },
            { 291, 257, 96, 48 },
            { 194, 306, 96, 48 },
            { 291, 306, 96, 48 },
            { 194, 355, 96, 48 },
            { 291, 355, 96, 48 },
            { 194, 404, 96, 54 },
            { 291, 404, 96, 54 },
            { 194, 459, 96, 54 },
            { 291, 459, 96, 54 },

            // unit 2 - FACTION 3
            { 388, 257, 96, 48 },
            { 485, 257, 96, 48 },
            { 388, 306, 96, 48 },
            { 485, 306, 96, 48 },
            { 388, 355, 96, 48 },
            { 485, 355, 96, 48 },
            { 388, 404, 96, 54 },
            { 485, 404, 96, 54 },
            { 388, 459, 96, 54 },
            { 485, 459, 96, 54 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME], SpriteFileMiniUnits, rects);
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
            { 130, 0, 24, 24 },

            // UPGRADE ICON
            { 155, 0, 24, 24 },
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
            // SPAWNING TOWER
            { 0, 1650, 192, 120 },
            { 193, 1650, 192, 120 },
            { 386, 1650, 192, 120 },
            { 579, 1650, 192, 120 },
            { 772, 1650, 192, 120 },
            { 965, 1650, 192, 120 },
            { 1158, 1650, 192, 120 },
            { 1351, 1650, 192, 120 },
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
            // unit 6
            { 0, 277, 96, 54 },
            { 96, 277, 96, 54 },
            { 194, 277, 96, 54 },
            { 291, 277, 96, 54 },
            { 388, 277, 96, 54 },
            { 485, 277, 96, 54 },
            // unit 7
            { 0, 332, 96, 61 },
            { 96, 332, 96, 61 },
            { 194, 332, 96, 61 },
            { 291, 332, 96, 61 },
            { 388, 332, 96, 61 },
            { 485, 332, 96, 61 },
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

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_GAME],
                           SpriteFileUnitsParticles, rectsUnitsPart);
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

    // ===== UI GAME =====
    // DIALOG EXIT
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 604, 70 },
            { 0, 71, 604, 90 },
            // BUTTON WISHLIST
            { 0, 161, 300, 44 },
            { 0, 206, 300, 44 },
            { 301, 206, 300, 44 },
            // ICONS
            { 301, 161, 32, 24 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogExit, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogExitExp);
    });

    // DIALOG EXPLORE TEMPLE
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 525 },
            { 91, 0, 110, 525 },
            { 201, 0, 90, 500 },
            { 293, 0, 90, 500 },

            // BUTTON
            { 0, 526, 300, 50 },
            { 0, 577, 300, 50 },
            { 0, 627, 300, 50 },
            { 0, 678, 300, 50 },

            // LAYOUT
            { 384, 0, 3, 285 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogExploreTemple, rects);

        const std::vector<sgl::core::Rectd> rectsExp
        {
             // BACKGROUND
             { 0, 0, 10, 525 },
             { 11, 0, 10, 500 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogExploreTempleExp, rectsExp);
    });

    // DIALOG MISSION GOALS
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUNDS
            { 0, 0, 1284, 70 },
            { 0, 71, 1284, 80 },

            // BUTTON COLLECT
            { 642, 152, 220, 44 },
            { 863, 152, 220, 44 },
            { 642, 197, 220, 44 },
            { 863, 197, 220, 44 },

            // BUTTON END MISSION
            { 0, 152, 320, 50 },
            { 321, 152, 320, 50 },
            { 0, 203, 320, 50 },
            { 321, 203, 320, 50 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogMissionGoals, rects);
    });

    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 1284, 10 },
            // PANELS ROW
            { 1285, 0, 10, 10 },
            { 1296, 0, 10, 10 },
            // SECTION HORIZONTAL LINE
            { 1307, 0, 10, 3 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogMissionGoalsExp, rects);
    });

    // DIALOG OBJECT
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 555 },
            { 91, 0, 110, 555 },
            // PANEL BACKGROUND
            { 202, 0, 10, 210 },
            { 202, 211, 10, 210 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogObject, rects);

        const std::vector<sgl::core::Rectd> rectsExp
        {
            // BACKGROUND
            { 0, 0, 10, 555 },
            { 11, 0, 10, 210 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogObjectExp, rectsExp);
    });

    // DIALOG RESEARCH
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 470 },
            { 91, 0, 110, 470 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogResearch, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                            SpriteFileDialogResearchExp);
    });

    // DIALOG TECH TREE
    mJobs.emplace_back([this, tm]
    {
#ifdef DEV_MODE
        // TODO remove later, now left just for reference on testing loading times
        auto t0 = std::chrono::high_resolution_clock::now();
#endif

        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 1002 },
            { 91, 0, 110, 1002 },
            // BUTTON SECTION
            { 202, 0, 300, 48 },
            { 202, 49, 300, 48 },
            { 202, 98, 300, 48 },
            { 202, 147, 300, 48 },
            // BUTTON UNLOCK
            { 202, 196, 300, 44 },
            { 202, 241, 300, 44 },
            { 202, 286, 300, 44 },
            { 202, 331, 300, 44 },
            // BUTTON UPGRADE
            { 202, 376, 96, 84 },
            { 299, 376, 96, 84 },
            { 396, 376, 96, 84 },
            { 202, 461, 96, 84 },
            { 299, 461, 96, 84 },
            { 352, 546, 20, 20 },
            // UPGRADE UNLOCKED
            { 396, 461, 96, 84 },
            // CONNECTIONS
            { 202, 546, 3, 64 },
            { 206, 546, 3, 107 },
            { 210, 546, 96, 3 },
            { 210, 550, 141, 3 },
            // ICONS UPGRADES
            { 202, 654, 48, 48 },
            { 251, 654, 48, 48 },
            { 300, 654, 48, 48 },
            { 349, 654, 48, 48 },
            { 398, 654, 48, 48 },
            { 447, 654, 48, 48 },
            { 202, 703, 48, 48 },
            { 251, 703, 48, 48 },
            { 300, 703, 48, 48 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
            SpriteFileDialogTechTree, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME],
            SpriteFileDialogTechTreeExp);

#ifdef DEV_MODE
        // TODO remove later, now left just for reference on testing loading times
        auto t1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
        std::cout << "ScreenInitGame::SetupTextures - DIALOG TECH TREE: " << duration.count() << " ms" << std::endl;
#endif
    });

    // DIALOG TRADING
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKROUNDS
            { 0, 0, 90, 630 },
            { 91, 0, 110, 630 },
            { 0, 631, 250, 40 },
            { 0, 672, 400, 40 },
            // BUTTON BUY
            { 202, 0, 200, 40 },
            { 202, 41, 200, 40 },
            { 202, 82, 200, 40 },
            { 202, 123, 200, 40 },
            // BUTTON SELL
            { 202, 164, 200, 40 },
            { 202, 205, 200, 40 },
            { 202, 246, 200, 40 },
            { 202, 287, 200, 40 },
            // BUTTON MINUS
            { 202, 328, 40, 34 },
            { 243, 328, 40, 34 },
            { 284, 328, 40, 34 },
            { 325, 328, 40, 34 },
            // BUTTON PLUS
            { 202, 363, 40, 34 },
            { 243, 363, 40, 34 },
            { 284, 363, 40, 34 },
            { 325, 363, 40, 34 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogTrading, rects);

        const std::vector<sgl::core::Rectd> rectsExp
        {
            // BACKROUNDS
            { 0, 0, 10, 630 },
            { 0, 631, 10, 10 },
            // LINE
            { 0, 642, 10, 3 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME],
                           SpriteFileDialogTradingExp, rectsExp);
    });

    // DIALOG UPGRADE
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // DIALOG
            { 0, 0, 90, 655 },
            { 91, 0, 360, 655 },
            { 0, 656, 600, 40 },
            { 0, 697, 600, 40 },
            { 452, 163, 300, 480 },
            // VALUE BAR
            { 601, 644, 14, 18 },
            { 601, 663, 14, 18 },
            { 601, 682, 14, 18 },
            // BUTTON -
            { 452, 0, 42, 30 },
            { 452, 31, 42, 30 },
            { 452, 62, 42, 30 },
            { 452, 93, 42, 30 },
            // BUTTON +
            { 616, 644, 42, 30 },
            { 659, 644, 42, 30 },
            { 616, 675, 42, 30 },
            { 659, 675, 42, 30 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogUpgrade, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogUpgradeExp);
    });

    // FACTION SELECTION
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 885 },
            { 91, 0, 365, 885 },
            { 0, 886, 450, 200 },
            { 0, 1087, 500, 50 },

            // BUTTON FACTION
            { 457, 0, 230, 200 },
            { 457, 201, 230, 200 },
            { 457, 402, 230, 200 },

            // LOGOS
            { 530, 603, 152, 170 },
            { 530, 774, 152, 170 },
            { 530, 945, 152, 170 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogFactionSelection, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogFactionSelectionExp);
    });

    // GAME UI
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // GOAL COMPLETED
            { 0, 0, 72, 72 },
            { 73, 0, 72, 72 },
            { 146, 0, 72, 72 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileGameUI, rects);

        const std::vector<sgl::core::Rectd> rectsExp
        {
            // SCREEN OVERLAY BG
            { 0, 0, 10, 10 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileGameUIExp, rectsExp);
    });

    // PANEL HIT
    mJobs.emplace_back([this, tm]
    {
        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelHit);
    });

    // MAP UI
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // MINIMAP
            { 0, 0, 342, 342 },
            // element
            { 343, 343, 10, 10 },
            // button close
            { 159, 343, 20, 20 },
            { 180, 343, 20, 20 },
            { 201, 343, 20, 20 },
            // button up
            { 343, 0, 60, 16 },
            { 343, 17, 60, 16 },
            { 343, 34, 60, 16 },
            { 343, 51, 60, 16 },
            // button down
            { 343, 68, 60, 16 },
            { 343, 85, 60, 16 },
            { 343, 102, 60, 16 },
            { 343, 119, 60, 16 },
            // button right
            { 343, 136, 16, 60 },
            { 359, 136, 16, 60 },
            { 343, 198, 16, 60 },
            { 359, 198, 16, 60 },
            // button left
            { 376, 136, 16, 60 },
            { 392, 136, 16, 60 },
            { 376, 198, 16, 60 },
            { 392, 198, 16, 60 },
            // camera corners
            { 222, 343, 14, 26 },
            { 237, 343, 14, 26 },
            { 252, 343, 26, 14 },
            { 252, 343, 26, 14 },
            // MINIMAP BUTTON
            { 0, 343, 52, 52 },
            { 53, 343, 52, 52 },
            { 106, 343, 52, 52 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelMinimap, rects);
    });

    // PANEL SELECTED OBJECT
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 342, 342 },
            // VALUE BAR
            { 0, 343, 154, 16 },
            { 155, 343, 150, 12 },
            { 306, 343, 154, 16 },
            // ICONS
            { 343, 66, 22, 22 },
            { 366, 66, 22, 22 },
            { 389, 66, 22, 22 },
            // BUTTON PANEL
            { 343, 110, 52, 52 },
            { 396, 110, 52, 52 },
            { 450, 110, 52, 52 },
            // BUTTON CLOSE PANEL
            { 343, 89, 20, 20 },
            { 364, 89, 20, 20 },
            { 385, 89, 20, 20 },
            // BUTTON ACTION
            { 343, 163, 60, 40 },
            { 404, 163, 60, 40 },
            { 343, 204, 60, 40 },
            { 404, 204, 60, 40 },
            { 343, 245, 60, 40 },
            // ICONS ACTION
            { 343, 0, 32, 32 },
            { 376, 0, 32, 32 },
            { 343, 33, 32, 32 },
            { 376, 33, 32, 32 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePanelSelectedObject, rects);
    });

    // PANEL TURN CONTROL
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 750, 62 },
            // PROGRESS BAR
            { 0, 63, 404, 16 },
            { 0, 80, 400, 12 },
            { 0, 93, 404, 16 },
            // BUTTON
            { 405, 63, 60, 40 },
            { 466, 63, 60, 40 },
            { 527, 63, 60, 40 },
            { 588, 63, 60, 40 },
            // ICONS
            { 649, 63, 32, 32 },
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

    // ===== UI TUTORIAL =====
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // AREA CORNERS
            { 0, 0, 25, 25 },
            { 26, 0, 25, 25 },
            { 52, 0, 25, 25 },
            { 78, 0, 25, 25 },
            // INFO PANEL CORNERS
            { 0, 26, 60, 60 },
            { 61, 26, 60, 60 },
            { 0, 87, 60, 60 },
            { 61, 87, 60, 60 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_TUTORIAL], SpriteFileTutorial, rects);

        // TUTORIAL EXP
        const std::vector<sgl::core::Rectd> rectsExp
        {
            // INFO PANEL SIDES AND CENTER
            { 0, 0, 10, 60 },
            { 11, 0, 10, 60 },
            { 0, 61, 60, 10 },
            { 0, 72, 60, 10 },
            { 22, 0, 10, 10 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_TUTORIAL], SpriteFileTutorialExp, rectsExp);
    });

    // END MISSION DIALOG
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUND
            { 0, 0, 90, 500 },
            { 91, 0, 90, 500 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogEndMission, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogEndMissionExp);
    });

    // NEW ELEMENT DIALOG
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUNDS
            { 0, 0, 90, 709 },
            { 91, 0, 360, 709 },
            { 452, 0, 90, 754 },
            { 543, 0, 360, 754 },
            { 904, 0, 200, 200 },
            { 904, 201, 200, 200 },
            { 904, 402, 200, 200 },
            { 904, 603, 200, 200 },
            { 0, 710, 400, 266 },
            // BUTTON LEFT
            { 400, 755, 20, 80 },
            { 421, 755, 20, 80 },
            { 442, 755, 20, 80 },
            { 463, 755, 20, 80 },
            // BUTTON RIGHT
            { 400, 836, 20, 80 },
            { 421, 836, 20, 80 },
            { 442, 836, 20, 80 },
            { 463, 836, 20, 80 }
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewElement, rects);

        // NEW ELEMENT DIALOG - EXPANDABLE GRAPHICS
        const std::vector<sgl::core::Rectd> rectsExp
        {
            // BACKGROUND
            { 0, 0, 10, 709 },
            { 11, 0, 10, 754 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewElementExp, rectsExp);
    });

    // NEW MINI UNITS SQUAD DIALOG
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BACKGROUNDS
            { 0, 0, 90, 615 },
            { 91, 0, 360, 615 },
            { 452, 0, 10, 200 },
            { 452, 201, 10, 200 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewMiniUnits, rects);

        const std::vector<sgl::core::Rectd> rectsExp
        {
            // BACKGROUNDS
            { 0, 0, 10, 615 },
            { 0, 616, 10, 200 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileDialogNewMiniUnitsExp, rectsExp);
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
            { 318, 74, 52, 52 },
            { 0, 127, 52, 52 },
            { 53, 127, 52, 52 },
            { 106, 127, 52, 52 },
            { 159, 127, 52, 52 },
            { 212, 127, 52, 52 },
            { 265, 127, 52, 52 },
            { 318, 127, 52, 52 },
            { 0, 180, 52, 52 },
            { 53, 180, 52, 52 },
            { 106, 180, 52, 52 },
            { 159, 180, 52, 52 },
            // BUTTON ACTION OPTION
            { 371, 0, 258, 40 },
            { 371, 41, 258, 40 },
            { 371, 82, 258, 40 },
            { 371, 123, 258, 40 },
            { 371, 164, 258, 40 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileObjActionButton, rects);
    });

    // PLANET MAP
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // PLANET
            { 0, 0, 700, 700 },
            // PLANET CELL
            { 701, 0, 72, 62 },
            { 701, 63, 72, 62 },
            { 701, 126, 72, 62 },
            { 701, 189, 72, 62 },
            { 701, 252, 72, 62 },
            { 701, 315, 72, 62 },
            { 701, 378, 72, 62 },
            { 701, 441, 72, 62 },
            { 701, 504, 72, 62 },
            { 701, 567, 72, 62 },
            { 701, 630, 72, 62 },
            // PLANET MAIN CELL
            { 774, 0, 96, 82 },
            { 774, 83, 96, 82 },
            { 774, 166, 96, 82 },
            { 774, 249, 96, 82 },
            { 774, 332, 96, 82 },
            { 774, 415, 96, 82 },
            { 871, 0, 96, 82 },
            { 871, 83, 96, 82 },
            { 871, 166, 96, 82 },
            { 871, 249, 96, 82 },
            { 871, 332, 96, 82 },
            // STARS BAR
            { 968, 360, 166, 28 },
            { 968, 389, 166, 28 },
            { 968, 418, 166, 28 },
            { 968, 447, 166, 28 },
            { 968, 476, 166, 28 },
            { 968, 505, 166, 28 },
            { 968, 534, 166, 28 },
            { 968, 563, 166, 28 },
            { 968, 592, 166, 28 },
            { 968, 621, 166, 28 },
            { 968, 650, 166, 28 },
            // PIPS BAR
            { 774, 498, 140, 18 },
            { 774, 517, 140, 18 },
            { 774, 536, 140, 18 },
            { 774, 555, 140, 18 },
            { 774, 574, 140, 18 },
            { 774, 593, 140, 18 },
            { 774, 612, 140, 18 },
            { 774, 631, 140, 18 },
            { 774, 650, 140, 18 },
            { 774, 669, 140, 18 },
            { 774, 688, 140, 18 },
            { 774, 707, 140, 18 },

            // BUTTON
            { 968, 0, 250, 44 },
            { 968, 45, 250, 44 },
            { 968, 90, 250, 44 },
            { 968, 135, 250, 44 },

            // BUTTON 2
            { 968, 180, 250, 44 },
            { 968, 225, 250, 44 },
            { 968, 270, 250, 44 },
            { 968, 315, 250, 44 },

            // ICONS
            { 968, 679, 32, 32 },
            { 1001, 679, 32, 32 },
            { 1034, 679, 32, 32 },
            { 1067, 679, 32, 32 },
            { 1100, 679, 32, 32 },
            { 968, 712, 32, 32 },
            { 1001, 712, 32, 32 },
            { 1034, 712, 32, 32 },
            { 1067, 712, 32, 32 },
            { 1100, 712, 32, 32 },

            // BUTTON LEAVE
            { 0, 701, 350, 44 },
            { 0, 746, 350, 44 },
            { 351, 701, 350, 44 },
            { 351, 746, 350, 44 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePlanetMap, rects);
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

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFilePlanetMap2, rects);
    });

    // RESOURCES BAR
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BAR
            { 0, 0, 75, 56 },
            { 76, 0, 75, 56 },
            // MINI BAR
            { 0, 57, 54, 8 },
            { 55, 57, 50, 4 },
            // ICONS
            { 152, 0, 32, 32 },
            { 185, 0, 32, 32 },
            { 218, 0, 32, 32 },
            { 152, 33, 32, 32 },
            { 185, 33, 32, 32 },
            { 218, 33, 32, 32 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileResourcesBar, rects);

        tm->RegisterTexture(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileResourcesBarExp);
    });

    // QUICK UNIT SELECTION
    mJobs.emplace_back([this, tm]
    {
        const std::vector<sgl::core::Rectd> rects
        {
            // BUTTON BG
            { 0, 0, 192, 60 },
            { 0, 61, 192, 60 },
            { 0, 122, 192, 60 },
            { 0, 183, 192, 60 },
            { 0, 244, 192, 60 },
            // ICONS
            { 268, 0, 24, 24 },
            { 293, 0, 24, 24 },
            // UNITS F1
            { 193, 0, 24, 40 },
            { 193, 41, 40, 40 },
            { 193, 82, 40, 40 },
            { 193, 123, 32, 32 },
            { 193, 156, 40, 33 },
            { 193, 190, 32, 32 },
            { 193, 223, 40, 40 },
            // UNITS F2
            { 218, 0, 24, 40 },
            { 234, 41, 40, 40 },
            { 234, 82, 40, 40 },
            { 225, 123, 32, 32 },
            { 234, 156, 40, 33 },
            { 226, 190, 32, 32 },
            { 234, 223, 40, 40 },
            // UNITS F3
            { 243, 0, 24, 40 },
            { 275, 41, 40, 40 },
            { 275, 82, 40, 40 },
            { 258, 123, 32, 32 },
            { 275, 156, 40, 33 },
            { 259, 190, 32, 32 },
            { 275, 223, 40, 40 },
        };

        tm->RegisterSprite(*mTexPackages[PACKAGE_IMGS_UI_GAME], SpriteFileUnitQuickSel, rects);
    });
}

} // namespace game
