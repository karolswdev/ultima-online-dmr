//-----------------------------------------------------------------------------

include "include/reputation/penalty";

//-----------------------------------------------------------------------------

const MREP_LASTA_DELAY       := 1200;
const MREP_MAX_DIST          := 20;

//-----------------------------------------------------------------------------

function CanReport(who, Killer)

  if(GetObjProperty(who, PROP_NOREPORT))
    return 0;
  endif

  if(!IsPlayer(who) or !IsPlayer(Killer))
    return 0;
  endif

  if(!IsOrderGuarded(who, Killer))
    return 0;
  endif

  if((who.criminal and !Killer.murderer) or IsRealMurderer(who) or who == Killer)
    return 0;
  endif

  if(Killer.cmdlevel)
    return 0;
  endif

  if(GuildAcp(who, Killer))
    return 0;
  endif

  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function NoMurderReport(who)

  SetObjProperty(who, PROP_NOREPORT, ReadGameClock());

endfunction

//-----------------------------------------------------------------------------

function SetMurderer(who)

  if(IsRealMurderer(who))
    return;
  endif

  UndoMod(who, MOD_STATUS, UMDF_WAIT);
  SetPaladinMurderer(who);
  who.SetMurderer(1);
  ValidateRace(who);

endfunction

//-----------------------------------------------------------------------------

function EraseMurderer(who)

  if(!IsRealMurderer(who))
    return;
  endif

  UndoMod(who, MOD_STATUS, UMDF_WAIT);
  who.SetMurderer(0);
  ValidateRace(who);

  if(IsPlayer(who))
    LoseDarkKnightFaith(who);
    ErasePaladinMurderer(who);
    SetObjProperty(who,"longmurders",0);
    EraseObjProperty(who, "decaylongcountat");
    SetFlag(who, PCF_WASMURD);
  endif

endfunction

//-----------------------------------------------------------------------------

function AdjustMurderCounts(who, victim)

  __ParamUnused(victim);

  var onlinetime := GetObjProperty(who, PROP_ONLINETIMER);
  var longcount := CInt(GetObjProperty(who,"longmurders")) + 1;

  if(longcount >= MAX_MURD_TIMES)
    var GSt := GetMemberStatus(who);
    if(GSt)
      if(GSt != CHAOS_ST)
        DismissGuildMember(FindGuild(GetGuildId(who)),who,1);
      else
        SetObjProperty(who,"GuildStatus",CHAOS_ST);
      endif
    endif

    if(IsRealMurderer(who))
      SendSysmessage(who,"Teraz jestes znan"+ggm(who,6)+" jako morderca!",FONT_NORMAL,COLOR_RED);
    endif
    SetMurderer(who);
    ResetMurderLongTime(who, longcount);
  else
    SetObjProperty(who,"longmurders",longcount);
    EraseObjProperty(who,"decaylongcountat");
    
    var shortcount := CInt(GetObjProperty(who,"shortmurders")) + 1;
    SetObjProperty(who,"shortmurders",shortcount);
    SetObjProperty(who,"decayshortcountat",onlinetime + SHORT_COUNT_DECAY_TIME);
  endif

  SetObjProperty(who,"blockfallmurderer", ReadGameClock() + HOW_LONG_WITHOUT_KILL);

  ValidateRace(who);

endfunction

//-----------------------------------------------------------------------------

function AdjustMurdList(who, victim)

  var longcount := CInt(GetObjProperty(who,"longmurders"));

  var Murders := GetObjProperty(who,"Murders");
  if(!Murders or !Murders.size())
    Murders := {};
  endif

  Murders.append(array( victim.serial, longcount, ReadGameClock(), {victim.x, victim.y, victim.z, victim.realm}, GetRealName(victim), {victim.criminal, victim.murderer, GuildAcp(victim, who)} ));
  SetObjProperty(who, "Murders", Murders);

endfunction

//-----------------------------------------------------------------------------

function GetMurders(who)

  var Murders := GetObjProperty(who, "Murders");

  if(!Murders)
    Murders := array;
  endif

  return Murders;

