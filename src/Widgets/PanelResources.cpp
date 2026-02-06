#include "PanelResources.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "Player.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ResourceDisplay.h"
#include "Widgets/ResourceTooltip.h"
#include "Widgets/SimpleResourceDisplay.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/utilities/StringManager.h>

#include <sgl/sgui/Image.h>

namespace game
{

PanelResources::PanelResources(Player * player, GameMap * gm, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mPlayer(player)
    , mGameMap(gm)
{
    using namespace sgl;

    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

    // -- BACKGROUND --
    const int slotW = 160;
    const int slots = 6;
    const int slotsW = slotW * slots;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = nullptr;

    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    tex = tm->GetTexture(SpriteFileResourcesBarExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    mBgC->SetWidth(slotsW);
    RegisterRenderable(mBgC);

    const int totW = mBgL->GetWidth() + slotsW + mBgR->GetWidth();

    SetSize(totW, mBgL->GetHeight());

    // -- SLOTS --
    const int x0 = mBgL->GetWidth();

    int slotX = x0;

    const int numDigitsMoney = 8;
    const int numDigitsResearch = 6;
    const int numDigits = 5;

    mCallbackValIds.resize(Player::Stat::NUM_PSTATS, 0);
    mCallbackRangeIds.resize(Player::Stat::NUM_PSTATS, 0);

    mGameTooltips.assign(Player::Stat::NUM_PSTATS, nullptr);
    mSimpleTooltips.assign(Player::Stat::NUM_PSTATS, nullptr);

    // MONEY
    auto st = Player::Stat::MONEY;
    const StatValue & money = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_MONEY);
    auto srd = new SimpleResourceDisplay(tex, numDigitsMoney, this);
    srd->SetValue(money.GetValue());
    srd->SetPosition(slotX + (slotW - srd->GetWidth()) * 0.5f, (GetHeight() - srd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(srd, sm->GetCString("MONEY_TURN"));
        mGameTooltips[st] = tt;

        srd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = mGameMap->GetFactionMoneyPerTurn(mPlayer->GetFaction());
            const int resOut = mPlayer->GetMoneySpentPerTurn();
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(srd, sm->GetCString("MONEY"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [srd](const StatValue *, int, int newVal)
    {
        srd->SetValue(newVal);
    });

    slotX += slotW;

    // ENERGY
    st = Player::Stat::ENERGY;
    const StatValue & energy = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_ENERGY);
    auto rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(energy.GetMin(), energy.GetMax());
    rd->SetValue(energy.GetValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(rd, sm->GetCString("ENERGY_TURN"));
        mGameTooltips[st] = tt;

        rd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = mPlayer->GetResourceProduction(ResourceType::RES_ENERGY);
            const int resOut = mPlayer->GetResourceConsumption(ResourceType::RES_ENERGY);
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(rd, sm->GetCString("ENERGY"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue *, int, int newVal)
    {
        rd->SetValue(newVal);
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetMin(), val->GetMax());
    });

    slotX += slotW;

    // MATERIAL
    st = Player::Stat::MATERIAL;
    const StatValue & material = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_MATERIAL);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(material.GetMin(), material.GetMax());
    rd->SetValue(material.GetValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(rd, sm->GetCString("MATERIAL_TURN"));
        mGameTooltips[st] = tt;

        rd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = mPlayer->GetResourceProduction(ResourceType::RES_MATERIAL1);
            const int resOut = mPlayer->GetResourceConsumption(ResourceType::RES_MATERIAL1);
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(rd, sm->GetCString("MATERIAL"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue *, int, int newVal)
    {
        rd->SetValue(newVal);
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetMin(), val->GetMax());
    });

    slotX += slotW;

    // DIAMONDS
    st = Player::Stat::DIAMONDS;
    const StatValue & diamonds = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_DIAMOND);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(diamonds.GetMin(), diamonds.GetMax());
    rd->SetValue(diamonds.GetValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(rd, sm->GetCString("DIAMONDS_TURN"));
        mGameTooltips[st] = tt;

        rd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = mPlayer->GetResourceProduction(ResourceType::RES_DIAMONDS);
            const int resOut = mPlayer->GetResourceConsumption(ResourceType::RES_DIAMONDS);
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(rd, sm->GetCString("DIAMONDS"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue *, int, int newVal)
    {
        rd->SetValue(newVal);
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetMin(), val->GetMax());
    });

    slotX += slotW;

    // BLOBS
    st = Player::Stat::BLOBS;
    const StatValue & blobs = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_BLOB);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(blobs.GetMin(), blobs.GetMax());
    rd->SetValue(blobs.GetValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(rd, sm->GetCString("BLOBS_TURN"));
        mGameTooltips[st] = tt;

        rd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = mPlayer->GetResourceProduction(ResourceType::RES_BLOBS);
            const int resOut = mPlayer->GetResourceConsumption(ResourceType::RES_BLOBS);
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(rd, sm->GetCString("BLOBS"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue * val, int, int newVal)
    {
        rd->SetValue(newVal);
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetMin(), val->GetMax());
    });

    slotX += slotW;

    // RESEARCH POINTS
    st = Player::Stat::RESEARCH;
    const StatValue & researchPts = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_RESEARCH);
    srd = new SimpleResourceDisplay(tex, numDigitsResearch, this);
    srd->SetValue(researchPts.GetValue());
    srd->SetPosition(slotX + (slotW - srd->GetWidth()) * 0.5f, (GetHeight() - srd->GetHeight()) * 0.5f);

