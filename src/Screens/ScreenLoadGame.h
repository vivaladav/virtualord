#pragma once

#include "Screen.h"

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class Label; }
}

namespace game
{

class ResourceLoader;

enum StateId : unsigned int;

class ScreenLoadGame : public Screen
{
public:
    ScreenLoadGame(Game * game, StateId nextState);
    ~ScreenLoadGame();

    void Update(float delta) override;
    void Render() override;

private:
    void UpdateStatus();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::Label * mLabelStatus = nullptr;

    ResourceLoader * mResLoader = nullptr;

    StateId mNextState;

    float mTimeLoad = 0.f;
};

} // namespace game
