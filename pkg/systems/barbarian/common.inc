//-----------------------------------------------------------------------------

const WC_BATTLE_SHAWL        := 143;
const WC_REPRESSION          := 140;

//-----------------------------------------------------------------------------

function CheckBarbAttackGain(who, defender)

  if(GetBaseSkill(who, SKILLID_BARBARIAN) < 5)
    return;
  endif

  if(ExPid(GetObjProperty(who, "#BattleShawlPid")))
    GainWarcastPoints(who, defender, WC_BATTLE_SHAWL, WCGAIN_ATTACK);
  endif

  if(CanMod(who, MOD_CTH) == 0)
    GainWarcastPoints(who, defender, WC_REPRESSION, WCGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------
