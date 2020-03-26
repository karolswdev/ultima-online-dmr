//-----------------------------------------------------------------------------

const WC_RAID                := 75;

//-----------------------------------------------------------------------------

function CheckDarkKnightAttackGain(who, defender)

  if(GetBaseSkill(who, SKILLID_FANATICISM) == 0)
    return;
  endif

  if(CanMod(who, MOD_ORDER_SLAY) == 0)
    GainWarcastPoints(who, defender, WC_RAID, WCGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------
