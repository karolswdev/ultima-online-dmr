//-----------------------------------------------------------------------------
// Mobile Titles
//-----------------------------------------------------------------------------

include "include/tempmods";

//-----------------------------------------------------------------------------

function GetTitlePrefix(who)

  if(!who.isa(POLCLASS_MOBILE))
    return "";
  endif

  var Prefix;

  if(CanMod(who, MOD_TPREFIX) == 0)
    Prefix := GetModOldAmount(who, MOD_TPREFIX);
  else
    Prefix := who.title_prefix;
  endif

  if(!Prefix)
    return "";
  endif

  return Prefix;

endfunction

//-----------------------------------------------------------------------------

function SetTitlePrefix(who, nValue)

  if(CanMod(who, MOD_TPREFIX) == 0)
    SetModOldAmount(who, MOD_TPREFIX, nValue);
  else
    who.title_prefix := nValue;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetTitleRace(who)

  if(!who.isa(POLCLASS_MOBILE))
    return "";
  endif

  var Prefix;

  if(CanMod(who, MOD_TRACE) == 0)
    Prefix := GetModOldAmount(who, MOD_TRACE);
  else
    Prefix := who.title_race;
  endif

  if(!Prefix)
    return "";
  endif

  return Prefix;

endfunction

//-----------------------------------------------------------------------------

function SetTitleRace(who, nValue)

  if(CanMod(who, MOD_TRACE) == 0)
    SetModOldAmount(who, MOD_TRACE, nValue);
  else
    who.title_race := nValue;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetTitleSuffix(who)

  if(!who.isa(POLCLASS_MOBILE))
    return "";
  endif

  var Suffix;

  if(CanMod(who, MOD_TSUFFIX) == 0)
    Suffix := GetModOldAmount(who, MOD_TSUFFIX);
  else
    Suffix := who.title_suffix;
  endif

  if(!Suffix)
    return "";
  endif

  return Suffix;

endfunction

//-----------------------------------------------------------------------------

function SetTitleSuffix(who, nValue)

  if(CanMod(who, MOD_TSUFFIX) == 0)
    SetModOldAmount(who, MOD_TSUFFIX, nValue);
  else
    who.title_suffix := nValue;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetTitleGuild(who)

  if(!who.isa(POLCLASS_MOBILE))
    return "";
  endif

  var Guild;

  if(CanMod(who, MOD_TGUILD) == 0)
    Guild := GetModOldAmount(who, MOD_TGUILD);
  else
    Guild := who.title_guild;
  endif

  if(!Guild)
    return "";
  endif

  return Guild;

endfunction

//-----------------------------------------------------------------------------

function SetTitleGuild(who, nValue)

  if(CanMod(who, MOD_TGUILD) == 0)
    SetModOldAmount(who, MOD_TGUILD, nValue);
  else
    who.title_guild := nValue;
  endif

endfunction

//-----------------------------------------------------------------------------
