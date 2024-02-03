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

class ScreenInit : public Screen
{
public:
    ScreenInit(Game * game);
    ~ScreenInit();

    void Update(float update) override;
    void Render() override;

private:
    void UpdateStatus();

    void SetupLoadPackages();
    void SetupFonts();
    void SetupMusic();
    void SetupSFX();
    void SetupTextures();

private:
    enum TexPackages : unsigned int
    {
        PACKAGE_IMGS_BACKGROUNDS,
        PACKAGE_IMGS_GAME,
        PACKAGE_IMGS_TEST,
        PACKAGE_IMGS_UI_GAME,
        PACKAGE_IMGS_UI_OTHERS,

        NUM_DATA_PACKAGES
    };

    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::Label * mLabelStatus = nullptr;

    std::vector<sgl::core::DataPackage *> mTexPackages;

    std::vector<std::function<void()>> mJobs;
    unsigned int mCurrJob = 0;
};

} // namespace game
