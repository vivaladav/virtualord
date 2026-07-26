#pragma once

#include <functional>
#include <vector>

namespace sgl { namespace core { class DataPackage; } }

namespace game
{

class Game;

class ResourceLoader
{
public:
    ResourceLoader(Game * game);
    ~ResourceLoader();

    unsigned int GetCurrentJobId() const;
    unsigned int GetNumJobs() const;

    // data loading
    void LoadFirstInitData();
    void LoadPreGameData();

    // data destruction
    void DestroyPreGameTextures();

    void Update(float delta);

    void ClearJobs();

private:
    void SetupFonts();

    void SetupMusic();
    void SetupSFX();

    void SetupPermanentTextures();
    void SetupPregameTextures();

    void SetupMouseCursors();

private:
    enum TexPackages : unsigned int
    {
        PACKAGE_IMGS_BACKGROUNDS_PREGAME,
        PACKAGE_IMGS_BACKGROUNDS_PERM,
        PACKAGE_IMGS_TEST,
        PACKAGE_IMGS_UI_PREGAME,
        PACKAGE_IMGS_UI_PERM,
        PACKAGE_IMGS_GAME,
        PACKAGE_IMGS_UI_GAME,
        PACKAGE_IMGS_UI_TUTORIAL,

        NUM_DATA_PACKAGES
    };

    std::vector<sgl::core::DataPackage *> mTexPackages;

    std::vector<std::function<void()>> mJobs;
    unsigned int mCurrJob = 0;

    Game * mGame = nullptr;

    bool mFirstInit = false;
};

inline unsigned int ResourceLoader::GetCurrentJobId() const { return mCurrJob; }
inline unsigned int ResourceLoader::GetNumJobs() const { return mJobs.size(); }

inline void ResourceLoader::ClearJobs()
{
    mCurrJob = 0;
    mJobs.clear();
}

} // namespace game
