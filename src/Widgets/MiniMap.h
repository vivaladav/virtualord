#include "Cell2D.h"

#include <sgl/sgui/Widget.h>

#include <functional>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class ImageButton; }
}

namespace game
{

class CameraMapController;
class IsoMap;

enum PlayerFaction : unsigned int;

class MiniMap : public sgl::sgui::Widget
{
public:
    enum MiniMapElemType: unsigned int
    {
        MME_FACTION1,
        MME_FACTION2,
        MME_FACTION3,
        MME_CONQUERABLE,
        MME_SCENE,

        NUM_MM_ELEM_TYPES
    };

public:
    MiniMap(CameraMapController * cameraController, IsoMap * im, sgl::sgui::Widget * parent);
    ~MiniMap();

    void AddFunctionOnClose(const std::function<void()> & f);

    void AddElement(int r0, int c0, int rows, int cols, MiniMapElemType type, PlayerFaction faction);
    void UpdateElement(int r0, int c0, int rows, int cols, MiniMapElemType type, PlayerFaction faction);
    void RemoveElement(int r0, int c0);
    void MoveElement(int startRow, int startCol, int endRow, int endCol);

    void SetCameraCells(const Cell2D & tl, const Cell2D & tr, const Cell2D & bl, const Cell2D & br);

    void Refresh();

private:
    struct MiniMapElem
    {
        MiniMapElem(int r0, int c0, int r1, int c1, MiniMapElemType mmt,
                    PlayerFaction pf, sgl::graphic::Image * i)
            : brR(r0)
            , brC(c0)
            , tlR(r1)
            , tlC(c1)
            , rows(r0 - r1 + 1)
            , cols(c0 - c1 + 1)
            , type(mmt)
            , faction(pf)
            , img(i)
        {}

        ~MiniMapElem();

        int brR;
        int brC;
        int tlR;
        int tlC;
        int rows;
        int cols;
        MiniMapElemType type;
        PlayerFaction faction;
        sgl::graphic::Image * img;
    };

private:
    void ClearElements();

    void PositionElement(MiniMapElem * elem);

    void MoveContentHorizontal(int val);
    void MoveContentVertical(int val);

    void UpdateAreaButtons();

    void UpdateMapArea();

    void HandlePositionChanged() override;

    void HandleMouseButtonDown(sgl::core::MouseButtonEvent & event) override;
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event) override;
    void HandleMouseMotion(sgl::core::MouseMotionEvent & event) override;

    void OnRender() override;

private:
    std::vector<MiniMapElem *> mElementsMap;
    std::vector<MiniMapElem *> mElementsRenderingList;

    CameraMapController * mCamController = nullptr;

    IsoMap * mIsoMap = nullptr;

    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    sgl::sgui::ImageButton * mButtonL = nullptr;
    sgl::sgui::ImageButton * mButtonR = nullptr;
    sgl::sgui::ImageButton * mButtonU = nullptr;
    sgl::sgui::ImageButton * mButtonD = nullptr;

    sgl::graphic::Image * mCameraCornerTL = nullptr;
    sgl::graphic::Image * mCameraCornerTR = nullptr;
    sgl::graphic::Image * mCameraCornerBL = nullptr;
    sgl::graphic::Image * mCameraCornerBR = nullptr;

    int mR0 = 0;
    int mC0 = 0;
    int mR1 = 0;
    int mC1 = 0;

    int mMapX = 0;
    int mMapY = 0;
    int mMapW = 0;
    int mMapH = 0;

    int mMapMove = 0;
};

} // namespace game
