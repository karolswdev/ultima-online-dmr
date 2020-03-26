//-----------------------------------------------------------------------------

const SPID_BONE_ARMOR        := 74;

//-----------------------------------------------------------------------------

function CheckNecroAttackGain(who, defender)

  if(GetBaseSkill(who, SKILLID_NECRO) == 0)
    return;
  endif

  if(CanMod(who, MOD_MONSTER_AR) == 0)
    GainMagicSpellPoints(who, defender, SPID_BONE_ARMOR, MAGICGAIN_ATTACK);
  endif

endfunction

//-----------------------------------------------------------------------------
