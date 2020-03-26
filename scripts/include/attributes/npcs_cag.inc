//--------------------------------------------------------------------------------

include "include/mobile/misc";

//--------------------------------------------------------------------------------

function CagIsAvailable(Npc, byref opponent)

  if(!opponent and Npc)
    opponent := Npc.opponent;
  endif

  if(!Npc or !opponent or !Npc.isa(POLCLASS_NPC) or !opponent.isa(POLCLASS_MOBILE))
    return error;
  endif

  var Dist := Distance(Npc, opponent);
  var NpcElem := GetNpcConfig(Npc);

  if(Dist > 12)
    return error;
  elseif(NpcElem.Immobile and Dist > 1)
    return error;
  elseif(Npc.frozen)
    return error;
  elseif(!CheckLineOfSight(Npc, opponent))
    return error;
  elseif(IsInvul(Npc))
    return error;
  elseif(NpcElem.CityNpc)
    return error;
  endif

  if(!IsPlayer(opponent) and !IsPlayerTamed(opponent))
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function CanGainOnNpcWithCag(Npc, opponent := 0)

  var Res := CagIsAvailable(Npc, opponent);

  if(Res == error)
    return Res;
  endif

  if(!Res)
    return 1;
  endif

  if(GetCagAmount(Npc, opponent) > 0)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetCagMaxAmount(Npc)

  if(!Npc or !Npc.isa(POLCLASS_NPC))
    return error;
  endif

  return GetMaxHp(Npc) * 2;

endfunction

//--------------------------------------------------------------------------------

function IsAnyCagTamedEmpty(Npc)

  var Cags := GetCagEnemies(Npc);

  foreach Serial in (Cags)
    var Mob := FindPlayerBySerial(Serial);

    if(IsPlayerTamed(Mob))
      if(GetCagAmount(Npc, Mob) == 0)
        return 1;
      endif
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetCagAmount(Npc, opponent := 0)

  if(!CagIsAvailable(Npc, opponent))
    return error;
  endif

  var Amt := GetObjProperty(Npc, PROP_CAG_PREFIX + hex(opponent.serial));

  if(Amt == error)
    return GetCagMaxAmount(Npc);
  endif

  return CInt(Amt);

endfunction

//--------------------------------------------------------------------------------

function GetCagEnemies(Npc)

  var Props := GetObjPropertyNames(Npc);
  var List := array;
  var pLen := len(PROP_CAG_PREFIX);

  foreach Prop in Props
    if(substr(Prop, 1, pLen) == PROP_CAG_PREFIX)
      List.append(substr(Prop, pLen+1, len(Prop) - pLen));
    endif
  endforeach

  return List;

endfunction

//--------------------------------------------------------------------------------

function CanGainFromEnemies(Mob)

  if(GetObjProperty(Mob, PROP_NO_FIGHT))
    return 0;
  endif

  var Attackers := ListHostiles(Mob);

  foreach Attacker in Attackers
    if(IsMobGainable(Attacker))
      CagNpcDamaged(Attacker, 5+RandomInt(5), Mob);
      return 1;
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CagNpcDamaged(Npc, Dmg, opponent := 0)

  if(!CagIsAvailable(Npc, opponent))
    return error;
  endif

  var CagAmt := GetCagAmount(Npc, opponent);

  if(CagAmt > 0)
    CagAmt -= Dmg;
    SetObjProperty(Npc, PROP_CAG_PREFIX + hex(opponent.serial), Max(CagAmt, 0) );
  endif

endfunction

//--------------------------------------------------------------------------------

function CagNpcHealed(Npc, Amt, opponent := 0)

  if(!CagIsAvailable(Npc, opponent))
    return error;
  endif

  if(IsPlayerTamed(opponent) or IsAnyCagTamedEmpty(opponent))
    return error;
  endif

  if(WasTamed(Npc))
    return 0;
  endif

  var CagAmt := GetCagAmount(Npc, opponent);
  var MaxAmt := GetCagMaxAmount(Npc);

  if(CagAmt < MaxAmt)
    CagAmt += PercentInterpolate(Amt, CAG_HEAL_PERCENT);
    SetObjProperty(Npc, PROP_CAG_PREFIX + hex(opponent.serial), Min(CagAmt, MaxAmt) );
  endif

endfunction

//--------------------------------------------------------------------------------
