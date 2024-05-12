#pragma once

#include <sgl/sgui/Widget.h>

#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class GameMap;
class Player;

class PanelResources : public sgl::sgui::Widget
{
public:
    PanelResources(Player * player, GameMap * gm, sgl::sgui::Widget * parent);
    PanelResources(Player * player, sgl::sgui::Widget * parent);
    ~PanelResources();

private:
    void HandlePositionChanged() override;

    void SetBg();

    void AssignResourceTooltip(sgl::sgui::Widget * target, const char * text);
    void AssignSimpleTooltip(sgl::sgui::Widget * target, const char * text);
    void SetTooltip(sgl::sgui::Widget * tt, sgl::sgui::Widget * target);

private:
    sgl::graphic::Image * mBg = nullptr;

    Player * mPlayer = nullptr;
    GameMap * mGameMap = nullptr;

    std::vector<unsigned int> mCallbackValIds;
    std::vector<unsigned int> mCallbackRangeIds;
};

} // namespace game
