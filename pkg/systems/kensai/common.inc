//-----------------------------------------------------------------------------

const WC_INTUITION           := 126;
const WC_RETORT              := 125;
const WC_FURIOUS_DEF         := 122;

//-----------------------------------------------------------------------------

function CheckKensaiAttackGain(who, defender)

  if(GetBaseSkill(who, SKILLID_KENSAI) == 0)
    return;
  endif

  if(CanMod(who, MOD_CRIT_HIT) == 0)
    GainWarcastPoints(who, defender, WC_INTUITION, WCGAIN_ATTACK);
  endif

  if(CanMod(who, MOD_RETORT) == 0)
    GainWarcastPoints(who, defender, WC_RETORT, WCGAIN_ATTACK);
  endif

  if(CanMod(who, MOD_CTH) == 0)
    GainWarcastPoints(who, defender, WC_FURIOUS_DEF, WCGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------

