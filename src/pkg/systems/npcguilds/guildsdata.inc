//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";
include "common";

//=============================================================================
//-----------------------------------------------------------------------------
// NpcGuilds Data Functions
//-----------------------------------------------------------------------------
//=============================================================================

function GetNGdGlobalProps()

  var Data      := GetNGdData();
  var GlobProps := 0;

  if(Data)
    GlobProps := Data.FindElement(NGD_GLOBAL_PROPS);

    if(!GlobProps)
      GlobProps := Data.CreateElement(NGD_GLOBAL_PROPS);
    endif

    if(GlobProps)
      return GlobProps;
    endif
  endif

  LogError("GetNGdGlobalProps: Podczas tworzenia GlobProps: "+GlobProps+" w NpcGuildsData: "+Data);
  return error;

endfunction

//-----------------------------------------------------------------------------

function GetNextNpcGuildID()

  var GlobProps := GetNGdGlobalProps();
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(GDGPROP_NEXTID)) + 1;
    if(!GlobProps.SetProp(GDGPROP_NEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// NpcGuild Config
//-----------------------------------------------------------------------------
//=============================================================================

function GetNGdTypeName(Type)

  var GdCfg := ReadConfigFile(NGD_CFG);
  
  if(!GdCfg)
    LogError("GetNGdTypeName: Nie udalo sie odczytac NpcGuildsCfg: "+NGD_CFG);
  else
    if(!GdCfg[NGDCFG_TNAMES])
      LogError("GetNGdTypeName: Brak grupy z nazwami Typow NpcGildii ["+NGD_CFG+"]");
    else
      var Name := GetConfigString(GdCfg[NGDCFG_TNAMES], NGDCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetNGdStatusName(Type)

  var GdCfg := ReadConfigFile(NGD_CFG);
  
  if(!GdCfg)
    LogError("GetNGdStatusName: Nie udalo sie odczytac NpcGuildsCfg: "+NGD_CFG);
  else
    if(!GdCfg[NGDCFG_SNAMES])
      LogError("GetNGdStatusName: Brak grupy z nazwami Typow NpcGildii ["+NGD_CFG+"]");
    else
      var Name := GetConfigString(GdCfg[NGDCFG_SNAMES], NGDCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetNpcGuildAgeReq(Lvl)

  var GdCfg := ReadConfigFile(NGD_CFG);
  
  if(!GdCfg)
    LogError("GetNpcGuildAgeReq: Nie udalo sie odczytac NpcGuildsCfg: "+NGD_CFG);
  else
    if(!GdCfg[NGDCFG_AGEREQ])
      LogError("GetNpcGuildAgeReq: Brak CfgElem z wymaganym wiekiem w NpcGildii ["+NGD_CFG+"]");
    else
      return GetConfigInt(GdCfg[NGDCFG_AGEREQ], NGDCFG_LVL_TITLE+CStr(CInt(Lvl)));
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetNGdElemCrInfo(byref Elem, GProp, who := 0, Id := 0)

  var CrInfo    := array(PolCore().systime, 0, 0);
  if(IsPlayer(who))
    CrInfo[2] := who.acctname;
    CrInfo[3] := who.serial;
  endif
  Elem.SetProp(GDAPROP_CRINFO, CrInfo);

  if(GProp)
    var GProps := GetNGdGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    CrInfo[4]  := CInt(oInfo[4])+1;
    CrInfo[5]  := Id;
    GProps.SetProp(GProp, CrInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetNGdElemUpInfo(byref Elem, GProp, who := 0, Id := 0)

  var UpInfo    := Elem.GetProp(GDAPROP_UPINFO);
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

  Elem.SetProp(GDAPROP_UPINFO, UpInfo);

  if(GProp)
    var GProps := GetNGdGlobalProps();
    var oInfo  := GProps.GetProp(GProp);
    UpInfo[4]  := CInt(oInfo[4])+1;
    UpInfo[5]  := Id;
    GProps.SetProp(GProp, UpInfo);
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// NpcGuild Data Initialization
//-----------------------------------------------------------------------------
//=============================================================================

function PrepareNGdData(byref GdData)

  GdData := struct;

  GdData.+Name;
  GdData.+Type;
  GdData.+QuestTypes;
  GdData.+Members;
  GdData.+Status;
  GdData.+Buildings;
  GdData.+SkIDs;
  GdData.+MainSkIDs;
  GdData.+Flags;
  GdData.+Master;
  GdData.+Cities;
  GdData.+Board;

endfunction

//-----------------------------------------------------------------------------

function InitNGdData(byref GdData)

  if(!GdData)
    PrepareNGdData(GdData);
  endif

  GdData.Name        := "";
  GdData.Type        := 0;
  GdData.QuestTypes  := array;
  GdData.Members     := array;
  GdData.Buildings   := array;
  GdData.Status      := 0;
  GdData.SkIDs       := array;
  GdData.MainSkIDs   := array;
  GdData.Flags       := 0;
  GdData.Master      := 0;
  GdData.Cities      := array;
  GdData.Board       := array;

endfunction

//-----------------------------------------------------------------------------

function AssignNGdData(Id, byref GdData)

  if(!GdData)
    InitNGdData(GdData);
  endif

  var Data  := GetNGdData();
  var Guild := Data.FindElement(NGUILD_TITLE+Id);
  if(!Guild)
    var Err := error;
    Err.errortext := "Nie znaleziono Gildii ID: "+Id+"!";
    return Err;
  endif

  GdData.Name        := Guild.GetProp(GDPROP_NAME);
  GdData.Type        := Guild.GetProp(GDPROP_TYPE);
  GdData.QuestTypes  := Guild.GetProp(GDPROP_QTYPES);
  GdData.Members     := Guild.GetProp(GDPROP_MEMBERS);
  GdData.Buildings   := Guild.GetProp(GDPROP_BUILDINGS);
  GdData.Status      := Guild.GetProp(GDPROP_STATUS);
  GdData.SkIDs       := Guild.GetProp(GDPROP_SKIDS);
  GdData.MainSkIDs   := Guild.GetProp(GDPROP_MAINSKIDS);
  GdData.Flags       := Guild.GetProp(GDAPROP_FLAGS);
  GdData.Master      := Guild.GetProp(GDPROP_MASTER);
  GdData.Cities      := Guild.GetProp(GDPROP_CITIES);
  GdData.Board       := Guild.GetProp(GDPROP_BOARD);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function InitNGdPlayerData(byref Data)

  Data := struct;

  Data.+Guilds   := dictionary;
  Data.+MaxSkID  := error;
  Data.+SkChTime := 0;

endfunction

//-----------------------------------------------------------------------------

function InitNGdPlayerGuildData(byref Data)

  Data := struct;

  Data.+Level       := 0;
  Data.+Exp         := 0;
  Data.+Delay       := 0;
  Data.+Flags       := 0;
  Data.+JoinTime    := 0;
  Data.+LastQuest   := struct;
  Data.+LastVisit   := 0;
  Data.+LastRepLose := 0;

  Data.LastQuest.+Ticket := 0;
  Data.LastQuest.+Time   := 0;
  Data.LastQuest.+QuestGroup := error;
  Data.LastQuest.+QId    := 0;
  Data.LastQuest.+From   := 0;

endfunction

//-----------------------------------------------------------------------------

function InitNGdNpcData(byref Data)

  Data := struct;

  Data.+Level        := 0;
  Data.+GuildId      := 0;

endfunction

//-----------------------------------------------------------------------------

function AddGuildToPlayerData(byref Data, GdId)

  if(!Data or !Data.Guilds)
    InitNGdPlayerData(Data);
  endif

  if(!Data.Guilds.exists(GdId))
    Data.Guilds.insert(GdId, 0);
    InitNGdPlayerGuildData(Data.Guilds[GdId]);
    Data.Guilds[GdId].LastVisit := ReadGameClock();
  endif

endfunction

//-----------------------------------------------------------------------------

function GetSimilarNpcGuild(who, BaseId)

  var Data := GetNGdData();
  var BaseGuild := Data.FindElement(NGUILD_TITLE+BaseId);
  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var CurExp := PlayerData.Guilds[BaseId].Exp;
  var SimId := 0;

  foreach GuildData in (PlayerData.Guilds)
    if(_GuildData_Iter != BaseId and !(GuildData.Flags & NGPF_MEMBER))
      var Guild := Data.FindElement(NGUILD_TITLE+_GuildData_Iter);

      if(IsSimilarNpcGuild(BaseGuild, Guild))
        if(GuildData.Exp > CurExp)
          CurExp := GuildData.Exp;
          SimId := _GuildData_Iter;
        endif
      endif
    endif
  endforeach

  return SimId;

endfunction

//-----------------------------------------------------------------------------

function IsSimilarNpcGuild(BaseGuild, Guild)
/*

const NGDTYPE_WARRIORS       := 1;
const NGDTYPE_ARCHERS        := 2;
const NGDTYPE_MAGES          := 3;
const NGDTYPE_MURDERERS      := 4;
const NGDTYPE_THIEFS         := 5;
const NGDTYPE_WEARWOLFS      := 6;
const NGDTYPE_VAMPIRES       := 7;
const NGDTYPE_NECROS         := 8;
const NGDTYPE_DRUIDS         := 9;
const NGDTYPE_CRAFTSMAN      := 10;

const NGDSTATUS_ORDER        := 1;
const NGDSTATUS_CHAOS        := 2;
const NGDSTATUS_NEUTRAL      := 3;
*/

  var BaseType := BaseGuild.GetProp(GDPROP_TYPE);
  var BaseStatus := BaseGuild.GetProp(GDPROP_STATUS);
  var Type := Guild.GetProp(GDPROP_TYPE);
  var Status := Guild.GetProp(GDPROP_STATUS);

  if(BaseType == Type)
    return 1;
  elseif(Type == NGDTYPE_MURDERERS and BaseStatus != NGDSTATUS_CHAOS)
    return 1;
  endif

  return 0;
 
endfunction

//-----------------------------------------------------------------------------
