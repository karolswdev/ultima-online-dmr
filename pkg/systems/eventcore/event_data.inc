//-----------------------------------------------------------------------------

include "include/logerror";
include "basic";

//-----------------------------------------------------------------------------
// Events Data Properties
//-----------------------------------------------------------------------------

const EVAPROP_CRINFO         := "CrInfo";
const EVAPROP_UPINFO         := "UpInfo";
const EVAPROP_USEINFO        := "UseInfo";
const EVAPROP_FLAGS          := "Flags";

const EVGPROP_CRINFO         := "EventCrInfo";
const EVGPROP_UPINFO         := "EventUpInfo";
const EVGPROP_NEXTID         := "EventNextID";

const PROP_EVENTS_DATA         := "EventsData";

const TIBPROP_EVENT_ID         := "EventId";

const EVENT_EVENTS_CFG         := ":events:events";
const EVENT_MAIN_CFG           := ":events:events";

//-----------------------------------------------------------------------------

function GetEventsGlobalProps()

  var Data      := GetEventsData();
  var GlobProps := 0;

  if(Data)
    GlobProps := Data.FindElement(EVENTS_GLOBAL_PROPS);

    if(!GlobProps)
      GlobProps := Data.CreateElement(EVENTS_GLOBAL_PROPS);
    endif

    if(GlobProps)
      return GlobProps;
    endif
  endif

  LogError("GetEventsGlobalProps: Podczas tworzenia GlobProps: "+GlobProps+" w EventsData: "+Data);
  return error;

endfunction

//-----------------------------------------------------------------------------

