#include "Screens/SharedScreenListener.h"

#include "Game.h"
#include "Player.h"
#include "Screens/Screen.h"

#ifdef DEV_MODE
#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Window.h>
#include <sgl/sgui/Stage.h>

#include <iostream>
#endif

namespace game
{

void SharedScreenListener::OnKeyUp(sgl::core::KeyboardEvent & event)
{
#ifdef DEV_MODE
    using namespace sgl;

    const int key = event.GetKey();

    auto win = graphic::Window::Instance();

    // F1-F4 set resolution
    if(key == core::KeyboardEvent::KEY_F1)
        win->SetSize(1280, 720);
    else if(key == core::KeyboardEvent::KEY_F2)
        win->SetSize(1600, 900);
    else if(key == core::KeyboardEvent::KEY_F3)
        win->SetSize(1920, 1080);
    else if(key == core::KeyboardEvent::KEY_F4)
        win->SetSize(2560, 1440);
    else if(key == core::KeyboardEvent::KEY_C)
    {
        if(mGame->GetNumActivePlayers() == 0)
            return ;

        Player * p = mGame->GetLocalPlayer();

        // SHIFT + C -> add resources
        if(event.IsModShiftDown())
        {
            p->SumResource(Player::MONEY, 1000);
            p->SumResource(Player::ENERGY, 1000);
            p->SumResource(Player::MATERIAL, 1000);
            p->SumResource(Player::BLOBS, 100);
            p->SumResource(Player::DIAMONDS, 100);
            p->SumResource(Player::RESEARCH, 500);
        }
        // CTRL + C -> remove resources
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
    // ALT + G : switch GOD MODE
    else if(key == core::KeyboardEvent::KEY_G && event.IsModAltDown())
    {
        Game::GOD_MODE = !Game::GOD_MODE;
        std::cout << "GOD MODE: " << (Game::GOD_MODE ? "ON" : "OFF") << std::endl;
    }
    // ALT + M -> toggle mouse cursor visibility
    else if(key == core::KeyboardEvent::KEY_M && event.IsModAltDown())
        sgui::Stage::Instance()->ToggleCursorVisibility();
#endif // DEV_MODE
}

} // namespace game
