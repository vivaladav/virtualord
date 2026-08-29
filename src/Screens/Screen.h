#pragma once

#include <sgl/core/event/ApplicationEventListener.h>
#include <sgl/core/event/KeyboardEventListener.h>
#include <sgl/core/event/MouseEventListener.h>
#include <sgl/graphic/event/WindowEventListener.h>

namespace sgl
{
    namespace sgui { class Widget; }

    namespace utilities { class BinaryFile; }
}

namespace game
{

class DialogSettings;
class Game;
class SharedScreenListener;

class Screen : public sgl::core::ApplicationEventListener, public sgl::core::KeyboardEventListener,
               public sgl::core::MouseEventListener, public sgl::graphic::WindowEventListener
{
public:
    Screen(Game * game);
    ~Screen();

    virtual bool Save(sgl::utilities::BinaryFile & bf) const;

    Game * GetGame() const;

    // UI
    DialogSettings * ShowDialogSettings();

    void ShowLabelGameSaved();

    virtual void Update(float delta) = 0;

    virtual void Render() = 0;

protected:
    void ShowScreenOverlay();
    void HideScreenOverlay();

private:
    Game * mGame = nullptr;

    SharedScreenListener * mSharedListener = nullptr;

    DialogSettings * mSettings = nullptr;

    sgl::sgui::Widget * mOverlay = nullptr;
};

inline Game * Screen::GetGame() const { return mGame; }

} // namespace game
