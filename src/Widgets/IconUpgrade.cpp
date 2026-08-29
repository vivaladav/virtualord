#include "IconUpgrade.h"

#include "GameConstants.h"
#include "GameData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

IconUpgrade::IconUpgrade(PlayerFaction f)
{
    using namespace sgl;

    // IconUpgrade is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileGameObjectsRelated, ID_GO_UPGRADE_ICON);
    SetTexture(tex);

    if(f != NO_FACTION)
    {
        const unsigned int colorIcon[NUM_FACTIONS] =
        {
            0xffb3b3ff,
            0xb3ffb9ff,
            0xb3f2ffff,
        };

        SetColor(colorIcon[f]);
    }
}

} // namespace game
