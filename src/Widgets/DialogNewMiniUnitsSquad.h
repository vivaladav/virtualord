#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <sgl/sgui/Widget.h>

#include <vector>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class AbstractButton;
        class Image;
        class Label;
        class Slider;
        class TextArea;
    }
}

namespace game
{

class GameObject;
class ObjectsDataRegistry;
class ObjectVisualAttribute;
class Player;

class DialogNewMiniUnitsSquad : public sgl::sgui::Widget
{
public:
    DialogNewMiniUnitsSquad(GameObject * spawner, Player * player,
                            const ObjectsDataRegistry * dataReg);

    GameObjectTypeId GetTypeToBuild() const;

    int GetNumElements() const;
    int GetNumSquads() const;

    void AddFunctionOnBuild(const std::function<void()> & f);
    void AddFunctionOnClose(const std::function<void()> & f);

private:
    void CreatePanelPreview();
    void CreatePanelDescription();
    void CreatePanelAttributes();
    void CreatePanelConfig();

    void ChangeIndex(int delta);

    void UpdateTotalCosts();
    void UpdatePreview();
    void UpdateData();

    void HandlePositionChanged() override;

    void SetPositions();

private:
    std::vector<ObjectVisualAttribute *> mAttributes;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::Widget * mPanelPreview = nullptr;
    sgl::sgui::Widget * mPanelDescription = nullptr;
    sgl::sgui::Widget * mPanelConfig = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
    sgl::sgui::AbstractButton * mBtnLeft = nullptr;
    sgl::sgui::AbstractButton * mBtnRight = nullptr;
    sgl::sgui::AbstractButton * mBtnBuild = nullptr;
    sgl::sgui::Image * mImgPreview = nullptr;
    sgl::sgui::Label * mLabelCostEnergy = nullptr;
    sgl::sgui::Label * mLabelCostMaterial = nullptr;
    sgl::sgui::Label * mLabelCostDiamonds = nullptr;
    sgl::sgui::Label * mLabelCostBlobs = nullptr;
    sgl::sgui::Label * mLabelTotCostEnergy = nullptr;
    sgl::sgui::Label * mLabelTotCostMaterial = nullptr;
    sgl::sgui::Label * mLabelTotCostDiamonds = nullptr;
    sgl::sgui::Label * mLabelTotCostBlobs = nullptr;
    sgl::sgui::Slider * mSliderElements = nullptr;
    sgl::sgui::Slider * mSliderSquads = nullptr;
    sgl::sgui::TextArea * mDescription = nullptr;

    GameObject * mSpawner = nullptr;

    Player * mPlayer = nullptr;

    const ObjectsDataRegistry * mDataReg = nullptr;

    unsigned int mCurrentIndex = 0;
};

} // namespace game
