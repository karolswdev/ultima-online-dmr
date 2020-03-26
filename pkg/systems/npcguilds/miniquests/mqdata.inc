//-----------------------------------------------------------------------------
// Modified: 2005-07-08
//-----------------------------------------------------------------------------

use cfgfile;
use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";
include "const";
include "include/coords";

//-----------------------------------------------------------------------------

function InitMQuestData(byref MQData)

  MQData := struct;

  MQData.+NpcTemp    := "";
  MQData.+Type       := 0;
  MQData.+Targ       := 0;
  MQData.+Loc        := "";
  MQData.+Flags      := 0;
  MQData.+Title      := array;
  MQData.+EncrText   := array;
  MQData.+MainText   := array;
  MQData.+SuccText   := array;
  MQData.+DNpcText   := array;

endfunction

//-----------------------------------------------------------------------------

function InitMQNpcItemTarg(byref Targ)

  Targ := struct;
  Targ.+Template   := "";
  Targ.+Script     := "";
  Targ.+Gender     := 0;
  Targ.+ObjType    := 0;
  Targ.+Color      := 0;
  Targ.+Name       := "";

endfunction

//-----------------------------------------------------------------------------

function InitMQNpcTarg(byref Targ)

  Targ := struct;
  Targ.+Template   := "";
  Targ.+Script     := "";
  Targ.+Gender     := 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// MiniQuest Data Functions
//-----------------------------------------------------------------------------
//=============================================================================

