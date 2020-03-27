//-----------------------------------------------------------------------------

function SetDelayedAttack(Npc, Time)

  var Res := CInt(GetObjProperty(Npc, "#NoAttack"));
  if(Res == 1 or Res == -1)
    return 0;
  endif

  SetObjProperty(Npc, "#NoAttack", ReadGameClock()+Time);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetInvulToAttack(Npc)

  SetObjProperty(Npc, "#NoAttack", 1);

endfunction

//-----------------------------------------------------------------------------

function EraseDelayedAttack(Npc)

  EraseObjProperty(Npc, "#NoAttack");

endfunction

//-----------------------------------------------------------------------------

function EraseInvulToAttack(Npc)

  EraseObjProperty(Npc, "#NoAttack");

endfunction

//-----------------------------------------------------------------------------

function IsInvulToAttack(Npc)

  if(!Npc.isa(POLCLASS_NPC))
    return 0;
  endif
  
  var Delayed := CInt(GetObjProperty(Npc, "#NoAttack"));

  if(Delayed == 1)
    if(HaveChance(20))
      PrintTextAbove(Npc, "*pssss*");
    endif
    PlayPcEffect(Npc, FX_HEAL_EFFECT, 1322, 24, 24, 3);
    PlaySoundEffect(Npc, SFX_DMG_ABSORBED);
    return 1;
  elseif(Delayed == -1)
    PlayPcEffect(Npc, FX_MARK_EFFECT, 999, 24, 24, 3);
    PlaySoundEffect(Npc, SFX_DMG_ABSORBED);
    return 1;
  elseif(Delayed)
    if(Delayed > ReadGameClock())
      if(HaveChance(10))
        PrintTextAbove(Npc, "*pssss*");
      endif
      PlayPcEffect(Npc, FX_SPARK_EFFECT, 1244, 24, 24, 11);
      PlaySoundEffect(Npc, SFX_DMG_ABSORBED);
      return 1;
    else
      EraseObjProperty(Npc, "#NoAttack");
    endif
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

const LSDMF_ONLY_PLAYERS := 0x01;

//-----------------------------------------------------------------------------

function ListSpecialDamageMobs(me, Range, Flags := 0)

  var Objects := ListMobilesNearLocationEx( me.x, me.y, LIST_IGNORE_Z, range, LISTEX_FLAG_HIDDEN | LISTEX_FLAG_NORMAL, me.realm);
  var Mobs := array;

  foreach Obj in Objects
    if(IsPlayer(Obj))
      Mobs.append(Obj);
    endif

    if(Flags & LSDMF_ONLY_PLAYERS)
      continue;  
    endif

    if(IsTamed(Obj))
      if(IsPlayer(GetMaster(Obj)))
        Mobs.append(Obj);
      endif
    endif
  endforeach

  return Mobs;

endfunction

//-----------------------------------------------------------------------------

function NpcAttackRandomMobile(Npc)

  var Mobs := ListSpecialDamageMobs(Npc, 15);

  if(Mobs.size())
    var Mob := Mobs[RandomInt(Mobs.size())+1];
    SendChEvent(Npc, Mob, EVID_DAMAGED);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetNpcAggroTable(Npc)

  return GetObjProperty(Npc, "AggroTable");

endfunction

//-----------------------------------------------------------------------------

function GetAggroLevelForPlayer(Npc, Player, TimeExpires := 0)

  var Table := GetNpcAggroTable(Npc);

  if(Table)
    if(Table.exists(Player.serial))
      if(TimeExpires)
        if(ReadGameClock() - Table[1] > TimeExpires)
          return 0;
        endif
      endif

      return Table[Player.serial][2];
    endif
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

