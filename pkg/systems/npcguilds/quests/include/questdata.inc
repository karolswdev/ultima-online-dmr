//-----------------------------------------------------------------------------
// Modified: 2005-05-22
//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";

//-----------------------------------------------------------------------------
// Quest Properties
//-----------------------------------------------------------------------------

const QSTPROP_TYPE           := "Type";
const QSTPROP_SUBTYPE        := "SubType";
const QSTPROP_TITLE          := "Title";
const QSTPROP_TIP            := "Tip";
const QSTPROP_TARG           := "Targ";
const QSTPROP_LOC            := "Loc";
const QSTPROP_GUARDS         := "Guards";

const QPROP_TYPE             := "Type";
const QPROP_SUBTYPE          := "SubType";
const QPROP_LEVEL            := "Level";
const QPROP_STAGES           := "Stages";
const QPROP_TITLE            := "Title";
const QPROP_TIP              := "Tip";
const QPROP_PRICE            := "Price";
const QPROP_MAXTIME          := "MaxTime";
const QPROP_CHANCE           := "Chance";
const QPROP_GDTYPE           := "GuildType";
const QPROP_CITIES           := "Cities";

const QAPROP_CRINFO          := "CrInfo";
const QAPROP_UPINFO          := "UpInfo";
const QAPROP_USEINFO         := "UseInfo";
const QAPROP_FLAGS           := "Flags";

//-----------------------------------------------------------------------------

const QUEST_DATA             := ":quests:";
const QUEST_GLOBAL_PROPS     := "Global";

const QUEST_MAXLVL           := 5;
const QTYPE_MAXID            := 10;
const QSTYPE_MAXID           := 26;
const QUEST_MAXTARG          := 20;
const QUEST_MAXPRICE         := 10;
const QUEST_MAXSTAGES        := 10;
const QUEST_MAXST_FWPAGES    := 0;

const QUEST_MAX_PL_LLOGTIME  := 86400;
const QUEST_MAXGEN_TRIES     := 6;

const QUEST_MINSK_VAL        := 30;

//-----------------------------------------------------------------------------
// Global Properties
//-----------------------------------------------------------------------------

const QGPROP_STNEXTID        := "StNextID";
const QGPROP_NEXTID          := "NextID";
const QGPROP_QCRINFO         := "QuestCrInfo";
const QGPROP_QSCRINFO        := "QuestStageCrInfo";
const QGPROP_LCRINFO         := "QLocCrInfo";
const QGPROP_QUPINFO         := "QuestUpInfo";
const QGPROP_QSUPINFO        := "QuestStageUpInfo";
const QGPROP_LUPINFO         := "QLocUpInfo";
const QGPROP_QUSEINFO        := "QuestUseInfo";
const QSTAGE_TITLE           := "St_";
const QUEST_TITLE            := "Qs_";
const QLOC_TITLE             := "Lc_";

//-----------------------------------------------------------------------------
// Quests Config
//-----------------------------------------------------------------------------

const QUESTS_CFG             := ":quests:quests";
const QPRICES_CFG            := ":quests:prices";
const QCFG_TYPE_TITLE        := "Type_";
const QCFG_LVL_TITLE         := "Level_";
const QCFG_TNAMES            := "TypeNames";
const QCFG_STNAMES           := "SubTypeNames";
const QCFG_AVSUBTYPES        := "AvSubTypes";
const QCFG_MAXQTIME          := "MaxQuestTime";
const QCFG_QUESTOBJS         := "QuestObjectives";
const QCFG_ITEMEXPTIME       := "ItemExpTime";
const QCFG_NPCQUESTTEXT      := "NpcQuestText_";
const QCFG_PRICES            := "Prices";
const QCFG_SKILLRANGE        := "SkillRange";
const QCFG_NSCORERANGE       := "NpcScoreRange";

//-----------------------------------------------------------------------------
// Quests Config Npc Quest Text
//-----------------------------------------------------------------------------

const QNT_GOTO_BADUSER       := 1;
const QNT_GOTO_NORMCOMP      := 2;
const QNT_GOTO_KILL          := 3;
const QNT_GOTO_DESTROY       := 4;
const QNT_GOTO_RESCUE        := 5;
const QNT_GOTO_GLOC          := 6;
const QNT_GOTO_GEXNPC        := 7;
const QNT_GOTO_STEAL         := 8;
const QNT_GOTO_LOCKPICK      := 9;
const QNT_GOTO_ITEM          := 10;
const QNT_GOTO_TAME          := 11;
const QNT_GOTO_DEFAULT       := 12;
const QNT_RESCUE_GUARDS      := 13;

