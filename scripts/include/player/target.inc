//--------------------------------------------------------------------------------

include "include/reputation/checks";
include "include/reputation/criminal";
include "include/checks/stdchecks";
include "include/attack";
include "include/bits";

//-----------------------------------------------------------------------------
// Player Flags
//-----------------------------------------------------------------------------

const PCF_TARGETING          := 0x00000001;
const PCF_WASMURD            := 0x00000002;

//--------------------------------------------------------------------------------

function ObjTarget(who, options := TGTOPT_CHECK_LOS+TGTOPT_NEUTRAL)

  if(!IsLoggedIn(who))
    return 0;
  endif

  if(IsSetTempFlag(who, PCF_TARGETING))
    return 0;
  endif

  SetObjProperty(who, "#RepSysNoLog", 1);

  var Obj := Target(who, options);

  if(GetObjProperty(who, "#RepSysNoLog"))
    EraseObjProperty(who, "#RepSysNoLog");
  endif

  return PostTargetCheck(who, Obj, options);

endfunction

//--------------------------------------------------------------------------------

function ObjTargetEx(who, options, byref Param)

  SetObjProperty(who, "#RepSysNoLog", 1);

  var Obj := PcTarget(who, options, Param);

  if(GetObjProperty(who, "#RepSysNoLog"))
    EraseObjProperty(who, "#RepSysNoLog");
  endif

  return PostTargetCheck(who, Obj, options);

endfunction

//--------------------------------------------------------------------------------

function PostTargetCheck(who, Obj, options)

  if(Obj)
    if(options & TGTOPT_HARMFUL)
      if(!CanVsAttack(who,Obj))
        SendSysMessage(who, "Nie mozesz " + ggm(obj, 19) + " zaatakowac.", FONT_NORMAL, COLOR_ORANGE);
        return 0;
      endif  
    endif

    if(CanGetCriminal(who, Obj, options))
      var Reason := "Err";

      if(options & TGTOPT_HARMFUL)
        Reason := "Wskazal ofensywnym celownikiem postac "+GetMobCriminalDesc(Obj);
      elseif(options & TGTOPT_HELPFUL)
        Reason := "Wskazal defensywnym celownikiem postac "+GetMobCriminalDesc(Obj);
      endif

      LogCriminal(who, "ObjTarget", Reason);
    endif

  endif

  return Obj;

endfunction

//--------------------------------------------------------------------------------

function LocTarget( who, options := TGTOPT_NOCHECK_LOS | TGTOPT_NEUTRAL)

  if(!IsLoggedIn(who))
    return 0;
  endif

  if(IsSetTempFlag(who, PCF_TARGETING))
    return 0;
  endif

  var Loc := TargetCoordinates(who);
  
  if(Loc.mobile)
    if(IsPlayer(Loc.mobile) and (options & TGTOPT_HARMFUL))
      if(who != Loc.mobile)
        if(!CanVsAttack(who,Loc.mobile))
          SendSysMessage(who, "Nie mozesz " + ggm(Loc.mobile,19) + " zatakowac.", FONT_NORMAL, COLOR_ORANGE);
          Loc.mobile := 0;
        endif
      endif
    endif
    if(CanGetCriminal(who, Loc.mobile, options))
      var Reason := "Err";
      if(options & TGTOPT_HARMFUL)
        Reason := "Wskazal ofensywnym celownikiem postac "+GetMobCriminalDesc(Loc.mobile);
      elseif(options & TGTOPT_HELPFUL)
        Reason := "Wskazal defensywnym celownikiem postac "+GetMobCriminalDesc(Loc.mobile);
      endif
      SetCriminal(who, 1, "LocTarget", Reason);
    endif
  endif

  if(options & TGTOPT_CHECK_LOS)
    if(!CheckLosAt(who,Loc.x,Loc.y,Loc.z))
      return 0;
    endif
  endif

  return Loc;

endfunction

//--------------------------------------------------------------------------------
