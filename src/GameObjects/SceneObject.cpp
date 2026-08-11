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
    const GameObjectTypeId type = data.GetType();

    if(ObjectData::TYPE_MOUNTAINS1 == type || ObjectData::TYPE_MOUNTAINS2 == type)
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

    unsigned int spriteId = 0;

    if(type == ObjectData::TYPE_ROCKS)
        spriteId = ID_ROCKS1_01 + mVariant;
    else if(type == ObjectData::TYPE_MOUNTAINS1)
        spriteId = ID_MOUNTAIN1_L + mVariant;
    else if(type == ObjectData::TYPE_MOUNTAINS2)
        spriteId = ID_MOUNTAIN2_L + mVariant;
    // this should never happen
    else
        return ;

    tex = tm->GetSprite(SpriteFileSceneObjects, spriteId);
    isoObj->SetTexture(tex);
}

void SceneObject::SetObjColors()
{
    mObjColors.clear();

    const GameObjectTypeId type = GetObjectType();

    if(type == ObjectData::TYPE_MOUNTAINS1)
    {
        mObjColors.push_back(0xad9485ff);
        mObjColors.push_back(0xa38776ff);
        mObjColors.push_back(0x997966ff);
        mObjColors.push_back(0x7a6152ff);
        mObjColors.push_back(0x6b5548ff);
        mObjColors.push_back(0x584a41ff);
    }
    if(type == ObjectData::TYPE_MOUNTAINS2)
    {
        mObjColors.push_back(0x736059ff);
        mObjColors.push_back(0x544840ff);
        mObjColors.push_back(0x607656ff);
        mObjColors.push_back(0x485841ff);
        mObjColors.push_back(0x303b2bff);
        mObjColors.push_back(0x303b2bff);
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
