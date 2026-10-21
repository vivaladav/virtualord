#include <sgl/sgui/ComboBox.h>
#include <sgl/sgui/ComboBoxItem.h>

namespace sgl
{
    namespace sgui { class Label; }
}

namespace game
{

// ====== COMBOBOX =====
class SettingsComboBox : public sgl::sgui::ComboBox
{
public:
    SettingsComboBox(sgl::sgui::Widget * parent);

private:
    void HandleButtonDown() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void SetGraphics(sgl::sgui::AbstractButton::VisualState state);
};

// ====== COMBOBOX ITEM =====
class GameComboBoxItem : public sgl::sgui::ComboBoxItem
{
public:
    GameComboBoxItem(const char * txt);

    void SetLabel(const char * txt);

private:
    void HandleMouseOver() override;

    void HandleButtonDown() override;

    void HandlePositionChanged() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void UpdateGraphics();

    void PositionElements();

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::sgui::Label * mLabel = nullptr;
};

} // namespace game
