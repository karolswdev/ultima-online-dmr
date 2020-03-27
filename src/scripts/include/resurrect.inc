//-----------------------------------------------------------------------------

include ":character:common";
include ":eventcore:proc_events";
include ":eventcore:resurrection";

//=============================================================================
//-----------------------------------------------------------------------------
// Resurrection
//-----------------------------------------------------------------------------
//=============================================================================

const RES_TIMES           := 5;
const RES_TIME            := 900;
const RES_STOP_TIME       := 1800;


//-----------------------------------------------------------------------------

function CanResurrect(who)

  if(IsOnAutoEvent(who) or FindEventAreaByLoc(who))
    return 1;
  endif

  if((GetObjProperty(who, "ResTimer")) and (GetObjProperty(who, "ResTimer") < ReadGameClock()))
    EraseObjProperty(who, "ResTimer");
    EraseObjProperty(who, "ResTimes");
  endif

  if((GetObjProperty(who, "ResStopTimer")) and (GetObjProperty(who, "ResStopTimer") > ReadGameClock()))
    EraseObjProperty(who, "ResTimes");
    return 0;
  endif

  EraseObjProperty(who, "ResStopTimer");
  return 1;

endfunction

//-----------------------------------------------------------------------------

function Resurrection(who,Txt := 0)

  if(!CanResurrect(who))
    if(Txt)
      PrintTextAbove(who,"*slab"+ggm(who,6)+"*");
    endif
    return 0;
  endif

  if(ResurrectPlayer(who))
    if(!IsOnAutoEvent(who) and !FindEventAreaByLoc(who))
      SetObjProperty(who, "ResTimes", CInt(GetObjProperty(who, "ResTimes"))+1 );
      SetObjProperty(who, "ResTimer", ReadGameClock() + RES_TIME );
      if(GetObjProperty(who, "ResTimes") >= RES_TIMES)
        SetObjProperty(who, "ResStopTimer", ReadGameClock() + RES_STOP_TIME);
      endif
    endif
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ResurrectPlayer(who)

  var Res := Resurrect(who);

  if(!Res)
    return Res;
  endif

  var Graphic := GetRaceGraphic(who);

  if(Graphic and who.graphic != Graphic)
    who.graphic := Graphic;
  endif

  CheckAutoEventResurrection(who);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsWeakSoul(who)

  if ( who.dead and (GetObjProperty(who, "ResStopTimer") != error) and CInt(GetObjProperty(who, "ResStopTimer") > ReadGameClock()))
    return 1;
  endif
  return 0;
endfunction