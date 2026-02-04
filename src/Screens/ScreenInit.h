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
    ScreenInit(Game * game, bool firstInit);
    ~ScreenInit();

    void Update(float delta) override;
    void Render() override;

private:
    void UpdateStatus();

    void SetupFonts();
    void SetupMouseCursors();
    void SetupMusic();
    void SetupSFX();
    void SetupPermanentTextures();
    void SetupPregameTextures();

private:
    enum TexPackages : unsigned int
    {
        PACKAGE_IMGS_BACKGROUNDS_PREGAME,
        PACKAGE_IMGS_BACKGROUNDS_PERM,
        PACKAGE_IMGS_TEST,
        PACKAGE_IMGS_UI_PREGAME,
        PACKAGE_IMGS_UI_PERM,

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
