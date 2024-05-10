#pragma once

#include <sgl/sgui/Widget.h>

#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class Player;

class PanelResources : public sgl::sgui::Widget
{
public:
    PanelResources(Player * player, sgl::sgui::Widget * parent);
    ~PanelResources();

private:
    void HandlePositionChanged() override;

    void SetBg();

    void AssignTooltip(sgl::sgui::Widget * target, const char * text);

private:
    sgl::graphic::Image * mBg = nullptr;

    Player * mPlayer = nullptr;

    std::vector<unsigned int> mCallbackValIds;
    std::vector<unsigned int> mCallbackRangeIds;
};

} // namespace game
