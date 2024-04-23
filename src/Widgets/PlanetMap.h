#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class AbstractButtonsGroup;
    }
}

namespace game
{

class ButtonMission;

enum PlayerFaction : unsigned int;
enum TerritoryStatus : unsigned int;

class PlanetMap : public sgl::sgui::Widget
{
public:
    PlanetMap();
    ~PlanetMap();

    void SetFunctionOnToggle(const std::function<void(unsigned int, bool)> & f);

    void SetButtonEnabled(unsigned int index, bool enabled);
    void SetButtonState(unsigned int index, PlayerFaction occupier, TerritoryStatus ts);

    int GetSelectedTerritoryId() const;

    sgl::sgui::AbstractButton * GetButton(unsigned int ind) const;

private:
    void HandlePositionChanged() override;

private:
    static const unsigned int MAX_MISSIONS = 12;

    sgl::sgui::AbstractButtonsGroup * mButtonsMission;

    sgl::graphic::Image * mBg = nullptr;
};

} // namespace game
