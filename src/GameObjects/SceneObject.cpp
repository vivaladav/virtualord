#include "GameObjects/SceneObject.h"

#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

SceneObject::SceneObject(const ObjectData & data, const ObjectInitData & initData,
                         GameObjectVariantId part)
    : GameObject(data, initData)
{
    SetStatic(true);

    mVariant = part;

    // set object health
    if(ObjectData::TYPE_MOUNTAINS == data.GetType())
    {
        // health
        const float maxHealthValue = 5000.f;
        SetMaxHealth(maxHealthValue);
    }

    SetImage();
}

void SceneObject::UpdateGraphics()
{
    SetImage();

    SetObjColors();
}

void SceneObject::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // set texture
    auto tm = sgl::graphic::TextureManager::Instance();

    sgl::graphic::Texture * tex = nullptr;

    const GameObjectTypeId type = GetObjectType();

    if(type == ObjectData::TYPE_ROCKS)
    {
        const unsigned int spriteId = ID_ROCKS1_01 + mVariant;
        tex = tm->GetSprite(SpriteFileSceneObjects, spriteId);
    }
    else if(type == ObjectData::TYPE_MOUNTAINS)
    {
        const unsigned int spriteId = ID_MOUNTAIN1_L + mVariant;
        tex = tm->GetSprite(SpriteFileSceneObjects, spriteId);
    }
    // this should never happen
    else
        return ;

    isoObj->SetTexture(tex);
}

void SceneObject::SetObjColors()
{
    mObjColors.clear();

    const GameObjectTypeId type = GetObjectType();

    if(type == ObjectData::TYPE_MOUNTAINS)
    {
        mObjColors.push_back(0xad9485ff);
        mObjColors.push_back(0xa38776ff);
        mObjColors.push_back(0x997966ff);
        mObjColors.push_back(0x7a6152ff);
        mObjColors.push_back(0x6b5548ff);
        mObjColors.push_back(0x584a41ff);
    }
    else if(type == ObjectData::TYPE_ROCKS)
    {
        mObjColors.push_back(0x999999ff);
        mObjColors.push_back(0x808080ff);
        mObjColors.push_back(0x666666ff);
        mObjColors.push_back(0x333333ff);
    }
}

} // namespace game
