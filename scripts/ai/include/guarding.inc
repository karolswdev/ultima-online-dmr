//-----------------------------------------------------------------------------

include "include/findcity";
include "include/reputation/murderer";
include ":jail:common";
include ":magic:common";
include ":major:major";

include "ai/include/tamed";

//-----------------------------------------------------------------------------

const NPCF_GUARD_ZONE        := 0x00001000;
const GUARD_CRIM_CHK_DELAY   := 1800;

const LFCF_NOSPEAK           := 0x00000001;
const LFCF_THIEF             := 0x00000002;

const RMBF_DONTERASEMEM      := 0x00000001;

//-----------------------------------------------------------------------------

var City      := FindCity(Self());
var Criminals := GetProperty(PROP_GUARD_CRIMINALS);
var CrimCheck := ReadGameClock();
var MemReason := 0;

if(!Criminals)
  Criminals := dictionary;
endif

//-----------------------------------------------------------------------------

function CheckGuardZone()

  SetFlag(me, NPCF_GUARD_ZONE);
  
endfunction

//-----------------------------------------------------------------------------

function LookForCriminals(Flags := 0)

  var extendedRange := CInt(GetProperty("ExRange"));

  if(!IsSetFlag(me, NPCF_GUARD_ZONE))
    if(!(Flags & LFCF_NOSPEAK))
      Say("Tego miejsca ja nie musze strzec, radz sobie sam!");
    endif
    return 0;
  endif

  if(!me.frozen)
    foreach npc in ListMobilesNearLocationEx(me.x,me.y,me.z,HALT_THRESHOLD + extendedRange,LISTEX_FLAG_NORMAL + LISTEX_FLAG_HIDDEN,me.realm)
      if(CheckGuardFight(npc, Flags))
        return 1;
      endif
    endforeach
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckGuardFight(Npc, Flags := 0)

  __ParamUnused(Flags);
  
  /*
  if(IsMajor(Npc))
	return 0;
  endif
  */

  if(Npc == me or Npc.dead or Npc.cmdlevel or Npc.concealed or Npc.enabled("invul") or IsJailed(Npc))
    return 0;
  endif

  if(Npc.hidden)
    return 0;
  endif

  if(MeHaveFlag("GuardOnlyPvp"))

    var Player := GetAggressivePlayer(Npc, GAP_CRIMINAL);

    if(Player)
      Say(GetName(Player)+", a ty co tu za bitwy sobie urzadzasz!?");
      MemReason := GMR_PLAYER_ATTACK;
      Fight(Player);
    endif

  elseif(IsPlayer(npc))

    if(MeHaveFlag("NoPlayerAttack"))
      return 0;
    endif

    if(IsInMemory(npc))
      MemReason := GMR_INMEMORY;
      Fight(npc);
      return 1;
    elseif(CanFightWith(npc))
      if(npc.name)
        if(MemReason == GMR_STEALING)
          Say("Juz mi nie uciekniesz!");
        else
          if(!City)
            Say("Wynos sie stad "+npc.name+"!");
          else
            Say("Wynos sie z tego miasta "+npc.name+"!");
          endif
        endif
      endif
      Fight(npc);
      return 1;
    endif

  elseif(npc.isa(POLCLASS_NPC))

    if(IsTamed(npc))
      var Master := GetMaster(npc);
      if(Master and (coordist(Master.x, Master.y, npc.x, Npc.y) < 30 and Master.realm == Npc.realm))
        if(IsPlayer(Master) and MeHaveFlag("NoPlayerAttack"))
          return 0;
        endif

        if(IsInMemory(Master))
          MemReason := GMR_INMEMORY;
          Remember(Master, RMBF_DONTERASEMEM);
          Fight(npc);
          return 1;
        elseif(CanFightWith(Master))
          Remember(Master, RMBF_DONTERASEMEM);
          Fight(npc);
          return 1;
        endif
      else
        Master := 0;
      endif

      if(CanFightWith(npc))
        if(Master)
          Remember(Master, RMBF_DONTERASEMEM);
        endif

        Fight(npc, 1);
        return 1;
      endif
    elseif((!NpcHaveFlag(npc.npctemplate, "guardignore") and !MeHaveFlag("GuardNotFromNpc") and !IsSetFlag(npc, NPCF_GUARDIGNORE)) or CanFightWithMounted(npc))
      Fight(npc);
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

const GAP_CRIMINAL := 0x01;

//-----------------------------------------------------------------------------

