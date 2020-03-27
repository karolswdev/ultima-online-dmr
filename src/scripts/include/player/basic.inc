//-----------------------------------------------------------------------------

include "include/const/misc";
include "include/const/props";
include "include/player/nutrition";

//-----------------------------------------------------------------------------

const WEIGHT_FACTOR          := 8;

//-----------------------------------------------------------------------------

function GetAccountExistsTime(Acc)

  var CrTime := CInt(Acc.GetProp(PROP_CREATE_TIME));

  if(CrTime)
    return ReadGameClock() - CrTime;
  else
    return -1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCharDeleteDelay(Acc)

  var CrTime    := CInt(Acc.GetProp(PROP_CREATE_TIME));
  var DelayTime := CHAR_DELETE_TIME;

  if(CrTime)
    if(ReadGameClock() - CrTime < ACC_YOUNG_TIME)
      DelayTime := CHAR_DELYOUNG_TIME;
    endif
  endif

  return DelayTime;

endfunction

//-----------------------------------------------------------------------------

function GetOnlineTimer(who)

  return CInt(GetObjProperty(who, PROP_ONLINETIMER));

endfunction

//-----------------------------------------------------------------------------

function GetMaxWeight(who)

  return CInt(((GetStrength(who) * 7) / 2) + 40);

endfunction

//-----------------------------------------------------------------------------

function GetFullWeight(who)

  return CInt((GetStrength(who) * WEIGHT_FACTOR) * GetRaceWeightModifier(who));

endfunction

//--------------------------------------------------------------------------------

function InstantDisconnect(who)

  SetObjProperty(who, PROP_SAFELOGOFF, 1);
  DisconnectClient(who);

endfunction

//-----------------------------------------------------------------------------

function GetPlayerLogonTime(who)

  if(IsPlayer(who))
    var LastLog := GetObjProperty(who, PROP_LASTLOG);

    if(LastLog == error)
      return 0;
    endif

    LastLog := CInt(LastLog);

    if(IsLoggedIn(who))
      return ReadGameClock() - LastLog;
    else
      return LastLog - ReadGameClock();
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HidePlayer(who)

  who.hidden := 1;

  var Snooper := FindPlayerBySerial(GetObjProperty(who, "#BeingSnoop"));

  if(Snooper)
    SetObjProperty(Snooper,"#SnoopBreak", 1);
  endif

endfunction

//-----------------------------------------------------------------------------

function UnhidePlayer(who)

  who.hidden := 0;

endfunction

//-----------------------------------------------------------------------------
