#include "Screens/ScreenInit.h"

#include "Game.h"
#include "GameConstants.h"
#include "ResourceLoader.h"
#include "States/StatesIds.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/ModuleGraphic.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

#include <cmath>
#include <sstream>

#ifdef DEV_MODE
#include <iostream>
#endif

namespace game
{

ScreenInit::ScreenInit(Game * game, bool firstInit)
    : Screen(game)
    , mResLoader(game->GetResourceLoader())
{
using namespace sgl;

    game->SetClearColor(0x12, 0x12, 0x12, 0xFF);

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();

    // == SETUP JOBS ==
    if(firstInit)
        mResLoader->LoadFirstInitData();

    mResLoader->LoadPreGameData();

    // BACKGROUND
    auto tex = tm->GetTexture("space_bg.jpg");
    mBg = new graphic::Image(tex);

    // STATUS LABEL
    graphic::Font * font = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);
    mLabelStatus = new sgui::Label(font);
    mLabelStatus->SetColor(0xEEEEEEFF);
    UpdateStatus();

    // SET DEFAULT FONT FOR SGUI
    sgui::Stage::Instance()->SetDefaultFont(font);

    // hide system mouse
    graphic::ModuleGraphic::HideSystemCursor();
}

ScreenInit::~ScreenInit()
{
    delete mBg;

    sgl::sgui::Stage::Instance()->ClearWidgets();

    GetGame()->SetCurrentCursor(CURSOR_DEFAULT);
}

void ScreenInit::Update(float delta)
{
    mTimeLoad += delta;

    mResLoader->Update(delta);

    UpdateStatus();
}

void ScreenInit::Render()
{
    mBg->Render();
}

void ScreenInit::UpdateStatus()
{
    // update status string
    const unsigned int currJob = mResLoader->GetCurrentJobId();
    const unsigned int numJobs = mResLoader->GetNumJobs();
    const int perc = static_cast<int>(std::roundf(currJob * 100.f / numJobs));

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

    // all done
    if(currJob == numJobs)
    {
        GetGame()->RequestNextActiveState(StateId::MAIN_MENU);

        #ifdef DEV_MODE
        std::cout << "ScreenInit - LOADING TIME: " << mTimeLoad << " seconds" << std::endl;
        #endif

        mResLoader->ClearJobs();
    };
}

} // namespace game