function GetAggressivePlayer(npc, flags := 0)

  if(IsPlayer(npc))
    var Victim := npc.opponent;

    if(IsTamed(Victim))
      var Master := GetMaster(Victim);

      if(Master and (coordist(Master.x, Master.y, Victim.x, Victim.y) < 30 and Master.realm == Victim.realm))
        Victim := Master;
      endif
  
    endif

    if(IsPlayer(Victim) and (!IsBitOn(flags, GAP_CRIMINAL) or (!Victim.murderer and !Victim.criminal)) )
      return npc;
    elseif(!IsBitOn(flags, GAP_CRIMINAL) or (!npc.murderer and !npc.criminal))
      var Attackers := ListHostiles(npc);

      foreach Attacker in Attackers
        if(IsPlayer(Attacker))
          return Attacker;
        elseif(IsTamed(Attacker))
          var Master := GetMaster(Attacker);

          if(IsPlayer(Master) and (coordist(Master.x, Master.y, Attacker.x, Attacker.y) < 30 and Master.realm == Attacker.realm))
            return Master; 
          elseif((!IsBitOn(flags, GAP_CRIMINAL) or Attacker.murderer or Attacker.criminal or GetObjProperty(Attacker, "SummonOffensive")) )
            return Attacker;
          endif
        endif
      endforeach
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(who)

  MemReason := 0;

  if(who.hidden or who.dead or who.concealed or who.cmdlevel or IsNpcGuard(who) or who.enabled("invul") or IsJailed(who))
    return 0;
  endif

  if(MeHaveFlag("GuardOnlyPvp"))

    if(IsPlayer(who))
      var Victim := GetAggressivePlayer(who, GAP_CRIMINAL);

      if(IsPlayer(Victim) and !Victim.criminal and !Victim.murderer)
        MemReason := GMR_PLAYER_ATTACK;
        return 1;
      endif
    endif

    return 0;
  endif

  if(IsPlayer(who) and MeHaveFlag("NoPlayerAttack"))
    return 0;
  endif

  if(IsGuardChasing(who))
    MemReason := GMR_CITYCHASING;
    return 1;
  endif

  /*
  if(GetEquipmentByLayer(who,LAYER_MOUNT))
    if(IsInBuilding(who.x,who.y,who.realm))
      Say("Gdzie z tym zwierzakiem?!");
      if(CityAl == CITYAL_NEUTRAL)
        SetGuardChase(who, GDCF_ALLCITIES);
      else
        SetGuardChase(who);
      endif
      MemReason := GMR_ANIMAL_INB;
      return 1;
    endif
  elseif(CanFightWithMounted(who))
    MemReason := GMR_ANIMAL_INB;
    return 1;
  elseif(IsCastingOffensiveNear(who, 15, ICN_ONLYCITY))
    MemReason := GMR_MAGIC_CAST;
    return 1;
  endif
  */

  var Player := who;

  if(IsTamed(who))
    Player := GetMaster(who);
    if(GetObjProperty(who, "SummonOffensive"))
      MemReason := GMR_MAGIC_CAST;
      return 1;
    endif
  endif  

  var cfgElem := GetNpcConfig(me);

  if(me.murderer)
    if(!who.murderer)
      MemReason := GMR_NOT_MURD;
      return 1;
    elseif(IsAttackingPlayer(who))
      MemReason := GMR_PLAYER_ATTACK;
      return 1;
