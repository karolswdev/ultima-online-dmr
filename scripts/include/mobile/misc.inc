//-----------------------------------------------------------------------------

include ":magic:common";

//-----------------------------------------------------------------------------

function GetMobWeaponSkillId(Mob)

  if(!Mob.isa(POLCLASS_MOBILE))
    return error;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[Mob.weapon.objtype].Attribute == error)
    return error;
  endif

  return FindSkillidByIdentifier(ItemCfg[Mob.weapon.objtype].Attribute);

endfunction

//-----------------------------------------------------------------------------

function IsInvul(Mob)

  if(Mob.enabled("invul"))
    return 1;
  endif

  if(Mob.isa(POLCLASS_NPC))
    if(CStr(GetNpcConfig(Mob).Privs)["invul"])
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsAttackingPlayer(who)

  if(who.isa(POLCLASS_MOBILE))
    if(IsPlayer(who.opponent))
      return 1;
    elseif(IsTamed(who.opponent) and IsPlayer(GetMaster(who.opponent)))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetAttackedPlayer(who)

  if(who.isa(POLCLASS_MOBILE))
    if(IsPlayer(who.opponent))
      return who.opponent;
    elseif(IsTamed(who.opponent) and IsPlayer(GetMaster(who.opponent)))
      return GetMaster(who.opponent);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetFirstGainableEngagingNpc(who)

  var Attackers := ListHostiles(who);

  foreach Attacker in Attackers
    if(!IsTamed(Attacker) and !IsPlayer(Attacker))
      if(IsMobGainable(Attacker, who))
        return Attacker;
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsEngaged(Mob, wTime := 30)

  var Attackers := ListHostiles(Mob);

  if(Attackers and (Attackers.size() > 0))
    return 1;
  endif

  var LastTime := CInt(GetObjProperty(Mob, "#LastHitTime"));

  if(LastTime and LastTime + wTime > ReadGameClock())
    return error;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMobManaDesc( percentmana )
    if (percentmana <= 10)
            return "prawie nie ma many";
    elseif (percentmana <= 20) 
            return "ma malo many";
    elseif (percentmana <= 40 )
            return "ma sredni zapas many";
    elseif (percentmana <= 60 )
            return "ma duzy zapas many";
    elseif (percentmana <= 80 )
            return "ma bardzo duzy zapas many";
    else  
            return "ma pelny zapas many";
    endif     
endfunction

//-----------------------------------------------------------------------------
