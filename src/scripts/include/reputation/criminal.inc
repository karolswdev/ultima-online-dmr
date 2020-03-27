//-----------------------------------------------------------------------------

use os;

//-----------------------------------------------------------------------------

include ":guildstone:common";
include "include/const/misc";
include "include/finding";
include "include/mobile/tamed";

//-----------------------------------------------------------------------------

function CanGetCriminal(who, vic, Noto := TGTOPT_HARMFUL)

  var Victim := 0;

  if(vic.isa(POLCLASS_MOBILE))
    Victim := vic;
  elseif(CInt(vic) > 0 and CInt(vic) < ITEM_START_SERIAL)
    Victim := FindPlayerBySerial(vic);
  endif

  if(who.isa(POLCLASS_NPC))
    if(IsPlayerTamed(who))
      who := GetMaster(who);
    else
      return 0;
    endif
  endif

  if(Victim.isa(POLCLASS_NPC) and IsPlayerTamed(Victim))
    Victim := GetMaster(Victim);
  endif

  if(Victim.isa(POLCLASS_MOBILE) and IsPlayer(who) and Victim != who)
    if((Noto & TGTOPT_HARMFUL and IsOrder(Victim) and !IsAggressorTo(who, Victim)) or (Noto & TGTOPT_HELPFUL and !IsOrder(Victim)))

      if(IsPlayer(Victim))
        if(GuildAcp(who, Victim) and !(Noto & TGTOPT_HELPFUL))
          return 0;
        endif
      elseif(Victim.isa(POLCLASS_NPC) and !IsTamed(Victim))
        if(Noto & TGTOPT_HELPFUL and Victim.alignment != NPCAL_EVIL)
          return 0;
        endif
      endif

      return 1;

    elseif(IsPlayer(who) and Victim.criminal and IsPlayer(Victim) and who != Victim and (Noto & TGTOPT_HARMFUL))
      
      Victim.SetCriminal(1); // refresh dla krima

    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetCriminal(who, State := 1, FuncName := "N/A", Reason := "N/A")

  State := CInt(State);

  who.SetCriminal(State);

  if(State)
    LogCriminal(who, FuncName, Reason);
  endif

endfunction

//-----------------------------------------------------------------------------

function LogCriminal(who, FuncName := "N/A", Reason := "N/A")

  var Tamed := 0;

  if(!IsPlayer(who))
    if(who.isa(POLCLASS_NPC) and IsPlayerTamed(who))
      Tamed  := who;
      who    := GetMaster(who);
      Reason := "[Tamed: "+Tamed.name+"/"+Tamed.serial+"/"+Tamed.npctemplate+"]: "+Reason;
    else
      return;
    endif
  endif

  var LastCrims := GetObjProperty(who, PROP_LASTCRIMINALSETS);

  if(!LastCrims or !LastCrims.size())
    LastCrims := array;
  endif

  var Script     := GetProcess(GetPid());
  var ScriptName := Script.name;
  
  var pos, i := 0;
  
  for(pos:=Len(ScriptName); pos>0; pos-=1)
    if(ScriptName[pos] == "/")
      if(i > 0)
        break;
      endif
      i += 1;
    endif
  endfor

  if(pos)
    ScriptName[1, pos] := "";
  endif

  var Crim  := array;

  if(Tamed)
    Crim[1] := {Tamed.x, Tamed.y, Tamed.z, Tamed.realm};
  else
    Crim[1] := {who.x, who.y, who.z, who.realm};
  endif
  Crim[2]   := PolCore().systime;
  Crim[3]   := Reason;
  Crim[4]   := ScriptName+"::"+FuncName;
  Crim[5]   := Script.call_depth - 1;
  Crim[6]   := 0;  //attached_to
  Crim[7]   := 0;  //controller

  if(Script.attached_to)
    Crim[6] := Script.attached_to.serial;
  endif

  if(Script.controller)
    Crim[7] := Script.controller.serial;
  endif

  if(LastCrims.size() < CRIMSLOG_MAXSIZE)
    LastCrims.append(Crim);
  else
    for k:=0 to (LastCrims.size()-CRIMSLOG_MAXSIZE)
      LastCrims.erase(1);
    endfor
    LastCrims.append(Crim);
  endif

  SetObjProperty(who, PROP_LASTCRIMINALSETS, LastCrims);

