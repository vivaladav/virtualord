#pragma once

#include <sgl/sgui/ImageButton.h>

namespace game
{

class ButtonPanelSelectedObject : public sgl::sgui::ImageButton
{
public:
    ButtonPanelSelectedObject(sgl::sgui::Widget * parent);

private:
    void HandleMouseOver() override;
    void HandleButtonDown() override;
};

} // namespace game