function GetNextEventID()

  var GlobProps := GetEventsGlobalProps();
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(EVGPROP_NEXTID)) + 1;
    if(!GlobProps.SetProp(EVGPROP_NEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function SetEventElemCrInfo(byref Elem, GProp, who := 0, Id := 0)

  var CrInfo    := array(PolCore().systime, 0, 0);
  if(IsPlayer(who))
    CrInfo[2] := who.acctname;
    CrInfo[3] := who.serial;
  endif
  Elem.SetProp(EVAPROP_CRINFO, CrInfo);

  if(GProp)
    var GProps := GetEventsGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    CrInfo[4]  := CInt(oInfo[4])+1;
    CrInfo[5]  := Id;
    GProps.SetProp(GProp, CrInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetEventElemUpInfo(byref Elem, GProp, who := 0, Id := 0)

  var UpInfo    := Elem.GetProp(EVAPROP_UPINFO);
  if(!UpInfo)
    UpInfo := array(0, 0, 0, 0);
  endif

  if(IsPlayer(who))
    UpInfo[2] := who.acctname;
    UpInfo[3] := who.serial;
  else
    UpInfo[2] := 0;
    UpInfo[3] := 0;
  endif

  UpInfo[1] := PolCore().systime;
  UpInfo[4] := CInt(UpInfo[4])+1;

  Elem.SetProp(EVAPROP_UPINFO, UpInfo);

  if(GProp)
    var GProps := GetEventsGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    UpInfo[4]  := CInt(oInfo[4])+1;
    UpInfo[5]  := Id;
    GProps.SetProp(GProp, UpInfo);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Events Data Initialization
//-----------------------------------------------------------------------------
//=============================================================================

function PrepareEventData(byref tData)

  tData := struct;

  tData.+Name;
  tData.+Text;
  tData.+EventScript;
  tData.+BeforeInterval;
  tData.+Delay;
  tData.+Hour;
  tData.+Flags;
  tData.+GlobalMsg;
  tData.+Areas;
  tData.+Variables;

endfunction

//-----------------------------------------------------------------------------

function InitEventData(byref tData)

  if(!tData)
    PrepareEventData(tData);
  endif

  tData.Name        := "";
  tData.Text        := array;
  tData.EventScript := "";
  tData.BeforeInterval := 0;
  tData.Hour        := 0;
  tData.Delay       := 0;
  tData.Flags       := 0;
  tData.GlobalMsg   := "";
  tData.Areas       := array;
  tData.Variables   := dictionary;

endfunction

//-----------------------------------------------------------------------------

function AssignEventData(Id, byref tData)

  if(!tData)
    InitEventData(tData);
  endif

  var Data  := GetEventsData();
  var Event   := Data.FindElement(EVENT_TITLE+Id);
  if(!Event)
    var Err := error;
    Err.errortext := "Nie znaleziono Event ID: "+Id+"!";
    return Err;
  endif

  tData.Name        := Event.GetProp("Name");
  tData.Text        := Event.GetProp("Text");
  tData.EventScript := Event.GetProp("EventScript");
  tData.BeforeInterval := Cint(Event.GetProp("BeforeInterval"));
  tData.Hour        := CInt(Event.GetProp("Hour"));
  tData.Delay       := CInt(Event.GetProp("Delay"));
  tData.Flags       := CInt(Event.GetProp("Flags"));
  tData.GlobalMsg   := Event.GetProp("GlobalMsg");
  tData.Areas       := Event.GetProp("Areas");
  tData.Variables   := Event.GetProp("Variables");

  if(!tData.Areas)
    tData.Areas := array;
  endif
  
  if(!tData.Variables)
    tData.Variables := dictionary;
  endif

  return 1;

endfunction


//-----------------------------------------------------------------------------

function AddEvent(byref tData, who := 0)

  var Res := VerifyEvent(tData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data   := GetEventsData();
  var NextId := GetNextEventID();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac EventsData! ["+Data.errortext+"]";
    return Res;
  endif

  if(!NextId)
    Res.errortext := "Nie udalo sie pobrac NextID Gildii";
    return Res;
  endif

  var NewEvent := Data.CreateElement(EVENT_TITLE+NextId);

  if(!NewEvent)
    Res.errortext := "Nie udalo sie stworzyc nowego elementu EventsData ["+NewEvent+"]";
    return Res;
  endif

  NewEvent.SetProp("Name",     tData.Name);
  NewEvent.SetProp("Text",     tData.Text);
  NewEvent.SetProp("EventScript", tData.EventScript);
  NewEvent.SetProp("BeforeInterval", CInt(tData.BeforeInterval));
  NewEvent.SetProp("Hour",    CInt(tData.Hour));
  NewEvent.SetProp("Delay",   CInt(tData.Delay));
  NewEvent.SetProp("Flags",   CInt(tData.Flags));
  NewEvent.SetProp("GlobalMsg",  tData.GlobalMsg);
  NewEvent.SetProp("Areas",  tData.Areas);
  NewEvent.SetProp("Variables",  tData.Variables);

  SetEventElemCrInfo(NewEvent, EVGPROP_CRINFO, who, NextID);
  
  SendGlobalMessage(MSGID_RELOAD_EVENTS, 0, who);

  return NextId;

endfunction

//-----------------------------------------------------------------------------

function SetEvent(GdId, byref tData, who := 0)

  var Res := VerifyEvent(tData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data := GetEventsData();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac Daty Gildii! ["+Data+"]";
    return Res;
  endif

  var Event := Data.FindElement(EVENT_TITLE+GdId);

  if(!Event)
    Res.errortext := "Nie znaleziono Gildii ID: ["+Event+"]";
    return Res;
  endif

  Event.SetProp("Name",     tData.Name);
  Event.SetProp("Text",     tData.Text);
  Event.SetProp("EventScript", tData.EventScript);
  Event.SetProp("BeforeInterval", CInt(tData.BeforeInterval));
  Event.SetProp("Hour",    CInt(tData.Hour));
  Event.SetProp("Delay",   CInt(tData.Delay));
  Event.SetProp("Flags",   CInt(tData.Flags));
  Event.SetProp("GlobalMsg",  tData.GlobalMsg);
  Event.SetProp("Areas",  tData.Areas);
  Event.SetProp("Variables",  tData.Variables);

  SetEventElemUpInfo(Event, EVGPROP_UPINFO, who, GdId);

  SendGlobalMessage(MSGID_RELOAD_EVENTS, 0, who);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteEvent(tId, who := 0)

  var Data := GetEventsData();
  var Res  := error;

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac EventData! ["+Data+"]";
    return Res;
  endif

  var Event := Data.FindElement(EVENT_TITLE+tId);

  if(!Event)
    Res.errortext := "Nie znaleziono Event ID: ["+tId+"]";
    return Res;
  endif

  SendGlobalMessage(MSGID_RELOAD_EVENTS, 0, who);

  return Data.DeleteElement(EVENT_TITLE+tId);

endfunction

//-----------------------------------------------------------------------------

function VerifyEvent(byref tData)

  var Err := error;

  var tIdx := tData.Text.size();

  while(tIdx > 0)
    if(!tData.Text[tIdx].size())
      tData.Text.erase(tIdx);
    else
      break;
    endif
    tIdx := tIdx - 1;
  endwhile

  while(tIdx > 0)
    if(!tData.Text[tIdx])
      tData.Text[tIdx] := "";
    endif
    tIdx := tIdx - 1;
  endwhile

  if(!tData.Name.size())
    Err.errortext := "Brak nazwy wskazowki.";
  elseif(!tData.Text or !tData.Text.size())
    Err.errortext := "Brak tekstu wskazowki.";
  elseif(!tData.EventScript)
    Err.errortext := "Brak skryptu wydarzenia.";
  elseif(tData.Delay < 0)
    Err.errortext := "Opoznienie jest mniejsze od 0!";
  elseif(!tData.Flags)
    Err.errortext := "Nie ustawiono zadnej flagi.";
  elseif(tData.Hour == error)
    Err.errortext := tData.Hour.errortext;
  endif

  var i    := 1;
  var Rlms := Realms();
  var Width, Height, Realm;

  foreach Build in (tData.Areas)
    if(!Build)
      Err.errortext := "Bledny pole Nr: "+i;
      break;
    elseif(Build.size() != 5)
      Err.errortext := "Za malo parametrow dla pola ["+Build.size()+"] (5)";
      break;
    endif

    Build[1] := CInt(Build[1]);
    Build[2] := CInt(Build[2]);
    Build[3] := CInt(Build[3]);
    Build[4] := CInt(Build[4]);
    Build[5] := CInt(Build[5]);

    if(!Build[1] or !Build[3])
      Err.errortext := "Bledne xSet ["+Build[1]+"] lub ySet ["+Build[2]+"]";
      break;
    endif

    if(Build[1] >= Build[3])
      Err.errortext := "xSet >= xEnd ["+Build[1]+" >= "+Build[3]+"]";
      break;
    endif

    if(Build[2] >= Build[4])
      Err.errortext := "ySet >= yEnd ["+Build[2]+" >= "+Build[4]+"]";
      break;
    endif

    Realm := GetRealmById(Build[5]);

    if(!Realm)
      Err.errortext := "Bledny RealmID ["+Build[5]+"]";
      break;
    endif

    Width  := Rlms[Realm].width;
    Height := Rlms[Realm].height;

    if(Build[2] > Width)
      Err.errortext := "xEnd > "+Width+" (max X "+Realm+") ["+Build[2]+"]";
      break;
    endif

    if(Build[4] > Height)
      Err.errortext := "yEnd > "+Height+" (max Y "+Realm+") ["+Build[4]+"]";
      break;
    endif

    i := i + 1;
  endforeach
  
  foreach Variable in (tData.Variables)
    if(!Variable)
      Err.errortext := "Zmienna ["+_Variable_iter+"] Nie posiada wartosci.";
      break;
    endif
  endforeach

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetEventsIdsByEvent(eventId)

  var Ids    := array;
  var Data   := GetEventsData();
  var allIds := Data.Keys();
  var Elem, Id;

  foreach eName in allIds
    Elem := Data.FindElement(eName);

    if(CInt(Elem.GetProp(EVENTROP_EVENT_ID)) == eventId)
      Id := eName;
      Id[EVENT_TITLE] := "";
      Ids.append(CInt(Id));
    endif

  endforeach

  return Ids;

endfunction

//-----------------------------------------------------------------------------
// Player Data
//-----------------------------------------------------------------------------

function GetEventPlayerData(who)

  var Data := GetObjProperty(who, PROP_EVENTS_DATA);

  if(!Data)
    Data := struct;
    Data.+Events := dictionary;
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetEventPlayerDataForEvent(who, EvId)

  var Data := GetEventPlayerData(who);
  var EvData := Data.Events[EvId];
  
  if(!EvData)
    InitEventEntryData(EvData);
  endif
   
  return EvData;

endfunction 

//-----------------------------------------------------------------------------

function GainAutoEventPoints(Player, EventId, Points)

  var EvData := GetEventPlayerDataForEvent(Player, EventId);
  EvData.LastTime := ReadGameClock();
  EvData.Points += Points;

  SetEventPlayerDataForEvent(Player, EventId, EvData);

endfunction

//-----------------------------------------------------------------------------

function SetEventPlayerData(who, byref Data)

  SetObjProperty(who, PROP_EVENTS_DATA, Data);

endfunction

//-----------------------------------------------------------------------------

function SetEventPlayerDataForEvent(who, EventId, byref EvData)

  var Data := GetEventPlayerData(who);

  Data.Events[EventId] := EvData;

  SetObjProperty(who, PROP_EVENTS_DATA, Data);

endfunction

//-----------------------------------------------------------------------------

function InitEventEntryData(byref eventData)

  eventData := struct;
  eventData.+LastTime := 0;
  eventData.+Points := 0;

endfunction

//-----------------------------------------------------------------------------

function DeleteEventPlayer(who, EvId, Event)

  var List := Event.GetProp("Players");
  var Idx := RetIndex(List, who.serial);
  if(Idx)
    List.erase(Idx);
    Event.SetProp("Players", List);
  endif

  var Data := GetObjProperty(who, PROP_EVENTS_DATA);

  if(Data.Events)
    Data.Events.erase(EvId);
    SetObjProperty(who, PROP_EVENTS_DATA, Data);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetEventTitle(EvId, Points)

  __ParamUnused(EvId);

  if(Points > 1000)
    return "Weteran";
  elseif(Points > 500)
    return "Sier¿ant";
  elseif(Points > 100)
    return "¯olnierz";
  else
    return "Nowicjusz";
  endif

endfunction

//-----------------------------------------------------------------------------

function GetEventVariable(Id, Name)

  var Event := GetEventElement(Id);
  var Vars := Event.GetProp("Variables");

  if(Vars and Vars.exists(Name))
    return Vars[Name];
  else
    LogError("[EventCore::GetEventVariable] Probowano pobrac zmienna '"+Name+"', ale taka nie istnieje dla Eventu ID: "+Id, LOGF_SCRIPTS);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function HaveEventVariable(Id, Name)

  var Event := GetEventElement(Id);
  var Vars := Event.GetProp("Variables");

  return Vars.exists(Name);

endfunction

//-----------------------------------------------------------------------------

function GetCaptureTeamTitle(TeamID)

  case(TeamID)
    CTF_RED_TEAM:
      return "Czerwoni";
     
    CTF_BLUE_TEAM:
      return "Niebiescy";
  endcase

  return "";

endfunction

//-----------------------------------------------------------------------------
