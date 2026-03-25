#pragma once

#include "Screen.h"

#include <functional>
#include <vector>

namespace sgl
{
    namespace core { class DataPackage; }
    namespace graphic { class Image; }
    namespace sgui { class Label; }
}

namespace game
{

class ScreenInitGame : public Screen
{
public:
    ScreenInitGame(Game * game);
    ~ScreenInitGame();

    void Update(float delta) override;
    void Render() override;

private:
    void UpdateStatus();

    void SetupLoadPackages();
    void SetupTextures();

private:
    enum TexPackages : unsigned int
    {
        PACKAGE_IMGS_GAME,
        PACKAGE_IMGS_UI_GAME,
        PACKAGE_IMGS_UI_TUTORIAL,

        NUM_DATA_PACKAGES
    };

    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::Label * mLabelStatus = nullptr;

    std::vector<sgl::core::DataPackage *> mTexPackages;

    std::vector<std::function<void()>> mJobs;
    unsigned int mCurrJob = 0;

    float mTimeLoad = 0.f;
};

} // namespace game
