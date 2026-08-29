#pragma once

#include <sgl/sgui/Label.h>

namespace game
{

class DisappearingLabel : public sgl::sgui::Label
{
public:
    DisappearingLabel(sgl::graphic::Font * font, const char * text, float timeSec);

private:
    void OnUpdate(float delta) override;

private:
    float mTimer = 0.f;
};

} // namespace game
