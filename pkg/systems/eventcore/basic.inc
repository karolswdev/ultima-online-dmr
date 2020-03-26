//-----------------------------------------------------------------------------
// Modified: 16 czerwca 2013
//-----------------------------------------------------------------------------
// uwazac strasznie co tu sie dodaje!
// pliczek ten includowany jest przez movement, a movement.. przez 3/4 oskryptu z racji wpiecia w MoveObject
//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------
// Basic event messages
//-----------------------------------------------------------------------------

const MSGID_START_EVENT    := 1;
const MSGID_STOP_EVENT     := 2;
const MSGID_RELOAD_EVENTS  := 3;
const MSGID_FORCE_NEXT     := 4;

//-----------------------------------------------------------------------------
// basic event player messages
//-----------------------------------------------------------------------------

const MSGID_PLAYER_ENTERED   := 10;
const MSGID_PLAYER_LEFT      := 11;
const MSGID_PLAYER_DEATH     := 12;
const MSGID_PLAYER_KICK      := 13;
const MSGID_PLAYER_RESURRECT := 14;
const MSGID_PLAYER_LOGIN     := 15;
const MSGID_PLAYER_LOGOFF    := 16;

const MSGID_ITEM_INSERTED    := 20;
const MSGID_ITEM_REMOVED     := 21;

//-----------------------------------------------------------------------------
// Global props and names for event data
//-----------------------------------------------------------------------------

const EVENTS_DATA              := ":eventcore:data";
const EVENTS_GLOBAL_PROPS      := "Global";
const EVENT_TITLE              := "Event_";

//-----------------------------------------------------------------------------

function GetEventsData()

  var Data := OpenDataFile(EVENTS_DATA);

  if(!Data)
    Data := CreateDataFile(EVENTS_DATA);
  endif

  if(!Data)
    LogError("GetEventsData: Nie udalo sie stworzyc EventsData ["+EVENTS_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetEventElement(Key)

  var Data := GetEventsData();
  return Data.FindElement(EVENT_TITLE+Key);

endfunction

//-----------------------------------------------------------------------------

function SendEventMessage(EvID, MsgID, pData := 0)
  
  var Event := GetEventElement(EvID);
  var Proc := GetProc(Event.GetProp("ProcessId"));

  if(Proc)
  
    var Data := struct;
    Data.+evid := EvID;
    Data.+msgid := MsgID;
    Data.+data := pData;

    var Res := Proc.sendevent(Data);
    return Res;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