//-----------------------------------------------------------------------------
// Quest Groups
//-----------------------------------------------------------------------------

const QGROUP_NPCGUILDS       := "quests";
const QGROUP_TUTORIALS       := "tutorials";
const QGROUP_LOCDATA         := QGROUP_NPCGUILDS;
const QGROUP_DEFAULT         := QGROUP_NPCGUILDS;

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Data Functions
//-----------------------------------------------------------------------------
//=============================================================================

function GetQuestData(QuestGroup)

  // dla obslugi starych questow - jak brak grupy, to domyslna jest quests.
  if(!QuestGroup)
    QuestGroup := QGROUP_DEFAULT;
  endif

  var Data := OpenDataFile(QUEST_DATA + QuestGroup);

  if(!Data)
    Data := CreateDataFile(QUEST_DATA + QuestGroup);
  endif

  if(!Data)
    LogError("GetQuestData: Nie udalo sie stworzyc QuestData ["+QUEST_DATA + QuestGroup+"]");
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function FindQuest(QuestGroup, Id)

  var Data := GetQuestData(QuestGroup);
  return Data.FindElement(CStr(Id));

endfunction

//-----------------------------------------------------------------------------

function GetQuestGlobalProps(QuestGroup)

  var Data      := GetQuestData(QuestGroup);
  var GlobProps := 0;

  if(Data)
    GlobProps := Data.FindElement(QUEST_GLOBAL_PROPS);

    if(!GlobProps)
      GlobProps := Data.CreateElement(QUEST_GLOBAL_PROPS);
    endif

    if(GlobProps)
      return GlobProps;
    endif
  endif

  LogError("GetQuestGlobalProps: Podczas tworzenia GlobProps: "+GlobProps+" w QuestData: "+Data);
  return error;

endfunction

//-----------------------------------------------------------------------------

