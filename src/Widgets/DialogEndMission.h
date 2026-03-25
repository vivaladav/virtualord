#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class AbstractButton; }
}

namespace game
{

class Game;
class Screen;

class DialogEndMission : public sgl::sgui::Widget
{
public:
    DialogEndMission(unsigned int time, unsigned int territoryConquered, unsigned int enemiesKilled,
                     unsigned int casualties, unsigned int turns, bool victory);

    void SetFunctionOnClose(const std::function<void()> & f);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mButton = nullptr;
};

} // namespace game