endfunction

//-----------------------------------------------------------------------------

function AddAggressorTo(who, Aggressor, RepSys := 0)

  if(who.isa(POLCLASS_NPC))
    if(IsPlayerTamed(who))
      who := GetMaster(who);
    else
      return;
    endif
  endif

  if(Aggressor.isa(POLCLASS_NPC))
    if(IsTamed(Aggressor))
      Aggressor := GetMaster(Aggressor);
    else
      return;
    endif
  endif

  if(who == Aggressor or !IsPlayer(who) or !Aggressor.isa(POLCLASS_MOBILE))
    return;
  endif

  var AggressorList := GetObjProperty(who, PROP_AGGRESSORS);

  if(!AggressorList)
    AggressorList := dictionary;
  endif

  var AggressorTime := ReadConfigFile("::repsys")["General"].AggressorFlagTimeout;

  if(!AggressorTime)
    return;
  endif

  AggressorList[Aggressor.serial] := array(ReadGameClock()+CInt(AggressorTime), RepSys);

  SetObjProperty(who, PROP_AGGRESSORS, AggressorList);

endfunction

//-----------------------------------------------------------------------------

function IsAggressorTo(who, Aggressor, RepSys := -1)

  if(who.isa(POLCLASS_NPC))
    if(IsPlayerTamed(who))
      who := GetMaster(who);
    else
      return 0;
    endif
  endif

  if(Aggressor.isa(POLCLASS_NPC))
    if(IsTamed(Aggressor))
      Aggressor := GetMaster(Aggressor);
    else
      return 0;
    endif
  endif

  if(who == Aggressor or !IsPlayer(who) or !Aggressor.isa(POLCLASS_MOBILE))
    return 0;
  endif

  var AggressorList := GetObjProperty(who, PROP_AGGRESSORS);

  if(!AggressorList[Aggressor.serial])
    return 0;
  endif

  var ExpireTime    := CInt(AggressorList[Aggressor.serial][1]);
  var ExpireRepSys  := CInt(AggressorList[Aggressor.serial][2]);

  if(!ExpireTime)
    return 0;
  endif

  if(RepSys != -1)
    if(ExpireRepSys != RepSys)
      return 0;
    endif
  endif

  return (ExpireTime > ReadGameClock());

endfunction

//-----------------------------------------------------------------------------

function GetMobCriminalDesc(Mob)

  if(!Mob.isa(POLCLASS_MOBILE))
    return "error";
  endif

  var Desc := Mob.name+"/"+Mob.serial;

  if(IsPlayer(Mob))
    Desc := "[Player] "+Desc+"/"+Mob.acctname+" [K:"+Mob.criminal+", M:"+Mob.murderer+"].";
  elseif(Mob.isa(POLCLASS_NPC))
    Desc += "/"+Mob.npctemplate+" [K:"+Mob.criminal+", M:"+Mob.murderer;

    if(IsTamed(Mob))
      Desc := "[Tamed] "+Desc+"], Master: "+GetMobCriminalDesc(GetMaster(Mob));
    else
      var Align := "";
      case(Mob.alignment)
        NPCAL_GOOD:     Align := CITYAL_GOOD;
        NPCAL_EVIL:     Align := CITYAL_EVIL;
        NPCAL_NEUTRAL:  Align := CITYAL_NEUTRAL;
      endcase

      Desc := "[Npc] "+Desc+", A:"+Align+"].";
    endif
  endif

  return Desc;

endfunction

//-----------------------------------------------------------------------------