function GetMQuestData()

  var Data := OpenDataFile(MQUEST_DATA);

  if(!Data)
    Data := CreateDataFile(MQUEST_DATA);
  endif

  if(!Data)
    LogError("GetMQuestData: Nie udalo sie stworzyc MiniQuestData ["+MQUEST_DATA+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetMQuestGlobalProps()

  var Data      := GetMQuestData();
  var GlobProps := 0;

  if(Data)
    GlobProps := Data.FindElement(MQUEST_GLOBAL_PROPS);

    if(!GlobProps)
      GlobProps := Data.CreateElement(MQUEST_GLOBAL_PROPS);
    endif

    if(GlobProps)
      return GlobProps;
    endif
  endif

  LogError("GetMQuestGlobalProps: Podczas tworzenia GlobProps: "+GlobProps+" w MiniQuestData: "+Data);
  return error;

endfunction

//-----------------------------------------------------------------------------

function GetNextMQuestID()

  var GlobProps := GetMQuestGlobalProps();
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(MQGPROP_NEXTID)) + 1;
    if(!GlobProps.SetProp(MQGPROP_NEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function AssignMQuestData(Id, byref QData)

  if(!QData)
    InitQuestData(QData);
  endif

  var Data  := GetMQuestData();
  var Quest := Data.FindElement(MQUEST_TITLE+Id);
  if(!Quest)
    var Err := error;
    Err.errortext := "Nie znaleziono MiniQuesta ID: "+Id+"!";
    return Err;
  endif  
  
  QData.NpcTemp   := Quest.GetProp(MQPROP_NPCTEMP);
  QData.Type      := Quest.GetProp(MQPROP_TYPE);
  QData.Targ      := Quest.GetProp(MQPROP_TARG);
  QData.Loc       := Quest.GetProp(MQPROP_LOC);
  QData.Title     := Quest.GetProp(MQPROP_TITLE);
  QData.EncrText  := Quest.GetProp(MQPROP_ENCRTEXT);
  QData.MainText  := Quest.GetProp(MQPROP_MAINTEXT);
  QData.SuccText  := Quest.GetProp(MQPROP_SUCCTEXT);
  QData.DNpcText  := Quest.GetProp(MQPROP_DNPCTEXT);
  QData.Flags     := Quest.GetProp(MQPROP_FLAGS);

  return 1;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Config
//-----------------------------------------------------------------------------
//=============================================================================

function GetMQuestGlobCfgElem()

  var QCfg := ReadConfigFile(MQUESTS_CFG);

  if(!QCfg)
    LogError("GetMQuestGlobCfgElem: Nie udalo sie odczytac MiniQuestCfg ["+MQUESTS_CFG+"}");
  else
    var Elem := QCfg[MQCFG_GLOBAL];
    if(!Elem)
      LogError("GetMQuestGlobCfgElem: Brak globalnego elementu MiniQuestow ["+MQUESTS_CFG+"]");
    else
      return Elem;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMQuestTypeName(Type)

  if(!Type)
    return "";
  endif

  var QCfg := ReadConfigFile(MQUESTS_CFG);
  
  if(!QCfg)
    LogError("GetMQuestTypeName: Nie udalo sie odczytac MiniQuestCfg ["+MQUESTS_CFG+"]");
  else
    if(!QCfg[MQCFG_TNAMES])
      LogError("GetMQuestTypeName: Brak grupy z nazwami Typow MiniQuestow ["+MQUESTS_CFG+"]");
    else
      var Name := GetConfigString(QCfg[MQCFG_TNAMES], MQCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetMQuestTextVars()

  var QCfg := ReadConfigFile(MQUESTS_CFG);
  
  if(!QCfg)
    LogError("GetMQuestTextVars: Nie udalo sie odczytac MiniQuestCfg ["+MQUESTS_CFG+"]");
  else
    if(!QCfg[MQCFG_TEXTVARS])
      LogError("GetMQuestTextVars: Brak grupy z zmiennymi tekstowymi MiniQuestow ["+MQUESTS_CFG+"]");
    else 
      var Elem  := QCfg[MQCFG_TEXTVARS];
      var Props := ListConfigElemProps(Elem);
      var Vars  := dictionary;

      foreach Prop in Props
        Vars[Prop] := GetConfigString(Elem, Prop);
      endforeach

      return Vars;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetMQuestElemCrInfo(byref Elem, who := 0, Id := 0)

  var CrInfo    := array(PolCore().systime, 0, 0);
  if(IsPlayer(who))
    CrInfo[2] := who.acctname;
    CrInfo[3] := who.serial;
  endif
  Elem.SetProp(MQPROP_CRINFO, CrInfo);

  var GProps := GetMQuestGlobalProps();
  var oInfo  := GProps.GetProp(MQPROP_CRINFO);
  CrInfo[4]  := CInt(oInfo[4])+1;
  CrInfo[5]  := Id;
  GProps.SetProp(MQPROP_CRINFO, CrInfo);

endfunction

//-----------------------------------------------------------------------------

function SetMQuestElemUpInfo(byref Elem, who := 0, Id := 0)

  var UpInfo    := Elem.GetProp(MQPROP_UPINFO);
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

  Elem.SetProp(MQPROP_UPINFO, UpInfo);

  var GProps := GetMQuestGlobalProps();
  var oInfo  := GProps.GetProp(MQPROP_UPINFO);
  UpInfo[4]  := CInt(oInfo[4])+1;
  UpInfo[5]  := Id;
  GProps.SetProp(MQPROP_UPINFO, UpInfo);

endfunction

//-----------------------------------------------------------------------------

function SetMQuestElemUseInfo(byref Elem, who := 0, Id := 0)

  var UseInfo    := Elem.GetProp(MQPROP_USEINFO);
  if(!UseInfo)
    UseInfo := array(0, 0, 0, 0);
  endif

  if(IsPlayer(who))
    UseInfo[2] := who.acctname;
    UseInfo[3] := who.serial;
  endif

  UseInfo[1] := PolCore().systime;
  if(UseInfo[4] > 10000000)
    UseInfo[4] := 0;
  endif
  UseInfo[4] := CInt(UseInfo[4])+1;

  Elem.SetProp(MQPROP_USEINFO, UseInfo);

  var GProps := GetMQuestGlobalProps();
  var oInfo  := GProps.GetProp(MQPROP_USEINFO);
  UseInfo[4] := CInt(oInfo[4])+1;
  UseInfo[5] := Id;
  GProps.SetProp(MQPROP_USEINFO, UseInfo);

endfunction

//-----------------------------------------------------------------------------

function ConvertMQText(byref Text, byref QData, Player)

  var SrcNpc   := FindNpcBySerial(QData.Source);
  var DstNpc   := FindNpcBySerial(QData.Targ.NpcSer);

  var Pos, SrcNpcName, SrcNpcType, DstNpcName, DstNpcType, LocName, ItemName;

  Pos := find(SrcNpc.name," - ",1);
  if(Pos)
    SrcNpcName := SrcNpc.name[1,Pos-1];
    SrcNpcType := SrcNpc.name[Pos+3, len(SrcNpc.name)-(Pos+2)];
  else
    SrcNpcName := SrcNpc.name;
    SrcNpcType := "";
  endif

  Pos := find(DstNpc.name," - ",1);
  if(Pos)
    DstNpcName := DstNpc.name[1,Pos-1];
    DstNpcType := DstNpc.name[Pos+3, len(DstNpc.name)-(Pos+2)];
  else
    DstNpcName := DstNpc.name;
    DstNpcType := "";
  endif

  ItemName := QData.Targ.ItemName;
  if(!ItemName)
    ItemName := GetDescForItem(QData.Targ.ObjType);
  endif

  LocName := FindCityCoords(QData.Loc.x, QData.Loc.y, QData.Loc.realm);
  if(!LocName)
    LocName := ToSextant(QData.Loc.x, QData.Loc.y, QData.Loc.realm);
  endif

  foreach Line in Text
    Line["%sn"]  := SrcNpcName;
    Line["%st"]  := SrcNpcType;
    Line["%nn"]  := DstNpcName;
    Line["%nt"]  := DstNpcType;
    Line["%pn"]  := Player.name;
    Line["%loc"] := LocName;
    Line["%in"]  := ItemName;
    ConvGgmTextLine(Line, "%gd", DstNpc);
    ConvGgmTextLine(Line, "%gs", SrcNpc);
    ConvGgmTextLine(Line, "%gp", Player);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function ConvGgmTextLine(byref Line, GmId, Mob)

  var Pos, Id;

  while(Line[GmId])
    Pos := find(Line,GmId,0)+len(GmId);
    Id  := CInt(Line[Pos, len(Line)]);
    if(!Id)
      Line[GmId] := "";
    else
      Line[GmId+Id] := ggm(Mob, Id);
    endif
  endwhile

endfunction

//-----------------------------------------------------------------------------
