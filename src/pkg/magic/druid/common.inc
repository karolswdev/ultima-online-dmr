//-----------------------------------------------------------------------------

const SPID_TREE_SUPPORT        := 89;

//-----------------------------------------------------------------------------

function CheckDruidAttackGain(who, defender)

  if(GetBaseSkill(who, SKILLID_DRUID) == 0)
    return;
  endif

  if(CanMod(who, MOD_TREE_SUPPORT) == 0)
    GainMagicSpellPoints(who, defender, SPID_TREE_SUPPORT, MAGICGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------
