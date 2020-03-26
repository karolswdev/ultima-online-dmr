//-----------------------------------------------------------------------

include "include/clock";

//-----------------------------------------------------------------------

const UOBJ_HUNTER_RUNE_SET             := 0xA910;
const UOBJ_HUNTER_RUNE_END             := 0xA91F;

const HN_MANASTOLEN                    := 160;
const HN_ICE_THROWER                   := 162;
const HN_FIRE_THROWER                  := 161;
const HN_EXPLOSION                     := 166;
const HN_MULTISHOT                     := 163;

//-----------------------------------------------------------------------------

function CheckHunterAttackGain(who, defender)

  if(!GetBaseSkill(who, SKILLID_HUNTER))
    return;
  endif

  if(CanMod(who, MOD_COLD_DMG) == 0)
    GainWarcastPoints(who, defender, HN_ICE_THROWER, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_FIRE_DMG) == 0)
    GainWarcastPoints(who, defender, HN_FIRE_THROWER, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_MANA_DMG) == 0)
    GainWarcastPoints(who, defender, HN_MANASTOLEN, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_EXPLO_DMG) == 0)
    GainWarcastPoints(who, defender, HN_EXPLOSION, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_MULTI_SHOT) == 0)
    GainWarcastPoints(who, defender, HN_MULTISHOT, WCGAIN_ATTACK);
  endif

endfunction

//------------------------------------------------------------------------
