//-----------------------------------------------------------------------------

use polsys;

//-----------------------------------------------------------------------------

include "include/const/eventid";
include "include/client/fonts";
include "include/start";

//-----------------------------------------------------------------------------

const PROP_STAFFNOTIFY                 := "StaffNotify";
const STAFFCORE_SCNAME                 := "::sc";

//-----------------------------------------------------------------------------

function SendStaffNotify(EvID, who, Param := 0)

  var Core := GetStaffCore();
  var stEv := struct;

  if(Core)
    stEv.+type   := EvID;
    stEv.+player := who;
    stEv.+param  := Param;
    Core.sendevent(stEv);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetStaffCore()

  return GetGlobCtrlProc(STAFFCORE_SCNAME);

endfunction

//-----------------------------------------------------------------------------

function CanStaffNotify(Person, who)

  if(who.cmdlevel)
    return CInt(GetObjProperty(Person, PROP_STAFFNOTIFY)) != 1 and who.concealed <= Person.cmdlevel;
  else
    return CInt(GetObjProperty(Person, PROP_STAFFNOTIFY)) == 2;
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
//                         Staff Alert
//-----------------------------------------------------------------------------
//=============================================================================

function AlertSeers(message,font := FONT_NORMAL,color := COLOR_GREEN)

  SendStaffNotify(EVID_SEND_SEER, 0, array(Message, Font, Color));

endfunction

//-----------------------------------------------------------------------------

function AlertGMs(message,font := FONT_NORMAL,color := COLOR_GREEN)

  SendStaffNotify(EVID_SEND_GM, 0, array(Message, Font, Color));
          
endfunction

//-----------------------------------------------------------------------------

function AlertAdmins(message,font := FONT_NORMAL,color := COLOR_GREEN)

  SendStaffNotify(EVID_SEND_ADMIN, 0, array(Message, Font, Color));

endfunction

//-----------------------------------------------------------------------------