    if(mGameMap)
    {
        auto tt = AssignResourceTooltip(srd, sm->GetCString("RESEARCH_TURN"));
        mGameTooltips[st] = tt;

        srd->SetFunctionOnShowingTooltip([this, tt]
        {
            const int resIn = 0;
            const int resOut = 0;
            tt->SetValues(resIn, resOut);
        });
    }
    else
        mSimpleTooltips[st] = AssignSimpleTooltip(srd, sm->GetCString("RESEARCH_PTS"));

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [srd](const StatValue *, int, int newVal)
    {
        srd->SetValue(newVal);
    });
}

PanelResources::PanelResources(Player * player, sgl::sgui::Widget * parent)
    : PanelResources(player, nullptr, parent)
{
}

PanelResources::~PanelResources()
{
    // CLEAR OBSERVERS FROM PLAYER STATS
    auto st = Player::Stat::MONEY;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);

    st = Player::Stat::ENERGY;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);
    mPlayer->RemoveOnResourceRangeChanged(st, mCallbackRangeIds[st]);

    st = Player::Stat::MATERIAL;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);
    mPlayer->RemoveOnResourceRangeChanged(st, mCallbackRangeIds[st]);

    st = Player::Stat::DIAMONDS;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);
    mPlayer->RemoveOnResourceRangeChanged(st, mCallbackRangeIds[st]);

    st = Player::Stat::BLOBS;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);
    mPlayer->RemoveOnResourceRangeChanged(st, mCallbackRangeIds[st]);
}

ResourceTooltip * PanelResources::AssignResourceTooltip(sgl::sgui::Widget * target, const char * text)
{
    const int showingMs = 5000;

    auto tt = new ResourceTooltip(text);
    CreateTooltip(tt, target, showingMs);

    return tt;
}

GameSimpleTooltip * PanelResources::AssignSimpleTooltip(sgl::sgui::Widget * target, const char * text)
{
    const int showingMs = 2000;

    auto tt = new GameSimpleTooltip(text);
    CreateTooltip(tt, target, showingMs);

    return tt;
}

void PanelResources::CreateTooltip(sgl::sgui::Widget * tt, sgl::sgui::Widget * target, int showingMs)
{
    target->SetTooltip(tt);
    target->SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
    target->SetTooltipShowingTime(showingMs);
}

void PanelResources::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    if(mGameMap)
    {
        mGameTooltips[Player::Stat::MONEY]->SetTitle(sm->GetCString("MONEY_TURN"));
        mGameTooltips[Player::Stat::ENERGY]->SetTitle(sm->GetCString("ENERGY_TURN"));
        mGameTooltips[Player::Stat::MATERIAL]->SetTitle(sm->GetCString("MATERIAL_TURN"));
        mGameTooltips[Player::Stat::BLOBS]->SetTitle(sm->GetCString("BLOBS_TURN"));
        mGameTooltips[Player::Stat::DIAMONDS]->SetTitle(sm->GetCString("DIAMONDS_TURN"));
        mGameTooltips[Player::Stat::RESEARCH]->SetTitle(sm->GetCString("RESEARCH_TURN"));
    }
    else
    {
        mSimpleTooltips[Player::Stat::MONEY]->SetText(sm->GetCString("MONEY"));
        mSimpleTooltips[Player::Stat::ENERGY]->SetText(sm->GetCString("ENERGY"));
        mSimpleTooltips[Player::Stat::MATERIAL]->SetText(sm->GetCString("MATERIAL"));
        mSimpleTooltips[Player::Stat::BLOBS]->SetText(sm->GetCString("BLOBS"));
        mSimpleTooltips[Player::Stat::DIAMONDS]->SetText(sm->GetCString("DIAMONDS"));
        mSimpleTooltips[Player::Stat::RESEARCH]->SetText(sm->GetCString("RESEARCH_PTS"));
    }
}

void PanelResources::HandlePositionChanged()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    int x = x0;

    mBgL->SetPosition(x, y0);
    x+= mBgL->GetWidth();

    mBgC->SetPosition(x, y0);
    x+= mBgC->GetWidth();

    mBgR->SetPosition(x, y0);
}

} // namespace game
