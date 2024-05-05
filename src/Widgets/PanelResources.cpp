#include "PanelResources.h"

#include "Player.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ResourceDisplay.h"
#include "Widgets/SimpleResourceDisplay.h"


#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

#include <sgl/sgui/Image.h>

namespace game
{

PanelResources::PanelResources(Player * player, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mBg(new sgl::graphic::Image)
    , mPlayer(player)
{
    RegisterRenderable(mBg);

    SetBg();

    // TODO change this to have dynamic slots added/removed at runtime
    const int slotW = 150;
    const int slots = 5;
    const int slotsW = slotW * slots;
    const int x0 = (GetWidth() - slotsW) * 0.5f;

    int slotX = x0;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = nullptr;

    const int numDigitsMoney = 8;
    const int numDigits = 5;

    mCallbackValIds.resize(Player::Stat::NUM_PSTATS, 0);
    mCallbackRangeIds.resize(Player::Stat::NUM_PSTATS, 0);

    // MONEY
    auto st = Player::Stat::MONEY;
    const StatValue & money = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_MONEY);
    auto srd = new SimpleResourceDisplay(tex, numDigitsMoney, this);
    srd->SetValue(money.GetIntValue());
    srd->SetPosition(slotX + (slotW - srd->GetWidth()) * 0.5f, (GetHeight() - srd->GetHeight()) * 0.5f);

    AssignTooltip(srd, "MONEY");

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [srd](const StatValue * val)
    {
        srd->SetValue(val->GetIntValue());
    });

    slotX += slotW;

    // ENERGY
    st = Player::Stat::ENERGY;
    const StatValue & energy = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_ENERGY);
    auto rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(energy.GetIntMin(), energy.GetIntMax());
    rd->SetValue(energy.GetIntValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    AssignTooltip(rd, "ENERGY");

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue * val)
    {
        rd->SetValue(val->GetIntValue());
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetIntMin(), val->GetIntMax());
    });

    slotX += slotW;

    // MATERIAL
    st = Player::Stat::MATERIAL;
    const StatValue & material = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_MATERIAL);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(material.GetIntMin(), material.GetIntMax());
    rd->SetValue(material.GetIntValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    AssignTooltip(rd, "MATERIAL");

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue * val)
    {
        rd->SetValue(val->GetIntValue());
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetIntMin(), val->GetIntMax());
    });

    slotX += slotW;

    // DIAMONDS
    st = Player::Stat::DIAMONDS;
    const StatValue & diamonds = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_DIAMOND);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(diamonds.GetIntMin(), diamonds.GetIntMax());
    rd->SetValue(diamonds.GetIntValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    AssignTooltip(rd, "DIAMONDS");

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue * val)
    {
        rd->SetValue(val->GetIntValue());
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetIntMin(), val->GetIntMax());
    });

    slotX += slotW;

    // BLOBS
    st = Player::Stat::BLOBS;
    const StatValue & blobs = player->GetStat(st);
    tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_BLOB);
    rd = new ResourceDisplay(tex, numDigits, this);
    rd->SetValueMinMax(blobs.GetIntMin(), blobs.GetIntMax());
    rd->SetValue(blobs.GetIntValue());
    rd->SetPosition(slotX + (slotW - rd->GetWidth()) * 0.5f, (GetHeight() - rd->GetHeight()) * 0.5f);

    AssignTooltip(rd, "BLOBS");

    mCallbackValIds[st] = player->AddOnResourceChanged(st, [rd](const StatValue * val)
    {
        rd->SetValue(val->GetIntValue());
    });
    mCallbackRangeIds[st] = player->AddOnResourceRangeChanged(st, [rd](const StatValue * val)
    {
        rd->SetValueMinMax(val->GetIntMin(), val->GetIntMax());
    });
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

void PanelResources::SetBg()
{
    // NOTE in the future this will chose the right bg based on the number of slots
    auto * tm = sgl::graphic::TextureManager::Instance();

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileResourcesBar, IND_RESBAR_BG);

    mBg->SetTexture(tex);

    SetSize(tex->GetWidth(), tex->GetHeight());
}

void PanelResources::AssignTooltip(sgl::sgui::Widget * target, const char * text)
{
    const int delayMs = 500;
    const int showingMs = 2000;
    auto tt = new GameSimpleTooltip(text);
    target->SetTooltip(tt);
    target->SetTooltipDelay(delayMs);
    target->SetTooltipShowingTime(showingMs);
}

void PanelResources::HandlePositionChanged()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

} // namespace game
