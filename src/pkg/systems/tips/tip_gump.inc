//-----------------------------------------------------------------------------
// Modified: 2006-11-22
//-----------------------------------------------------------------------------

include "include/gumps";
include "include/gump_utils";
include "include/clock";

//================================================================================
//--------------------------------------------------------------------------------
// Build Quest Gump Utility
//--------------------------------------------------------------------------------
//================================================================================

function BuildTipElemModSegInfo(byref Info, Title, sX, sY)

  if(Info)
    var Person := FindPlayerBySerial(Info[3]);
    GTextLine(sX, sY, 999, Title);
    if(Info[5])
      GTextLine(sX+250, sY, 670, Info[5]);
    endif
    GTextLine(sX, sY+20, 1419, CUnixTime(Info[1]));
    if(Person)
      GTextLine(sX+140, sY+20, 670, Person.name);
    endif
    if(Info[2])
      GTextLine(sX+290, sY+20, 670, Info[2]);
    endif
    if(Info[4])
      GTextLine(sX+360, sY+20, 670, Info[4]);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function BuildTipElemModInfo(Elem, sX, sY)

  var CrInfo  := Elem.GetProp(TIAPROP_CRINFO);
  var UpInfo  := Elem.GetProp(TIAPROP_UPINFO);
  var UseInfo := Elem.GetProp(TIAPROP_USEINFO);

  BuildTipElemModSegInfo(CrInfo,  "Stworzone:",    sX, sY);
  BuildTipElemModSegInfo(UpInfo,  "Modyfikowane:", sX, sY+50);
  BuildTipElemModSegInfo(UseInfo, "Uzywane:",      sX, sY+100);

endfunction

//--------------------------------------------------------------------------------

function SelectTipEvent(who)

  var Res := BQSelectEntry(Who, GetTipEventNames());

  if(Res)
    return UnZerofill(Res);
  endif

  return Res;

endfunction

//--------------------------------------------------------------------------------
