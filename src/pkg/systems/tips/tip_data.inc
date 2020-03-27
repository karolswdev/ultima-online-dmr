//-----------------------------------------------------------------------------
// Modified: 2006-11-22
//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------

const TIPS_DATA              := ":tips:tipdata";
const TIPS_GLOBAL_PROPS      := "Global";
const TIP_TITLE              := "Tip_";

//-----------------------------------------------------------------------------
// Tips Data Properties
//-----------------------------------------------------------------------------

const TIPROP_NAME            := "Name";
const TIPROP_EVENT_ID        := "EventID";
const TIPROP_QUEST_ID        := "QuestID";
const TIPROP_DELAY           := "Delay";
const TIPROP_TEXT            := "Text";

const TIAPROP_CRINFO         := "CrInfo";
const TIAPROP_UPINFO         := "UpInfo";
const TIAPROP_USEINFO        := "UseInfo";
const TIAPROP_FLAGS          := "Flags";

const TIGPROP_CRINFO         := "TipCrInfo";
const TIGPROP_UPINFO         := "TipUpInfo";
const TIGPROP_NEXTID         := "TipNextID";

const PROP_TIPS_DATA         := "TipsData";
const PROP_TIPS_EVENTS       := "TipsEvents";

const TIBPROP_TIP_ID         := "TipId";

const TIP_EVENTS_CFG         := ":tips:events";
const TIP_MAIN_CFG           := ":tips:tips";

//-----------------------------------------------------------------------------
// Flags
//-----------------------------------------------------------------------------

const TPF_ARCHER             := 0x00000001;
const TPF_WARRIOR            := 0x00000002;
const TPF_MAGE               := 0x00000004;
const TPF_ONDEMAND           := 0x00000010;
const TPF_ALL_CLASSES        := 0x0000000F;

//-----------------------------------------------------------------------------
// Events
//-----------------------------------------------------------------------------

const TEID_CHAR_CREATE       := 1;
const TEID_CHOOSING_CLASS    := 2;
const TEID_BEGINING          := 3;
const TEID_DEATH             := 4;
const TEID_HUNGER            := 5;

const TEID_WARMODE           := 6;
const TEID_TRAIN_BUTTE       := 7;
const TEID_TRAIN_DUMMY       := 8;
const TEID_PICK_DUMMY        := 9;

const TEID_HARV_FARMING      := 10;
const TEID_HARV_FISHING      := 11;
const TEID_HARV_HERDING      := 12;
const TEID_HARV_LUMBERJACKING := 13;
const TEID_HARV_MINING       := 14;

const TEID_CRAFT_ALCHEMY     := 15;
const TEID_CRAFT_BLACKSMITHY := 16;
const TEID_CRAFT_BOWCRAFT    := 17;
const TEID_CRAFT_CARPENTRY   := 18;
const TEID_CRAFT_CARTOGRAPHY := 19;
const TEID_CRAFT_COOKING     := 20;
const TEID_CRAFT_INSCRIPTION := 21;
const TEID_CRAFT_MAGICITEM   := 22;
const TEID_CRAFT_TAILORING   := 23;
const TEID_CRAFT_TINKERING   := 24;
const TEID_CRAFT_ALCHEMY_GLASS := 25;

const TEID_MAGIC_CAST        := 26;

const MIN_TIPS_TO_TURNOFF    := 10;

//-----------------------------------------------------------------------------

