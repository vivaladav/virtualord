#pragma once

#include "Screen.h"

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class ScreenFactionSelection : public Screen
{
public:
    ScreenFactionSelection(Game * game);
    ~ScreenFactionSelection();

    void Update(float delta) override;
    void Render() override;

private:
    sgl::graphic::Image * mBg = nullptr;
};

} // namespace game