endfunction

//-----------------------------------------------------------------------------

function GetBadMurdCount(who)

  var Murders := GetObjProperty(who, "Murders");
  var Cnt := 0;

  if(Murders)
    foreach Murd in Murders
      if(Murd[6] != error)
        if(!Murd[6][1] and !Murd[6][2] and !Murd[6][3])
          Cnt := Cnt + 1;
        endif
      endif

      Sleepms(1);
    endforeach
  endif

  return Cnt;

endfunction

//-----------------------------------------------------------------------------

function GetMurdReports(who)

  var Reports := GetObjProperty(who, PROP_REPORT_MURDER);
  var rReps   := dictionary;
  var Attacker;

  foreach Report in Reports
    if(Report + MREP_LASTA_DELAY > ReadGameClock())
      Attacker := FindPlayerBySerial(_Report_iter);
      if(Attacker)
        if(Distance(who, Attacker) < MREP_MAX_DIST)
          rReps[_Report_iter] := Report;
        endif
      endif
    endif
  endforeach

  SetObjProperty(who, PROP_REPORT_MURDER, rReps);
  return rReps;

endfunction

//-----------------------------------------------------------------------------

function GetMurdReportKillers(byref Reports)

  var Killers := array;
  var Serials := Reports.keys();

  foreach Serial in Serials
    Killers.append(FindPlayerBySerial(Serial));
  endforeach

  return Killers;

endfunction

//-----------------------------------------------------------------------------

function AddMurdReport(who, Attacker)

  CheckAggresiveAction(Attacker, who);

  if(who == Attacker or !IsPlayer(who) or !IsPlayer(Attacker))
    return;
  endif

  var Reports := GetObjProperty(who, PROP_REPORT_MURDER);

  if(!Reports || !Reports.size())
    Reports := dictionary;
  endif

  Reports[Attacker.serial] := ReadGameClock();
  SetObjProperty(who, PROP_REPORT_MURDER, Reports);

endfunction

//-----------------------------------------------------------------------------

function RemMurdReport(who, Attacker)

  var Reports := GetObjProperty(who, PROP_REPORT_MURDER);

  if(Reports)
    if(Reports[Attacker.serial])
      Reports.erase(Attacker.serial);
      SetObjProperty(who, PROP_REPORT_MURDER, Reports);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function RemoveMurdReports(who)

  EraseObjProperty(who, PROP_REPORT_MURDER);
  EraseObjProperty(who, PROP_LOGIN_REPORT_MURDER);

endfunction

//-----------------------------------------------------------------------------

function GetShortMurdersCount(who)

  return CInt(GetObjProperty(who, "shortmurders"));

endfunction

//-----------------------------------------------------------------------------

function GetMurdersCount(who)

  return CInt(GetObjProperty(who, "longmurders"));

endfunction

//-----------------------------------------------------------------------------

function CheckMurderLongCount(who)

  if(GetObjProperty(who, "PermRed"))
	who.SetMurderer(1);
  endif

  if(IsRealMurderer(who))
    var Count := GetObjProperty(who, "longmurders");
    ResetMurderLongTime(who, Count, 0);
  endif

endfunction

//-----------------------------------------------------------------------------

function ResetMurderLongTime(who, Count, bForce := 1)

  Count := CInt(Count);
  SetObjProperty(who, "longmurders", Count);

  Count := Max(Count - MAX_MURD_TIMES, 0);

  var onlinetime := GetObjProperty(who, PROP_ONLINETIMER);
  var Time := CInt(LONG_COUNT_DECAY_TIME + MURD_TIME_MULTIPLIER * Count);
  Time := Min(Time, LONG_COUNT_DECAY_TIME_MAX);
  var oldCount := CInt(GetObjProperty(who, "decaylongcountat"));

  if(onlinetime + Time < oldCount or bForce or !oldCount)
    SetObjProperty(who, "decaylongcountat", onlinetime + Time);
  endif

endfunction

//-----------------------------------------------------------------------------