function GetTipsData()

  var Data := OpenDataFile(TIPS_DATA);

  if(!Data)
    Data := CreateDataFile(TIPS_DATA);
  endif

  if(!Data)
    LogError("GetTipsData: Nie udalo sie stworzyc TipsData ["+TIPS_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetTipsGlobalProps()

  var Data      := GetTipsData();
  var GlobProps := 0;

  if(Data)
    GlobProps := Data.FindElement(TIPS_GLOBAL_PROPS);

    if(!GlobProps)
      GlobProps := Data.CreateElement(TIPS_GLOBAL_PROPS);
    endif

    if(GlobProps)
      return GlobProps;
    endif
  endif

  LogError("GetTipsGlobalProps: Podczas tworzenia GlobProps: "+GlobProps+" w TipsData: "+Data);
  return error;

endfunction

//-----------------------------------------------------------------------------

function GetNextTipID()

  var GlobProps := GetTipsGlobalProps();
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(TIGPROP_NEXTID)) + 1;
    if(!GlobProps.SetProp(TIGPROP_NEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function SetTipElemCrInfo(byref Elem, GProp, who := 0, Id := 0)

  var CrInfo    := array(PolCore().systime, 0, 0);
  if(IsPlayer(who))
    CrInfo[2] := who.acctname;
    CrInfo[3] := who.serial;
  endif
  Elem.SetProp(TIAPROP_CRINFO, CrInfo);

  if(GProp)
    var GProps := GetTipsGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    CrInfo[4]  := CInt(oInfo[4])+1;
    CrInfo[5]  := Id;
    GProps.SetProp(GProp, CrInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetTipElemUpInfo(byref Elem, GProp, who := 0, Id := 0)

  var UpInfo    := Elem.GetProp(TIAPROP_UPINFO);
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

  Elem.SetProp(TIAPROP_UPINFO, UpInfo);

  if(GProp)
    var GProps := GetTipsGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    UpInfo[4]  := CInt(oInfo[4])+1;
    UpInfo[5]  := Id;
    GProps.SetProp(GProp, UpInfo);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Tips Data Initialization
//-----------------------------------------------------------------------------
//=============================================================================

function PrepareTipData(byref tData)

  tData := struct;

  tData.+Name;
  tData.+Text;
  tData.+EventId;
  tData.+QuestId;
  tData.+Delay;
  tData.+Flags;

endfunction

//-----------------------------------------------------------------------------

function InitTipData(byref tData)

  if(!tData)
    PrepareTipData(tData);
  endif

  tData.Name        := "";
  tData.Text        := array;
  tData.EventId     := 0;
  tData.QuestId     := 0;
  tData.Delay       := 0;
  tData.Flags       := 0;

endfunction

//-----------------------------------------------------------------------------

function AssignTipData(Id, byref tData)

  if(!tData)
    InitTipData(tData);
  endif

  var Data  := GetTipsData();
  var Tip   := Data.FindElement(TIP_TITLE+Id);
  if(!Tip)
    var Err := error;
    Err.errortext := "Nie znaleziono Tip ID: "+Id+"!";
    return Err;
  endif

  tData.Name        := Tip.GetProp(TIPROP_NAME);
  tData.Text        := Tip.GetProp(TIPROP_TEXT);
  tData.EventId     := CInt(Tip.GetProp(TIPROP_EVENT_ID));
  tData.QuestId     := CInt(Tip.GetProp(TIPROP_QUEST_ID));
  tData.Delay       := CInt(Tip.GetProp(TIPROP_DELAY));
  tData.Flags       := CInt(Tip.GetProp(TIAPROP_FLAGS));

  return 1;

endfunction


//-----------------------------------------------------------------------------

function AddTip(byref tData, who := 0)

  var Res := VerifyTip(tData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data   := GetTipsData();
  var NextId := GetNextTipID();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac TipsData! ["+Data+"]";
    return Res;
  endif

  if(!NextId)
    Res.errortext := "Nie udalo sie pobrac NextID Gildii";
    return Res;
  endif

  var NewTip := Data.CreateElement(TIP_TITLE+NextId);

  if(!NewTip)
    Res.errortext := "Nie udalo sie stworzyc nowego elementu TipsData ["+NewTip+"]";
    return Res;
  endif

  NewTip.SetProp(TIPROP_NAME,     tData.Name);
  NewTip.SetProp(TIPROP_TEXT,     tData.Text);
  NewTip.SetProp(TIPROP_EVENT_ID, CInt(tData.EventId));
  NewTip.SetProp(TIPROP_QUEST_ID, CInt(tData.QuestId));
  NewTip.SetProp(TIPROP_DELAY,    CInt(tData.Delay));
  NewTip.SetProp(TIAPROP_FLAGS,   CInt(tData.Flags));

  SetTipElemCrInfo(NewTip, TIGPROP_CRINFO, who, NextID);

  return NextId;

endfunction

//-----------------------------------------------------------------------------

function SetTip(GdId, byref tData, who := 0)

  var Res := VerifyTip(tData);

  if(!Res)
    return Res;
  endif

  Res := error;

  var Data := GetTipsData();

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac Daty Gildii! ["+Data+"]";
    return Res;
  endif

  var Tip := Data.FindElement(TIP_TITLE+GdId);

  if(!Tip)
    Res.errortext := "Nie znaleziono Gildii ID: ["+Tip+"]";
    return Res;
  endif

  Tip.SetProp(TIPROP_NAME,     tData.Name);
  Tip.SetProp(TIPROP_TEXT,     tData.Text);
  Tip.SetProp(TIPROP_EVENT_ID, CInt(tData.EventId));
  Tip.SetProp(TIPROP_QUEST_ID, CInt(tData.QuestId));
  Tip.SetProp(TIPROP_DELAY,    CInt(tData.Delay));
  Tip.SetProp(TIAPROP_FLAGS,   CInt(tData.Flags));

  SetTipElemUpInfo(Tip, TIGPROP_UPINFO, who, GdId);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteTip(tId)

  var Data := GetTipsData();
  var Res  := error;

  if(!Data)
    Res.errortext := "Nie udalo sie odczytac TipData! ["+Data+"]";
    return Res;
  endif

  var Tip := Data.FindElement(TIP_TITLE+tId);

  if(!Tip)
    Res.errortext := "Nie znaleziono Tip ID: ["+tId+"]";
    return Res;
  endif

  return Data.DeleteElement(TIP_TITLE+tId);

endfunction

//-----------------------------------------------------------------------------

function VerifyTip(byref tData)

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
  elseif(!tData.EventId)
    Err.errortext := "Brak IDa zdarzenia.";
  elseif(tData.Delay < 0)
    Err.errortext := "Opoznienie jest mniejsze od 0!";
  elseif(!tData.Flags)
    Err.errortext := "Nie ustawiono zadnej flagi.";
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetTipsIdsByEvent(eventId)

  var Ids    := array;
  var Data   := GetTipsData();
  var allIds := Data.Keys();
  var Elem, Id;

  foreach eName in allIds
    Elem := Data.FindElement(eName);

    if(CInt(Elem.GetProp(TIPROP_EVENT_ID)) == eventId)
      Id := eName;
      Id[TIP_TITLE] := "";
      Ids.append(CInt(Id));
    endif

  endforeach

  return Ids;

endfunction

//-----------------------------------------------------------------------------
// Events
//-----------------------------------------------------------------------------

function GetTipEventName(eId)

  var eCfg := ReadConfigFile(TIP_EVENTS_CFG);

  if(eCfg)
    return ClStr(GetConfigString(eCfg["Names"], CStr(eId)));
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetTipEventNames()

  var eCfg  := ReadConfigFile(TIP_EVENTS_CFG);
  var Names := dictionary;

  if(eCfg)
    var Props := ListConfigElemProps(eCfg["Names"]);
    foreach Prop in Props
      Names[ZeroFill(Prop, 2)] := GetConfigString(eCfg["Names"], Prop);
    endforeach
  endif

  return Names;

endfunction

//-----------------------------------------------------------------------------
// Player Data
//-----------------------------------------------------------------------------

function GetTipPlayerData(who)

  var Data := GetObjProperty(who, PROP_TIPS_DATA);

  if(!Data)
    Data := struct;
    Data.+Showed  := dictionary;
    Data.+Pending := dictionary;
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function SetTipPlayerData(who, byref Data)

  SetObjProperty(who, PROP_TIPS_DATA, Data);

endfunction

//-----------------------------------------------------------------------------

function InitTipEntryData(byref tipData)

  tipData := struct;
  tipData.+startTime;
  tipData.+Delay;
  tipData.+Id;
  tipData.+tFlags;

endfunction

//-----------------------------------------------------------------------------

function GetTipName(TipId)
  
  var Data  := GetTipsData();
  var Tip   := Data.FindElement(TIP_TITLE + TipId);

  if(!Tip)
    return "<Brak>";
  endif

  return Tip.GetProp(TIPROP_NAME);

endfunction

//-----------------------------------------------------------------------------
