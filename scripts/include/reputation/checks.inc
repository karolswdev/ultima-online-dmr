//-----------------------------------------------------------------------------

function IsMurdPenaltyEnabled()

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsOrder(Mob)

  if(IsPlayer(Mob))
    return (!Mob.criminal and !Mob.murderer);
  elseif(Mob.isa(POLCLASS_NPC))
    if(IsTamed(Mob))
      var Master := GetMaster(Mob);
      if(Master.isa(POLCLASS_MOBILE))
        return (!Master.criminal and !Master.murderer);
      endif
    else
      return (Mob.alignment == NPCAL_GOOD and !Mob.criminal and !Mob.murderer);
    endif
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function IsCriminal(Mob)

  if(IsPlayer(Mob))
    return Mob.criminal;
  elseif(Mob.isa(POLCLASS_NPC))
    if(IsTamed(Mob))
      var Master := GetMaster(Mob);
      if(Master.isa(POLCLASS_MOBILE))
        return Master.criminal;
      endif
    else
      return (Mob.alignment != NPCAL_GOOD or Mob.criminal);
    endif
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function IsMurderer(Mob)

  if(IsPlayer(Mob))
	if(GetObjProperty(Mob, "PermRed"))
		return 1;
	endif
    return Mob.murderer;
  elseif(Mob.isa(POLCLASS_NPC))
    if(IsTamed(Mob))
      var Master := GetMaster(Mob);
      if(Master.isa(POLCLASS_MOBILE))
        return Master.murderer;
      endif
    else
      return (Mob.alignment == NPCAL_EVIL or Mob.alignment == NPCAL_GOOD and Mob.murderer);
    endif
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function IsRealMurderer(who)

  if(IsPlayer(who))
    if(CanMod(who, MOD_STATUS) == 0)
      return CInt(GetModOldAmount(who, MOD_STATUS)[2]);
    endif
  endif

  return IsMurderer(who);

endfunction

//-----------------------------------------------------------------------------
