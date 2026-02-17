#include "Screens/SharedScreenListener.h"

#include "Game.h"
#include "Player.h"
#include "Screens/Screen.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Window.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

#ifdef DEV_MODE
#include <iostream>
#endif

namespace game
{

void SharedScreenListener::OnKeyUp(sgl::core::KeyboardEvent & event)
{
    using namespace sgl::core;
    using namespace sgl::graphic;

    const int key = event.GetKey();

    Game * game = mScreen->GetGame();

#ifdef DEV_MODE
    // SHIFT-ESC -> EXIT
    if(key == KeyboardEvent::KEY_ESCAPE && event.IsModShiftDown())
        game->Exit();
#endif

    // -- WINDOW --
    if(key == KeyboardEvent::KEY_F && event.IsModAltDown())
        Window::Instance()->SwitchFullscreen();

#ifdef DEV_MODE
    if(key == KeyboardEvent::KEY_F1)
        Window::Instance()->SetSize(1280, 720);
    else if(key == KeyboardEvent::KEY_F2)
        Window::Instance()->SetSize(1600, 900);
    else if(key == KeyboardEvent::KEY_F3)
        Window::Instance()->SetSize(1920, 1080);
    else if(key == KeyboardEvent::KEY_F4)
        Window::Instance()->SetSize(2560, 1440);

    // -- GAME --
    // DEBUG: SHIFT/CTRL + R -> add/remove resources
    else if(key == KeyboardEvent::KEY_R)
    {
        if(mGame->GetNumPlayers() == 0)
            return ;

        Player * p = mGame->GetLocalPlayer();

        if(event.IsModShiftDown())
        {
            p->SumResource(Player::MONEY, 1000);
            p->SumResource(Player::ENERGY, 1000);
            p->SumResource(Player::MATERIAL, 1000);
            p->SumResource(Player::BLOBS, 100);
            p->SumResource(Player::DIAMONDS, 100);
            p->SumResource(Player::RESEARCH, 100);
        }
        else if(event.IsModCtrlDown())
        {
            p->SumResource(Player::MONEY, -100);
            p->SumResource(Player::ENERGY, -100);
            p->SumResource(Player::MATERIAL, -100);
            p->SumResource(Player::BLOBS, -10);
            p->SumResource(Player::DIAMONDS, -10);
            p->SumResource(Player::RESEARCH, -100);
        }
    }
    // switch GOD MODE
    else if(key == KeyboardEvent::KEY_G && event.IsModAltDown())
    {
        Game::GOD_MODE = !Game::GOD_MODE;
        std::cout << "GOD MODE: " << (Game::GOD_MODE ? "ON" : "OFF") << std::endl;
    }
#endif

    // -- AUDIO --
    else if(event.IsModAltDown())
    {
        auto am = sgl::media::AudioManager::Instance();
        auto ap = am->GetPlayer();

        // TOGGLE MUSIC
        if(key == KeyboardEvent::KEY_M)
            ap->SetMusicEnabled(!ap->IsMusicEnabled());
        else if(key == KeyboardEvent::KEY_X)
            ap->SetSoundEnabled(!ap->IsSoundEnabled());
    }
}

} // namespace game
