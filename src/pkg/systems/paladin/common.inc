//-----------------------------------------------------------------------------

const WC_CHAMPION            := 33;

//-----------------------------------------------------------------------------

function CheckPaladinAttackGain(who, defender)

  // Ensure processing occurs with thread safety
  Malloc(who);

  if(GetBaseSkill(who, SKILLID_PALADINISM) == 0)
    return;
  endif

  if((CanMod(who, MOD_UNDEAD_SLAY) == 0) or (CanMod(who, MOD_DAEMON_SLAY) == 0))
    GainWarcastPoints(who, defender, WC_CHAMPION, WCGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------