/*
    elseif(IsPlayer(who))
      if(GetMurdersCount(who) < 15)
        MemReason := GMR_TO_WEAK_MURD;
        return 1;
      endif
*/
    endif
    return 0;    
  elseif(CInt(cfgElem.GuardIgnoreSkReq))
    if(GetBaseSkill(Player, CInt(cfgElem.GuardIgnoreSkReq)) >= 40)
      if(IsAttackingPlayer(Player))
        MemReason := GMR_PLAYER_ATTACK;
        return 1;
      elseif(IsCastingOffensiveNear(Player, 15, ICN_ONLYCITY))
        MemReason := GMR_MAGIC_CAST;
        return 1;
      else
        return 0;
      endif
    endif
  endif
  
  if(who.murderer)
    MemReason := GMR_MURD;
    return 1;
  endif
  
  if(IsPlayer(who))
    if(CInt(GetObjProperty(who,"#WasStealing")) > ReadGameClock())
      var bFound := 1;

      if(who.hidden)
        var SkVal := GetEffectiveSkill(who, SKILLID_STEALTH);
        if(!HaveChance(100 - SkVal*0.7))
          bFound := 0;
        endif
      endif

      if(bFound)
        Say("Stoj zlodzieju!");
        MemReason := GMR_STEALING;
        return 1;
      endif
    endif
  endif

  if(who.criminal)

    if(IsAttackingPlayer(who))
      MemReason := GMR_PLAYER_ATTACK;
      return 1;
    elseif(CheckGuardRemind(who))
      MemReason := GMR_CRIMINAL;
      return 1;
    endif

  elseif(IsAttackingSkGuarded(who))

    MemReason := GMR_PLAYER_ATTACK;
    return 1;

  endif

  if(IsCastingOffensiveNear(who, 15, ICN_ONLYCITY))
    MemReason := GMR_MAGIC_CAST;
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsAttackingSkGuarded(Player)

  var cfgElem := GetNpcConfig(me);

  if(CInt(cfgElem.GuardIgnoreSkReq))
    var Attacked := GetAttackedPlayer(Player);

    if(Attacked and GetBaseSkill(Attacked, CInt(cfgElem.GuardIgnoreSkReq)) >= 40)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWithMounted(Npc)

  if(Npc.isa(POLCLASS_NPC))
    if(GetMountType(Npc.graphic))
      if(IsInBuilding(Npc.x,Npc.y,Npc.realm))
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckGuardRemind(who)

  if(who.isa(POLCLASS_NPC))
    return 1;
  endif

  var GLvl   := GetPlayerNpcGuildCityLvl(who, City);
  var Attack := 0;

  if(GLvl)
    LoseGuildExpInCity(who, City, NGD_CITY_LEXP_VALUE);
    if(GLvl >= NGD_CITY_RESPECT_LVL)
      var rTime := CInt(GetObjProperty(who, PROP_GUARD_REMTIME));
      var rCnt  := CInt(GetObjProperty(who, PROP_GUARD_REMIND));

      if(rTime + GUARD_REMIND_DELAY < ReadGameClock())
        if(rTime + GUARD_REMIND_TIMEOFF < ReadGameClock())
          rCnt := 0;
        endif

        if(rCnt > MAX_GUARD_REMIND + (GLvl-NGD_CITY_RESPECT_LVL)*5)
          rCnt   := 0;
          Attack := 1;
          Say("No to sie doigral"+ggm(who,3)+"!");
        else
          Say("Oby mi to bylo ostatni raz!");
          rCnt := rCnt + 1;
        endif

        SetObjProperty(who, PROP_GUARD_REMTIME, ReadGameClock());
        SetObjProperty(who, PROP_GUARD_REMIND, rCnt);
      endif
    else
      Attack := 1;
    endif
  else
    Attack := 1;
  endif

  return Attack;

endfunction

//-----------------------------------------------------------------------------

function Remember(who, Flags := 0)

  if(IsPlayer(who))
    if(!Criminals[who.serial])
      Criminals[who.serial] := array(0, 0, 0);
    endif
    Criminals[who.serial][1] := ReadGameClock();
    Criminals[who.serial][3] := Criminals[who.serial][3] + 1;
    if(!Criminals[who.serial][2] or (MemReason and Criminals[who.serial][3] == 1))
      Criminals[who.serial][2] := MemReason;
    endif
    SetObjProperty(me, PROP_GUARD_CRIMINALS, Criminals);
    SendBad(who, "Zostal"+ggm(who,3)+" zapamietany przez straznika!");
    SendBad(who, "Lepiej nie pokazuj sie tutaj przez najblizsze poltora dnia!");
  endif

  if(!(Flags & RMBF_DONTERASEMEM))
    MemReason := 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function IsInMemory(who)

  if(Criminals.exists(who.serial))
 
    if(Criminals[who.serial][1] + GUARD_REMEMBER_TIME > ReadGameClock())
      return 1;
    endif
   
    Forget(who);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetRememberTime(who)

  if(!Criminals.exists(who.serial))
    return 0;
  else
    return Criminals[who.serial][1];
  endif

endfunction

//-----------------------------------------------------------------------------

function Forget(who)

  if(Criminals.exists(who.serial))
    Criminals.erase(who.serial);
    SetObjProperty(me,PROP_GUARD_CRIMINALS,Criminals);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckMemory()

  if(CrimCheck < ReadGameClock())
    var Keys := Criminals.keys();

    foreach Serial in Keys
      if(Criminals[Serial][1] + GUARD_REMEMBER_TIME < ReadGameClock())
        Criminals.erase(Serial);
      elseif(!FindPlayerBySerial(Serial))
        Criminals.erase(Serial);
      endif
    endforeach

    SetObjProperty(me,PROP_GUARD_CRIMINALS,Criminals);
    CrimCheck := ReadGameClock() + GUARD_CRIM_CHK_DELAY;
  endif

endfunction

//-----------------------------------------------------------------------------
