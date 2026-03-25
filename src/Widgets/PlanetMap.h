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

enum PlayerFaction : unsigned int;
enum PlanetSize : unsigned int;
enum TerritoryStatus : unsigned int;

/* ----- MAP SCHEME -----
 *
 * LARGE
 *
 *     0
 * 2       3
 * 6   8   7
 * 4       5
 *     1
 *
 * MEDIUM
 *
 * 0       1
 * 4   6   5
 * 2       3
 *
 * SMALL
 *
 *      0
 *  2   4   3
 *      1
 */

class PlanetMap : public sgl::sgui::Widget
{
public:
    PlanetMap(PlanetSize ps);
    ~PlanetMap();

    void SetFunctionOnToggle(const std::function<void(unsigned int, bool)> & f);

    void SetButtonEnabled(unsigned int index, bool enabled);
    void SetButtonState(unsigned int index, PlayerFaction occupier, TerritoryStatus ts);

    int GetSelectedTerritoryId() const;

    sgl::sgui::AbstractButton * GetButton(unsigned int ind) const;

private:
    void HandlePositionChanged() override;

private:
    sgl::sgui::AbstractButtonsGroup * mButtonsMission;

    sgl::graphic::Image * mBg = nullptr;
};

} // namespace game
