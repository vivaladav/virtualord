#pragma once

#include "GameObject.h"

namespace game
{

class SceneObject : public GameObject
{
public:
    enum SceneObjectParts : GameObjectVariantId
    {
        // MOUNTAINS
        MOUNTAINS_L = 0,
        MOUNTAINS_T,
        MOUNTAINS_B,
        MOUNTAINS_R,
        MOUNTAINS_1,
        MOUNTAINS_2,
        MOUNTAINS_3,
        MOUNTAINS_4,
        MOUNTAINS_5,
        MOUNTAINS_6,
        MOUNTAINS_7,
        MOUNTAINS_8,

        NUM_MOUNTAINS_PARTS,

        // ROCKS
        ROCKS_01 = 0,
        ROCKS_02,
        ROCKS_03,
        ROCKS_04,
        ROCKS_05,
        ROCKS_06,
        ROCKS_07,
        ROCKS_08,
        ROCKS_L,
        ROCKS_R,
        ROCKS_B,
        ROCKS_T,
        ROCKS_TL,
        ROCKS_BR,
        ROCKS_BL,
        ROCKS_TR,

        NUM_ROCKS_PARTS
    };

public:
    SceneObject(const ObjectData & data, const ObjectInitData & initData,
                GameObjectVariantId part);

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void SetObjColors();
};

} // namespace game
