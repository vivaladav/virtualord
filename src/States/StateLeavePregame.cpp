#include "States/StateLeavePregame.h"

#include "Game.h"
#include "Screens/DummyScreen.h"
#include "States/StatesIds.h"
#include "Widgets/GameUIData.h"

#ifdef DEV_MODE
#include "GameTestData.h"
#endif

#include <sgl/graphic/TextureManager.h>

#include <cassert>

namespace game
{

StateLeavePregame::StateLeavePregame(Game * game)
    : BaseGameState(StateId::LEAVE_PREGAME, game)
{
}

void StateLeavePregame::SetNextStateData(sgl::utilities::StateData * data)
{
    assert(data != nullptr);

    auto d = static_cast<StateDataLeavePregame *>(data);

    mNextState = d->GetNextState();
}

void StateLeavePregame::OnActive()
{
    mScreen = new DummyScreen(mGame);

    DestroyPregameTextures();

    mGame->RequestNextActiveState(mNextState);
}

void StateLeavePregame::OnInactive()
{
    delete mScreen;
    mScreen = nullptr;
}

void StateLeavePregame::DestroyPregameTextures()
{
    auto tm = sgl::graphic::TextureManager::Instance();

    // BACKGROUNDS PREGAME
    tm->DestroyTexture("main_menu_bg.png");

    // UI PREGAME
    tm->DestroySprite(SpriteFileMainMenu);
    tm->DestroyTexture("UI/main_menu_build_badge.png");
    tm->DestroyTexture("UI/main_menu_warning_bg.png");

    // MAIN MENU BUTTONS
    tm->DestroySprite(SpriteFileMainMenuButtons);

    // TEST
#ifdef DEV_MODE
    tm->DestroySprite(SpriteFileTestSprite);
    tm->DestroySprite(SpriteFileTestUI);
    tm->DestroyTexture("test/obj_null.png");
    tm->DestroyTexture("test/red_dot4.png");
    tm->DestroyTexture("test/square100.png");
    tm->DestroyTexture("test/test-bar-bg.png");
    tm->DestroyTexture("test/test-bar-nobg.png");
    tm->DestroyTexture("test/text_area.png");
    tm->DestroyTexture(SpriteFileTestSprite);
#endif
}

} // namespace game