function GetNextQuestStageID(QuestGroup)

  var GlobProps := GetQuestGlobalProps(QuestGroup);
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(QGPROP_STNEXTID)) + 1;
    if(!GlobProps.SetProp(QGPROP_STNEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function GetNextQuestID(QuestGroup)

  var GlobProps := GetQuestGlobalProps(QuestGroup);
  var NextID    := error;

  if(GlobProps)
    NextID := CInt(GlobProps.GetProp(QGPROP_NEXTID)) + 1;
    if(!GlobProps.SetProp(QGPROP_NEXTID, NextID))
      return error;
    endif
  endif

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function PrepareQuestStageData(byref QStageData)

  QStageData := struct;

  QStageData.+Title;
  QStageData.+Type;
  QStageData.+SubType;
  QStageData.+Loc;
  QStageData.+Targ;
  QStageData.+Tip;
  QStageData.+Flags;
  QStageData.+Guards;

endfunction

//-----------------------------------------------------------------------------

function PrepareQuestData(byref QData)

  QData      := struct;

  QData.+Title;
  QData.+Tip;
  QData.+Level;
  QData.+Stages;
  QData.+Flags;
  QData.+Type;
  QData.+SubType;
  QData.+Price;
  QData.+MaxTIme;
  QData.+Chance;
  QData.+GuildCities;
  QData.+GuildType;

endfunction

//-----------------------------------------------------------------------------

function PrepareQLocData(byref QLocData)

  QLocData := struct;

  QLocData.+GroupName;
  QLocData.+Locs;
  QLocData.+Flags;

endfunction

//-----------------------------------------------------------------------------

function InitQuestStageData(byref QStageData)

  if(!QStageData)
    PrepareQuestStageData(QStageData);
  endif

  QStageData.Title    := "";
  QStageData.Type     := 0;
  QStageData.SubType  := 0;
  QStageData.Loc      := 0;
  QStageData.Targ     := 0;
  QStageData.Tip      := array;
  QStageData.Flags    := 0;
  QStageData.Guards   := array;

endfunction

//-----------------------------------------------------------------------------

function InitQuestData(byref QData)

  if(!QData)
    PrepareQuestData(QData);
  endif

  QData.Title     := 0;
  QData.Tip       := array;
  QData.Level     := 0;
  QData.Stages    := array;
  QData.Flags     := 0;
  QData.Type      := 0;
  QData.SubType   := 0;
  QData.Price     := 0;
  QData.MaxTime   := 0;
  QData.Chance    := 100;
  QData.GuildType := 0;
  QData.GuildCities := array;

endfunction

//-----------------------------------------------------------------------------

function InitQLocData(byref QLocData)

  if(!QLocData)
    PrepareQLocData(QLocData);
  endif

  QLocData.GroupName := 0;
  QLocData.Flags     := 0;
  QLocData.Locs      := array;

endfunction

//-----------------------------------------------------------------------------

function InitQTargItem(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+ToObjType := 0;
  Targ.+Color     := 0;
  Targ.+Name      := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargLockTake(byref Targ)

  Targ := struct;

  Targ.+ContObjType := 0;
  Targ.+ObjType     := 0;
  Targ.+ToObjType   := 0;
  Targ.+Color       := 0;
  Targ.+Name        := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargNpc(byref Targ)

  Targ := struct;

  Targ.+Template  := "";
  Targ.+Color     := 0;
  Targ.+Name      := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargExNpc(byref Targ)

  Targ := struct;

  Targ.+Template  := "";
  Targ.+Gender    := 0;
  Targ.+Script    := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargLoc(byref Targ)

  Targ := struct;

  Targ.+x     := 0;
  Targ.+y     := 0;
  Targ.+z     := 0;
  Targ.+Realm := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQTargFromNpc(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+Color     := 0;
  Targ.+Gender    := 0;
  Targ.+Name      := "";
  Targ.+Template  := "";
  Targ.+Script    := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargLootNpc(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+Color     := 0;
  Targ.+Name      := "";
  Targ.+Template  := "";
  Targ.+NpcName   := "";

endfunction

//-----------------------------------------------------------------------------

function InitQTargSkill(byref Targ)

  Targ := struct;

  Targ.+ObjType    := 0;
  Targ.+ToObjType  := 0;
  Targ.+SkVal      := 0;
  Targ.+ExpChance  := 0;
  Targ.+MatObjType := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQTargMine(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+ToObjType := 0;
  Targ.+SkVal     := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQTargPlayer(byref Targ)

  Targ := struct;

  Targ.+SkID      := 0;
  Targ.+SkVal     := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQTargHouse(byref Targ)

  Targ := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQuestPrice(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+ToObjType := 0;
  Targ.+Amount    := 1;
  Targ.+Chance    := 100;
  Targ.+Color     := 0;
  Targ.+Name      := "";

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Config
//-----------------------------------------------------------------------------
//=============================================================================

function GetQuestTypeName(Type)
 
  if(!Type)
    return "";
  endif

  var QCfg := ReadConfigFile(QUESTS_CFG);
  
  if(!QCfg)
    LogError("GetQuestTypeName: Nie udalo sie odczytac QuestCfg: "+QUESTS_CFG);
  else
    if(!QCfg[QCFG_TNAMES])
      LogError("GetQuestTypeName: Brak grupy z nazwami Typow Questow ["+QUESTS_CFG+"]");
    else
      var Name := GetConfigString(QCfg[QCFG_TNAMES], QCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetQuestSubTypeName(Type)

  if(!Type)
    return "";
  endif

  var QCfg := ReadConfigFile(QUESTS_CFG);
  
  if(!QCfg)
    LogError("GetQuestSubTypeName: Nie udalo sie odczytac QuestCfg: "+QUESTS_CFG);
  else
    if(!QCfg[QCFG_STNAMES])
      LogError("GetQuestSubTypeName: Brak grupy z nazwami PodTypow Questow ["+QUESTS_CFG+"]");
    else
      var Name := GetConfigString(QCfg[QCFG_STNAMES], QCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(Name)
        return Name;
      endif
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetQuestSkillRange(Level)

  var QCfg := ReadConfigFile(QUESTS_CFG);
  
  if(!QCfg)
    LogError("GetQuestSkillRange: Nie udalo sie odczytac QuestCfg: "+QUESTS_CFG);
  else
    if(!QCfg[QCFG_SKILLRANGE])
      LogError("GetQuestSkillRange: Brak grupy z zasiegiem skilli Questow ["+QUESTS_CFG+"]");
    else
      var Range := GetConfigString(QCfg[QCFG_SKILLRANGE], QCFG_LVL_TITLE+CStr(CInt(Level)));
      if(Range)
        Range    := SplitWords(Range);
        Range[1] := CInt(Range[1]);
        Range[2] := CInt(Range[2]);
        return Range;
      endif
    endif
  endif

  return array(0,0);

endfunction

//-----------------------------------------------------------------------------

function GetQuestNpsScoreRange(Level)

  var QCfg := ReadConfigFile(QUESTS_CFG);
  
  if(!QCfg)
    LogError("GetQuestNpsScoreRange: Nie udalo sie odczytac QuestCfg: "+QUESTS_CFG);
  else
    if(!QCfg[QCFG_NSCORERANGE])
      LogError("GetQuestNpsScoreRange: Brak grupy z zasiegiem NpcScore Questow ["+QUESTS_CFG+"]");
    else
      var Range := GetConfigString(QCfg[QCFG_NSCORERANGE], QCFG_LVL_TITLE+CStr(CInt(Level)));
      if(Range)
        Range    := SplitWords(Range);
        Range[1] := CInt(Range[1]);
        Range[2] := CInt(Range[2]);
        return Range;
      endif
    endif
  endif

  return array(0,0);

endfunction

//-----------------------------------------------------------------------------

function GetQuestAvSubTypes(Type)

  if(!Type)
    return array;
  endif

  var QCfg := ReadConfigFile(QUESTS_CFG);
  
  if(!QCfg)
    LogError("GetQuestAvSubTypes: Nie udalo sie odczytac QuestCfg: "+QUESTS_CFG);
  else
    if(!QCfg[QCFG_AVSUBTYPES])
      LogError("GetQuestAvSubTypes: Brak grupy z dostepnymi PodTypami Typow ["+QUESTS_CFG+"]");
    else
      var SubTypes := GetConfigString(QCfg[QCFG_AVSUBTYPES], QCFG_TYPE_TITLE+CStr(CInt(Type)));
      if(!SubTypes)
        LogError("GetQuestAvSubTypes: Brak PodTypow dla Typu: "+CInt(Type)+", ["+QUESTS_CFG+"]");
      else
        SubTypes := SplitWords(SubTypes);
        foreach SubType in SubTypes
          SubType := CInt(SubType);
          if(!SubType)
            LogError("GetQuestAvSubTypes: Bledny PodTyp (0) w Typie "+CInt(Type)+", ["+QUESTS_CFG+"]");
            return array;
          endif
        endforeach
        return SubTypes;
      endif
    endif
  endif

  return array;

endfunction

//-----------------------------------------------------------------------------

function GetQuestObjective(Type, SubType)

  var QuestCfg := ReadConfigFile(QUESTS_CFG);
  
  return ClStr(GetConfigString(QuestCfg[QCFG_QUESTOBJS], QCFG_TYPE_TITLE+Type+"_"+SubType));

endfunction

//-----------------------------------------------------------------------------

function GetQuestItemExpTime(Level)

  var QuestCfg := ReadConfigFile(QUESTS_CFG);
  
  return CInt(GetConfigInt(QuestCfg[QCFG_ITEMEXPTIME], QCFG_LVL_TITLE+Level));

endfunction

//-----------------------------------------------------------------------------

function SetQuestElemCrInfo(QuestGroup, byref Elem, GProp, who := 0, Id := 0)

  var CrInfo    := array(PolCore().systime, 0, 0);
  if(IsPlayer(who))
    CrInfo[2] := who.acctname;
    CrInfo[3] := who.serial;
  endif
  Elem.SetProp(QAPROP_CRINFO, CrInfo);

  if(GProp)
    var GProps := GetQuestGlobalProps(QuestGroup);
    var oInfo  := GProps.GetProp(GProp);
    CrInfo[4]  := CInt(oInfo[4])+1;
    CrInfo[5]  := Id;
    GProps.SetProp(GProp, CrInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetQuestElemUpInfo(QuestGroup, byref Elem, GProp, who := 0, Id := 0)

  var UpInfo    := Elem.GetProp(QAPROP_UPINFO);
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

  Elem.SetProp(QAPROP_UPINFO, UpInfo);

  if(GProp)
    var GProps := GetQuestGlobalProps(QuestGroup);
    var oInfo  := GProps.GetProp(GProp);
    UpInfo[4]  := CInt(oInfo[4])+1;
    UpInfo[5]  := Id;
    GProps.SetProp(GProp, UpInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function SetQuestElemUseInfo(QuestGroup, byref Elem, GProp, who := 0, Id := 0)

  var UseInfo    := Elem.GetProp(QAPROP_USEINFO);
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

  Elem.SetProp(QAPROP_USEINFO, UseInfo);

  if(GProp)
    var GProps := GetQuestGlobalProps(QuestGroup);
    var oInfo  := GProps.GetProp(GProp);
    UseInfo[4] := CInt(oInfo[4])+1;
    UseInfo[5] := Id;
    GProps.SetProp(GProp, UseInfo);
  endif

endfunction

//-----------------------------------------------------------------------------

function AssignQuestStageData(QuestGroup, Id, byref QStageData)

  if(!QStageData)
    InitQuestStageData(QStageData);
  endif

  var Data  := GetQuestData(QuestGroup);
  var Stage := Data.FindElement(QSTAGE_TITLE+Id);
  if(!Stage)
    var Err := error;
    Err.errortext := "Nie znaleziono Etapu ID: "+Id+"!";
    return Err;
  endif

  QStageData.Type     := Stage.GetProp(QSTPROP_TYPE);
  QStageData.SubType  := Stage.GetProp(QSTPROP_SUBTYPE);
  QStageData.Title    := Stage.GetProp(QSTPROP_TITLE);
  QStageData.Tip      := Stage.GetProp(QSTPROP_TIP);
  QStageData.Targ     := Stage.GetProp(QSTPROP_TARG);
  QStageData.Loc      := Stage.GetProp(QSTPROP_LOC);
  QStageData.Flags    := Stage.GetProp(QAPROP_FLAGS);
  QStageData.Guards   := Stage.GetProp(QSTPROP_GUARDS);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AssignQuestData(QuestGroup, Id, byref QData)

  if(!QData)
    InitQuestData(QData);
  endif

  var Data  := GetQuestData(QuestGroup);
  var Quest := Data.FindElement(QUEST_TITLE+Id);
  if(!Quest)
    var Err := error;
    Err.errortext := "Nie znaleziono Questa ID: "+Id+"!";
    return Err;
  endif

  QData.Type      := Quest.GetProp(QPROP_TYPE);
  QData.SubType   := Quest.GetProp(QPROP_SUBTYPE);
  QData.Title     := Quest.GetProp(QPROP_TITLE);
  QData.Tip       := Quest.GetProp(QPROP_TIP);
  QData.Stages    := Quest.GetProp(QPROP_STAGES);
  QData.Price     := Quest.GetProp(QPROP_PRICE);
  QData.MaxTime   := Quest.GetProp(QPROP_MAXTIME);
  QData.Level     := Quest.GetProp(QPROP_LEVEL);
  QData.Chance    := Quest.GetProp(QPROP_CHANCE);
  QData.GuildType := Quest.GetProp(QPROP_GDTYPE);
  QData.Flags     := Quest.GetProp(QAPROP_FLAGS);
  QData.GuildCities := Quest.GetProp(QPROP_CITIES);

  if(!IsVarType(QData.GuildCities, VAR_ARRAY))
    QData.+GuildCities;
    QData.GuildCities := array;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetQuestNpcText(Type, Source, Dest)

  var QuestCfg := ReadConfigFile(QUESTS_CFG);
  var CfgElem  := QuestCfg[QCFG_NPCQUESTTEXT+Type];

  if(!CfgElem)
    LogError("GetQuestNpcText: Brak NpcQuestText ["+QCFG_NPCQUESTTEXT+Type+"]: ["+QuestCfg[QCFG_NPCQUESTTEXT+Type]+"]");
    return "*hmm*";
  endif

  var Count := GetConfigInt(CfgElem, "Count");
 
  if(!Count)
    LogError("GetQuestNpcText: Brak NpcQuestText ["+Count+"] ["+QCFG_NPCQUESTTEXT+Type+"]: ["+Count+"]");
    return "*hmm*";
  endif

  var Text := GetConfigString(CfgElem, "Text_"+RandomDiceRoll("1d"+CStr(Count)));

  if(!Text)
    LogError("GetQuestNpcText: Brak NpcQuestText Text ["+Count+"] ["+QCFG_NPCQUESTTEXT+Type+"]: ["+Text+"]");
    return "*hmm*";
  endif

  if(Text["%gs"])
    var Pos := find(Text,"%gs",0)+3;
    var Id  := CInt(Text[Pos, len(Text)]);
    if(!Id)
      Text["%gs"] := "";
    else
      Text["%gs"+Id] := ggm(Source, Id);
    endif
  endif

  if(Text["%gd"])
    var Pos := find(Text,"%gd",0)+3;
    var Id  := CInt(Text[Pos, len(Text)]);
    if(!Id)
      Text["%gd"] := "";
    else
      Text["%gd"+Id] := ggm(Dest, Id);
    endif
  endif

  return Text;

endfunction

//-----------------------------------------------------------------------------
