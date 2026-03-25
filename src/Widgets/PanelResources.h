#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class GameMap;
class GameSimpleTooltip;
class Player;
class ResourceTooltip;

enum ExtendedResource : unsigned int;

class PanelResources : public sgl::sgui::Widget, public sgl::utilities::StringsChangeListener
{
public:
    PanelResources(Player * player, GameMap * gm, sgl::sgui::Widget * parent);
    PanelResources(Player * player, sgl::sgui::Widget * parent);
    ~PanelResources();

private:
    void HandlePositionChanged() override;

    ResourceTooltip * AssignResourceTooltip(sgl::sgui::Widget * target, ExtendedResource res);
    GameSimpleTooltip * AssignSimpleTooltip(sgl::sgui::Widget * target, const char * text);
    void CreateTooltip(sgl::sgui::Widget * tt, sgl::sgui::Widget * target, int showingMs);

    void OnStringsChanged() override;

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    Player * mPlayer = nullptr;
    GameMap * mGameMap = nullptr;

    std::vector<unsigned int> mCallbackValIds;
    std::vector<unsigned int> mCallbackRangeIds;

    std::vector<ResourceTooltip *> mGameTooltips;
    std::vector<GameSimpleTooltip *> mSimpleTooltips;
};

} // namespace game
