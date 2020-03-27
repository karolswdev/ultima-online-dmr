//-----------------------------------------------------------------------------
// Modified: 2005-05-23
//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";
include "questdata";
include "include/dices";

//-----------------------------------------------------------------------------
// Quest Types
//-----------------------------------------------------------------------------

const QTYPE_KILL             := 1;
const QTYPE_DESTROY          := 2;
const QTYPE_TAKE             := 3;
const QTYPE_RESCUE           := 4;
const QTYPE_GOTO             := 5;
const QTYPE_STEAL            := 6;
const QTYPE_CREATE           := 7;
const QTYPE_HARVEST          := 8;
const QTYPE_LOCKPICK         := 9;
const QTYPE_TAME             := 10;

//-----------------------------------------------------------------------------
// Quest SubTypes
//-----------------------------------------------------------------------------

const QSTYPE_NPC             := 1;
const QSTYPE_EXNPC           := 2;
const QSTYPE_ITEM            := 3;
const QSTYPE_PLAYER          := 4;
const QSTYPE_LOC             := 5;
const QSTYPE_FROMNPC         := 6;
const QSTYPE_ALCHEMY         := 7;
const QSTYPE_BLACKSMITH      := 8;
const QSTYPE_BOWCRAFTING     := 9;
const QSTYPE_CARPENTING      := 10;
const QSTYPE_TINKERING       := 11;
const QSTYPE_TAILORING       := 12;
const QSTYPE_COOKING         := 13;
//const QSTYPE_PAINTING        := 14;
//const QSTYPE_WEAVING_OLD         := 15; // not used ! !  2012-09-22
const QSTYPE_INSCRIPTION     := 16;
const QSTYPE_MAGICITEMMAKING := 17;
const QSTYPE_MINE            := 18;
const QSTYPE_CHOP            := 19;
const QSTYPE_FISH            := 20;
const QSTYPE_HERB            := 21;
const QSTYPE_FARMING         := 22;
const QSTYPE_HOUSE           := 23;
const QSTYPE_CARTOGRAPHY     := 24;
const QSTYPE_LOOTNPC         := 25;
const QSTYPE_LOCKTAKE        := 26;

//-----------------------------------------------------------------------------
// Quest Stage Flags
//-----------------------------------------------------------------------------

const QSTF_PREV_INDEPENDENT  := 0x00000001;
const QSTF_RANDOM_GUARD      := 0x00000002;
const QSTF_HIDDEN_LOC        := 0x00000004;
const QSTF_HIDDEN_TARG       := 0x00000008;
const QSTF_TARG_GROUP        := 0x00000010;
const QSTF_SIMPLE_LOC        := 0x00000020;

const QSTF_COMPLETED         := 0x00010000;
const QSTF_FAILED            := 0x00020000;
const QSTF_GUARDS            := 0x00040000;
const QSTF_GUARDS_CREATED    := 0x00080000;
const QSTF_GUARDS_KILLED     := 0x00100000;
const QSTF_TARG_CREATED      := 0x00200000;
const QSTF_TARG_COMPLETED    := 0x00400000;

//-----------------------------------------------------------------------------
// Quest Flags
//-----------------------------------------------------------------------------

const QF_CUSTOM_PRICE        := 0x00000001;
const QF_CUSTOM_MAXTIME      := 0x00000002;

const QF_COMPLETED           := 0x00010000;
const QF_FAILED              := 0x00020000;
const QF_FINISHED            := 0x00040000;

//-----------------------------------------------------------------------------
// Quest Loc Flags
//-----------------------------------------------------------------------------

const QLF_NOCHECK_Z          := 0x00000001;
const QLF_WORLD_Z            := 0x00000002;
const QLF_ACP_MULTI          := 0x00000004;

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Stage Addition & Verification
//-----------------------------------------------------------------------------
//=============================================================================

function AddQuestStage(QuestGroup, byref QStageData, who := 0)

  var Err := VerifyQuestStage(QStageData);

  if(Err == error)
    return Err;
  endif

  Err := error;

  var NextID := GetNextQuestStageID(QuestGroup);

  if(!NextID)
    Err.errortext := "Nie udalo sie pobrac nastepnego IDa questu.";
    return Err;
  endif
 
  var QuestData := GetQuestData(QuestGroup);
  var NewQuest  := QuestData.CreateElement(QSTAGE_TITLE+CStr(NextID));

  if(!NewQuest)
    Err.errortext := "Nie udalo sie stworzyc nowego elementu w QuestData.";
    return Err;
  endif

  VerifyQuestTips(QStageData);

  NewQuest.SetProp(QSTPROP_TYPE,    QStageData.Type);
  NewQuest.SetProp(QSTPROP_SUBTYPE, QStageData.SubType);
  NewQuest.SetProp(QSTPROP_TITLE,   QStageData.Title);
  NewQuest.SetProp(QSTPROP_TIP,     QStageData.Tip);
  NewQuest.SetProp(QSTPROP_TARG,    QStageData.Targ);
  NewQuest.SetProp(QSTPROP_LOC,     QStageData.Loc);
  NewQuest.SetProp(QAPROP_FLAGS,    QStageData.Flags);
  NewQuest.SetProp(QSTPROP_GUARDS,  QStageData.Guards);

  SetQuestElemCrInfo(QuestGroup, NewQuest, QGPROP_QSCRINFO, who, NextID);

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestTips(byref QData)

  var tIdx := QData.Tip.size();

  while(tIdx > 0)
    if(!QData.Tip[tIdx].size())
      QData.Tip.erase(tIdx);
    else
      break;
    endif
    tIdx := tIdx - 1;
  endwhile

  while(tIdx > 0)
    if(!QData.Tip[tIdx])
      QData.Tip[tIdx] := "";
    endif
    tIdx := tIdx - 1;
  endwhile

endfunction

//-----------------------------------------------------------------------------

function SetQuestStage(QuestGroup, QsID, byref QStageData, who := 0)

  var Err := VerifyQuestStage(QStageData);

  if(Err == error)
    return Err;
  endif

  Err := error;

  var QuestData := GetQuestData(QuestGroup);
  var Stage     := QuestData.FindElement(QSTAGE_TITLE+CStr(QsID));

  if(!Stage)
    Err.errortext := "Nie znaleziono Etapu Questa ID: "+QsID+"!";
    return Err;
  endif

  VerifyQuestTips(QStageData);

  Stage.SetProp(QSTPROP_TYPE,    QStageData.Type);
  Stage.SetProp(QSTPROP_SUBTYPE, QStageData.SubType);
  Stage.SetProp(QSTPROP_TITLE,   QStageData.Title);
  Stage.SetProp(QSTPROP_TIP,     QStageData.Tip);
  Stage.SetProp(QSTPROP_TARG,    QStageData.Targ);
  Stage.SetProp(QSTPROP_LOC,     QStageData.Loc);
  Stage.SetProp(QAPROP_FLAGS,    QStageData.Flags);
  Stage.SetProp(QSTPROP_GUARDS,  QStageData.Guards);

  SetQuestElemUpInfo(QuestGroup, Stage, QGPROP_QSUPINFO, who, QsID);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteQuestStage(QuestGroup, QsID)

  var QuestData := GetQuestData(QuestGroup);

  if(QuestData)
    var Quests := GetDataKeys(QuestData, QUEST_TITLE);
    var Stages, QElem;

    foreach Quest in Quests
      QElem  := QuestData.FindElement(QUEST_TITLE+Quest);
      Stages := QElem.GetProp(QPROP_STAGES);
      if(QsID in Stages)
        var Err := error;
        Err.errortext := "Etap ["+QsID+"] istnieje w Quescie ["+Quest+"] !";
        return Err;
      endif
    endforeach

    var Res := QuestData.DeleteElement(QSTAGE_TITLE+CStr(QsID));
    if(!Res)
      return Res;
    else
      return 1;
    endif
  else
    return QuestData;
  endif

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStage(byref QStageData)

  var Err := error;

  if(!QStageData.Title)
    Err.errortext := "Nie okreslony tytul.";
    return Err;
  elseif(!QStageData.Type or QStageData.Type > QTYPE_MAXID)
    Err.errortext := "Nie okreslony lub bledny Typ.";
    return Err;
  endif

  var SubTypes := GetQuestAvSubTypes(QStageData.Type);
  Err.errortext := "Bledny PodTyp.";

  foreach SubType in SubTypes
    if(SubType != QStageData.SubType)
      continue;
    endif

    case(SubType)
      QSTYPE_NPC:             Err := VerifyQuestStageNpc(QStageData);
      QSTYPE_EXNPC:           Err := VerifyQuestStageExistNpc(QStageData);
      QSTYPE_ITEM:            Err := VerifyQuestStageItem(QStageData);
      QSTYPE_PLAYER:          Err := VerifyQuestStagePlayer(QStageData);
                              if(Err)
                                Err := error;
                              endif
      QSTYPE_LOC:             Err := VerifyQuestStageLoc(QStageData);
                              if(Err)
                                Err := error;
                              endif
      QSTYPE_FROMNPC:         Err := VerifyQuestStageFromNpc(QStageData);
      QSTYPE_LOOTNPC:         Err := VerifyQuestStageLootNpc(QStageData);

      QSTYPE_ALCHEMY:
      QSTYPE_BLACKSMITH:
      QSTYPE_BOWCRAFTING:
      QSTYPE_CARPENTING:
      QSTYPE_TINKERING:
      QSTYPE_TAILORING:
      QSTYPE_COOKING:
      QSTYPE_INSCRIPTION:
      QSTYPE_CARTOGRAPHY:
      QSTYPE_MAGICITEMMAKING: Err := VerifyQuestStageCreate(QStageData);
                              if(Err)
                                Err := error;
                              endif
      QSTYPE_MINE:
      QSTYPE_CHOP:
      QSTYPE_HERB:
      QSTYPE_FISH:
      QSTYPE_FARMING:         Err := VerifyQuestStageMine(QStageData);
      QSTYPE_HOUSE:           Err := VerifyQuestStageHouse(QStageData);

      QSTYPE_LOCKTAKE:        Err := VerifyQuestStageLockTake(QStageData);
    endcase
  endforeach

  if(Err != error)
    Err := VerifyQuestStageGenLoc(QStageData);
  elseif(!Err.errortext and QStageData.Targ)
    QStageData.Loc := "";
  endif

  if(Err.errortext)
    return Err;
  endif

  Err := error;

  if(QStageData.Guards and QStageData.Guards.size())
    var i := 1;
    foreach Guard in (QStageData.Guards)
      if(!Guard)
        Err.errortext := "Nie okreslony straznik Nr: "+i;
        break;
      elseif(!GetNpcConfig(Guard))
        Err.errortext := "Nie istniejacy Template straznika Nr: "+i+" ["+Guard+"]";
        break;
      endif
      i := i + 1;
    endforeach
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageNpc(byref QStageData)

  var Err    := error;

  if(!QStageData.Targ)
    Err.errortext := "Nie okreslony cel.";
    return Err;
  endif

  if(QStageData.Flags & QSTF_TARG_GROUP)
    if(!QStageData.Targ.size())
      Err.errortext := "Nie zostala okreslona grupa Npecow.";
    else
      var i;
      for(i:=1;i<=QStageData.Targ.size();i:=i+1)
        if(!GetNpcConfig(QStageData.Targ[i].Template))
          Err.errortext := "Template Nr "+i+" ["+QStageData.Targ[i].Template+"] nie istnieje.";
          break;
        endif
      endfor
    endif
  elseif(!QStageData.Targ.Template)
    Err.errortext := "Nie zostal okreslony Template Npeca.";
  elseif(!GetNpcConfig(QStageData.Targ.Template))
    Err.errortext := "Template ["+QStageData.Targ.Template+"] nie istnieje.";
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStagePlayer(byref QStageData)

  var Err    := error;

  if(!QStageData.Targ)
    Err.errortext := "Nie okreslony cel.";
    return Err;
  endif

  if(QStageData.Flags & QSTF_TARG_GROUP)
    var i;
    if(!QStageData.Targ.size())
      Err.errortext := "Nie okreslono grupy SkID'ow.";
    else
      for(i:=1;i<=QStageData.Targ.size();i:=i+1)
        QStageData.Targ[i].SkID  := CInt(QStageData.Targ[i].SkID);
        QStageData.Targ[i].SkVal := CInt(QStageData.Targ[i].SkVal);
        if(!QStageData.Targ[i].SkID)
          Err.errortext := "Brak SkID w grupie, Nr: "+i;
          break;
        elseif(QStageData.Targ[i].SkVal > 105)
          Err.errortext := "Za duza wartosc w grupie Nr: "+i+" SkID: "+QStageData.Targ[i].SkID+", SkVal: "+QStageData.Targ[i].SkVal+" (105)";
          break;
        endif
      endfor
    endif
  elseif(!CInt(QStageData.Targ.SkID))
    QStageData.Targ.SkID := 0;
  else
    QStageData.Targ.SkID  := CInt(QStageData.Targ.SkID);
    QStageData.Targ.SkVal := CInt(QStageData.Targ.SkVal);
    if(QStageData.Targ.SkVal > 105)
      Err.errortext := "Za duza wartosc SkID: "+QStageData.Targ.SkID+", SkVal: "+QStageData.Targ.SkVal+" (105)";
    endif
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageItem(byref QStageData)

  var Err    := error;

  if(!QStageData.Targ)
    Err.errortext := "Nie okreslony cel.";
    return Err;
  endif

  if(QStageData.Flags & QSTF_TARG_GROUP)
    if(!QStageData.Targ.size())
      Err.errortext := "Nie okreslono grupy Item'ow.";
    else
      var i;
      for(i:=1;i<=QStageData.Targ.size();i:=i+1)
        VerifyQSItemPropriety(QStageData.Targ[i], QStageData.SubType, QStageData.Type, Err);
        if(Err.errortext)
          Err.errortext += " Grupa Nr: "+i+".";
          break;
        endif
      endfor
    endif
  else
    VerifyQSItemPropriety(QStageData.Targ, QStageData.SubType, QStageData.Type, Err);
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQSItemPropriety(byref Targ, byref SubType, byref Type, byref Err)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(Targ.ToObjType)
    if(Targ.ObjType >= Targ.ToObjType)
      Err.errortext := "Bledny zakres, ["+Targ.ObjType+" >= "+Targ.ToObjType+"].";
      return;
    else
      var n;
      for(n:=Targ.ObjType; n<=Targ.ToObjType; n+=1)
        if(!ItemCfg[n])
          Err.errortext := "Bledny zasieg ObjType'ow, ["+n+"] nie istnieje w ["+Targ.ObjType+"-"+Targ.ToObjType+"].";
          return;
        else
          VerifyQSItemAdditionalInfo(SubType, Type, n, Err);
          if(Err.errortext)
            return;
          endif
        endif
      endfor
    endif
  elseif(!ItemCfg[Targ.ObjType])
    Err.errortext := "Bledny ObjType ["+Targ.ObjType+"].";
    return;
  else
    VerifyQSItemAdditionalInfo(SubType, Type, Targ.ObjType, Err);
    if(Err.errortext)
      return;
    endif
  endif

  if(Targ.Color != error and Targ.Color < 0)
    Err.errortext := "Bledny kolor ["+Targ.Color+"].";
    return;
  endif

endfunction

//-----------------------------------------------------------------------------

function VerifyQSItemAdditionalInfo(byref SubType, byref Type, byref ObjType, byref Err)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  case(SubType)
    QSTYPE_ITEM:       case(Type)
                         QTYPE_LOCKPICK:       if(!ItemCfg[ObjType].LockPickDiff)
                                                 Err.errortext := "ObjType ["+ObjType+"] nie posiada trudnosci zamka.";
                                                 return;
                                               endif
                       endcase
  endcase

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageExistNpc(byref QStageData)

  var Err    := error;

  if(QStageData.Flags & QSTF_TARG_GROUP)
    var i;
    for(i:=1;i<=QStageData.Targ.size();i:=i+1)
      VerifyQSExistNpcPropriety(QStageData.Targ[i], Err);
      if(Err.errortext)
        Err.errortext += " Grupa Nr: "+i+".";
        break;
      endif
    endfor
  else
    VerifyQSExistNpcPropriety(QStageData.Targ, Err);
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQSExistNpcPropriety(byref Targ, byref Err)

  if(!Targ.Template and !Targ.Script)
    Targ.Template := "";
    Targ.Script   := "";
    Targ.Gender   := 0;
    return;
  elseif(Targ.Template and Targ.Script)
    Err.errortext := "Okreslono Template i Skrypt, mozna tylko jedno.";
    return;
  elseif(Targ.Template)
    if(!GetNpcConfig(Targ.Template))
      Err.errortext := "Template ["+Targ.Template+"] nie istnieje.";
      return;
    endif
  elseif(Targ.Script)
    if(!VerifyNpcScript(Targ.Script))
      Err.errortext := "Bledny skrypt Npeca: "+Targ.Script+".";
      return;
    endif
  endif

  if(Targ.Gender < 0 or Targ.Gender > 2)
    Err.errortext := "Bledna plec: "+Targ.Gender+" (0 - nie wazne, 1 - meska, 2 - zenska).";
    return;
  endif

endfunction

//-----------------------------------------------------------------------------

function VerifyNpcScript(Script)

  var AllTemps := GetAllNpcTemplates();

  foreach NpcTemp in AllTemps
    if(GetNpcConfig(NpcTemp).Script == Script)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageFromNpc(byref QStageData)

  var Err    := error;

  Err := VerifyQuestStageItem(QStageData);

  if(Err)
    Err := VerifyQuestStageExistNpc(QStageData);
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageLootNpc(byref QStageData)

  var Err    := error;

  Err := VerifyQuestStageItem(QStageData);

  if(Err)
    Err := VerifyQuestStageNpc(QStageData);
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageLockTake(byref QStageData)

  var Err    := error;

  Err := VerifyQuestStageItem(QStageData);

  if(Err.errortext)
    return Err;
  endif

  Err := error;

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var i;

  if(QStageData.Flags & QSTF_TARG_GROUP)

    if(!QStageData.Targ.size())
      Err.errortext := "Nie okreslono grupy Item'ow.";
    else
      for(i:=1;i<=QStageData.Targ.size();i:=i+1)
        if(!ItemCfg[QStageData.Targ[i].ContObjType])
          Err.errortext := "Bledny ObjType pojemnika ["+QStageData.Targ[i].ContObjType+"] nie istnieje, Grupa Nr: "+i;
          break;
        elseif(!ItemCfg[QStageData.Targ[i].ContObjType].Gump)
          Err.errortext := "Bledny ObjType pojemnika ["+QStageData.Targ[i].ContObjType+"] to nie pojemnik, Grupa Nr: "+i;
          break;
        endif

        if(Err.errortext)
          break;
        endif
      endfor
    endif

  else

    if(!ItemCfg[QStageData.Targ.ContObjType])
      Err.errortext := "Bledny ObjType pojemnika ["+QStageData.Targ.ContObjType+"]";
    elseif(!ItemCfg[QStageData.Targ.ContObjType].Gump)
      Err.errortext := "Bledny ObjType pojemnika ["+QStageData.Targ.ContObjType+"] to nie pojemnik";
    endif

  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageLoc(byref QStageData)

  if(!QStageData.Targ)
    return 1;
  endif

  var Err    := error;
  var Rlms   := Realms();

  if(QStageData.Flags & QSTF_TARG_GROUP)
    var i;
   
    if(!QStageData.Targ.size())
      Err.errortext := "Nie okreslona grupa lokacji.";
    else
      for(i:=1;i<=QStageData.Targ.size();i:=i+1)
        if(!QStageData.Targ[i].x or !QStageData.Targ[i].y or QStageData.Targ[i].z == error)
          Err.errortext := "Bledna lokacja, zle wspolrzedne Nr: "+i+".";
        elseif(!QStageData.Targ[i].realm or len(QStageData.Targ[i].realm) == 1)
          QStageData.Targ[i].realm := GetRealmById(CInt(QStageData.Targ[i].realm));
          if(!QStageData.Targ[i].realm)
            Err.errortext := "Bledna nazwa krainy w lokacji Nr: "+i+".";
          endif
        elseif(!Rlms[QStageData.Targ[i].realm])
          Err.errortext := "Bledna nazwa krainy w lokacji Nr: "+i+".";
        endif
      endfor
    endif
  else
    if(!QStageData.Targ.x or !QStageData.Targ.y or QStageData.Targ.z == error)
      Err.errortext := "Bledna lokacja, zle wspolrzedne.";
    elseif(!QStageData.Targ.realm or len(QStageData.Targ.realm) == 1)
      QStageData.Targ.realm := GetRealmById(CInt(QStageData.Targ.realm));
      if(!QStageData.Targ.realm)
        Err.errortext := "Bledna nazwa krainy w lokacji";
      endif
    elseif(!Rlms[QStageData.Targ.realm])
      Err.errortext := "Bledna nazwa krainy w lokacji";
    endif
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageGenLoc(byref QStageData)

  var Err := error;

  if(!QStageData.Loc)
    Err.errortext := "Nie okreslona nazwa grupy lokacji.";
  else
    var Data  := GetQuestData(QGROUP_LOCDATA);
    var Group := Data.FindElement(QLOC_TITLE+CStr(QStageData.Loc));
    if(!Group)
      Err.errortext := "Bledna nazwa grupy lokacji (nie istnieje): "+QStageData.Loc;
    else
      var Locs := Group.GetProp(QLPROP_LOCS);
      if(!Locs or !Locs.size())
        Err.errortext := "Grupa lokacji ["+QStageData.Loc+"] jest pusta!";
      endif
    endif
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageHouse(byref QStageData)

  var Err := error;

  if(QStageData.Targ)
    Err.errortext := "PodTyp HOUSE nie powinien miec zadnego celu.";
  else
    QStageData.Targ := 0;
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageCreate(byref QStageData)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(QStageData.Flags & QSTF_TARG_GROUP)
    var i;
    for(i:=1;i<=QStageData.Targ.size();i:=i+1)
      if(QStageData.Targ[i].MatObjType)
        if(QStageData.SubType != QSTYPE_CARTOGRAPHY and !ItemCfg[QStageData.Targ[i].MatObjType])
          var Err := error;
          Err.errortext := "Bledny ObjType materialu ["+QStageData.Targ[i].MatObjType+"], Nr: "+i;
          return Err;
        endif
      endif

      if(QStageData.Targ[i].SkVal > 100)
        QStageData.Targ[i].SkVal := 100;
      elseif(QStageData.Targ[i].SkVal < 0)
        QStageData.Targ[i].SkVal := 0;
      endif

      if(QStageData.Targ[i].ExpChance > 100)
        QStageData.Targ[i].ExpChance := 100;
      elseif(QStageData.Targ[i].ExpChance < 0)
        QStageData.Targ[i].ExpChance := 0;
      endif
    endfor
  else
    if(QStageData.Targ.MatObjType)
      if(QStageData.SubType != QSTYPE_CARTOGRAPHY and !ItemCfg[QStageData.Targ.MatObjType])
        var Err := error;
        Err.errortext := "Bledny ObjType materialu ["+QStageData.Targ.MatObjType+"]";
        return Err;
      endif
    endif

    if(QStageData.Targ.SkVal > 100)
      QStageData.Targ.SkVal := 100;
    elseif(QStageData.Targ.SkVal < 0)
      QStageData.Targ.SkVal := 0;
    endif

    if(QStageData.Targ.ExpChance > 100)
      QStageData.Targ.ExpChance := 100;
    elseif(QStageData.Targ.ExpChance < 0)
      QStageData.Targ.ExpChance := 0;
    endif
  endif

  return VerifyQuestStageItem(QStageData);

endfunction

//-----------------------------------------------------------------------------

function VerifyQuestStageMine(byref QStageData)

  if(QStageData.Flags & QSTF_TARG_GROUP)
    var i;
    for(i:=1;i<=QStageData.Targ.size();i:=i+1)
      if(QStageData.Targ[i].SkVal > 100)
        QStageData.Targ[i].SkVal := 100;
      elseif(QStageData.Targ[i].SkVal < 0)
        QStageData.Targ[i].SkVal := 0;
      endif
    endfor
  else
    if(QStageData.Targ.SkVal > 100)
      QStageData.Targ.SkVal := 100;
    elseif(QStageData.Targ.SkVal < 0)
      QStageData.Targ.SkVal := 0;
    endif
  endif

  return VerifyQuestStageItem(QStageData);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Addition & Veryfication
//-----------------------------------------------------------------------------
//=============================================================================

function AddQuest(QuestGroup, byref QData, who := 0)

  var Err := VerifyQuest(QuestGroup, QData);

  if(Err == error)
    return Err;
  endif

  Err := error;

  var NextID := GetNextQuestID(QuestGroup);

  if(!NextID)
    Err.errortext := "Nie udalo sie pobrac nastepnego IDa questu.";
    return Err;
  endif
 
  var QuestData := GetQuestData(QuestGroup);
  var NewQuest  := QuestData.CreateElement(QUEST_TITLE+CStr(NextID));

  if(!NewQuest)
    Err.errortext := "Nie udalo sie stworzyc nowego elementu w QuestData.";
    return Err;
  endif

  VerifyQuestTips(QData);

  NewQuest.SetProp(QPROP_STAGES,  QData.Stages);
  NewQuest.SetProp(QAPROP_FLAGS,  QData.Flags);
  NewQuest.SetProp(QPROP_TITLE,   QData.Title);
  NewQuest.SetProp(QPROP_TIP,     QData.Tip);
  NewQuest.SetProp(QPROP_LEVEL,   QData.Level);
  NewQuest.SetProp(QPROP_TYPE,    QData.Type);
  NewQuest.SetProp(QPROP_SUBTYPE, QData.SubType);
  NewQuest.SetProp(QPROP_PRICE,   QData.Price);
  NewQuest.SetProp(QPROP_MAXTIME, QData.MaxTime);
  NewQuest.SetProp(QPROP_CHANCE,  QData.Chance);
  NewQuest.SetProp(QPROP_GDTYPE,  QData.GuildType);
  NewQuest.SetProp(QPROP_CITIES,QData.GuildCities);

  SetQuestElemCrInfo(QuestGroup, NewQuest, QGPROP_QCRINFO, who, NextID);

  return NextID;

endfunction

//-----------------------------------------------------------------------------

function SetQuest(QuestGroup, QID, byref QData, who := 0)

  var Err := VerifyQuest(QuestGroup, QData);

  if(Err == error)
    return Err;
  endif

  Err := error;

  var QuestData := GetQuestData(QuestGroup);
  var Quest     := QuestData.FindElement(QUEST_TITLE+CStr(QID));

  if(!Quest)
    Err.errortext := "Nie znaleziono Questa ID: "+QID+"!";
    return Err;
  endif

  VerifyQuestTips(QData);

  Quest.SetProp(QPROP_STAGES,  QData.Stages);
  Quest.SetProp(QAPROP_FLAGS,  QData.Flags);
  Quest.SetProp(QPROP_TITLE,   QData.Title);
  Quest.SetProp(QPROP_TIP,     QData.Tip);
  Quest.SetProp(QPROP_LEVEL,   QData.Level);
  Quest.SetProp(QPROP_TYPE,    QData.Type);
  Quest.SetProp(QPROP_SUBTYPE, QData.SubType);
  Quest.SetProp(QPROP_PRICE,   QData.Price);
  Quest.SetProp(QPROP_MAXTIME, QData.MaxTime);
  Quest.SetProp(QPROP_CHANCE,  QData.Chance);
  Quest.SetProp(QPROP_GDTYPE,  QData.GuildType);
  Quest.SetProp(QPROP_CITIES,  QData.GuildCities);

  SetQuestElemUpInfo(QuestGroup, Quest, QGPROP_QUPINFO, who, QID);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteQuest(QuestGroup, QID)

  var QuestData := GetQuestData(QuestGroup);

  if(QuestData)
    var Res := QuestData.DeleteElement(QUEST_TITLE+CStr(QID));
    if(!Res)
      return Res;
    else
      return 1;
    endif
  else
    return QuestData;
  endif

endfunction

//-----------------------------------------------------------------------------

function VerifyQuest(QuestGroup, byref QData)

  var Err := error;

  QData.Type      := CInt(QData.Type);
  QData.SubType   := CInt(QData.SubType);
  QData.Level     := CInt(QData.Level);
  QData.Flags     := CInt(QData.Flags);
  QData.Chance    := CInt(QData.Chance);
  QData.MaxTime   := CInt(QData.MaxTime);
  QData.GuildType := CInt(QData.GuildType);

  if(!QData.Title)
    Err.errortext := "Brak tytulu.";
  elseif(!QData.Stages or !QData.Stages.size())
    Err.errortext := "Brak etapow.";
  elseif(!QData.Level or QData.Level > QUEST_MAXLVL)
    Err.errortext := "Nie okreslono poziomu.";
  elseif(!QData.Type or QData.Type > QTYPE_MAXID)
    Err.errortext := "Nie podano Typu lub jest bledny.";
  elseif(!QData.SubType or QData.SubType > QSTYPE_MAXID)
    Err.errortext := "Nie podano PodTypu lub jest bledny.";
  elseif(QData.Flags & QF_CUSTOM_PRICE and (!QData.Price.size() or !QData.Price))
    Err.errortext := "Bledna nagroda, pusta tablica lub zla wartosc. ["+QData.Price+"]";
  elseif(QData.Flags & QF_CUSTOM_MAXTIME and QData.MaxTime < 120)
    Err.errortext := "Bledny limit czasowy, za maly - "+QData.MaxTime+" sec?";
  elseif(QData.Chance < 3 or QData.Chance > 100)
    Err.errortext := "Bledna szansa, ["+QData.Chance+"], zakres [3-100]";
  else
    var Data := GetQuestData(QuestGroup);
    var i,n;
    
    if(QData.Flags & QF_CUSTOM_PRICE)
      var ItemCfg := ReadConfigFile(ITEMS_CFG);

      for(i:=1;i<=QData.Price.size();i:=i+1)
        if(QData.Price[i].ToObjType)

          if(QData.Price[i].ObjType >= QData.Price[i].ToObjType)
            Err.errortext := "Bledny zakres w nagrodzie Nr: "+i+", ["+QData.Price[i].ObjType+" >= "+QData.Price[i].ToObjType+"]";
          else
            for(n:=QData.Price[i].ObjType;n<=QData.Price[i].ToObjType;n:=n+1)
              if(!ItemCfg[n])
                Err.errortext := "Bledny zasieg ObjType'ow, ["+n+"] nie istnieje w ["+QData.Price[i].ObjType+"-"+QData.Price[i].ToObjType+"] Nr: "+i;
                break;
              endif
            endfor
          endif

        elseif(!ItemCfg[QData.Price[i].ObjType])
          Err.errortext := "Bledny ObjType nagrody ["+QData.Price[i].ObjType+"], Nr: "+i;
        endif

        if(!CheckDiceRoll(QData.Price[i].Amount) or !CInt(QData.Price[i].Amount))
          Err.errortext := "Nie okreslona ilosc przedmiotu nagrody, Nr: "+i;
        endif

        if(QData.Price[i].Chance < 3 or QData.Price[i].Chance > 100)
          Err.errortext := "Bledna szansa na przedmiot nagrody ["+QData.Price[i].Chance+"], zakres 3-100, Nr: "+i;
        endif

        if(Err.errortext)
          break;
        endif
      endfor
    endif

    if(Err.errortext)
      return Err;
    endif

    for(i:=1;i<=QData.Stages.size();i:=i+1)
      if(!Data.FindElement(QSTAGE_TITLE+CStr(QData.Stages[i])))
        Err.errortext := "Bledny etap ["+QData.Stages[i]+"], nie istnieje. Nr: "+i;
        break;
      endif
    endfor
  endif

  if(Err.errortext)
    return Err;
  endif

  return 1;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Misc Funcs
//-----------------------------------------------------------------------------
//=============================================================================

function GetQuestSubTypeFromSkID(SkID)

  case(SkID)
    SKILLID_ALCHEMY:         return QSTYPE_ALCHEMY;
    SKILLID_BLACKSMITHY:     return QSTYPE_BLACKSMITH;
    SKILLID_BOWCRAFT:        return QSTYPE_BOWCRAFTING;
    SKILLID_CARPENTRY:       return QSTYPE_CARPENTING;
    SKILLID_TINKERING:       return QSTYPE_TINKERING;
    SKILLID_TAILORING:       return QSTYPE_TAILORING;
    SKILLID_COOKING:         return QSTYPE_COOKING;
    SKILLID_INSCRIPTION:     return QSTYPE_INSCRIPTION;
    SKILLID_MAGICITEMMAKING: return QSTYPE_MAGICITEMMAKING;
    SKILLID_MINING:          return QSTYPE_MINE;
    SKILLID_LUMBERJACKING:   return QSTYPE_CHOP;
    SKILLID_FISHING:         return QSTYPE_FISH;
    SKILLID_HERDING:         return QSTYPE_HERB;
    SKILLID_FARMING:         return QSTYPE_FARMING;
    SKILLID_CARTOGRAPHY:     return QSTYPE_CARTOGRAPHY;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetQuestSkIDFromSubType(SkID)

  case(SkID)
    QSTYPE_ALCHEMY:          return SKILLID_ALCHEMY;
    QSTYPE_BLACKSMITH:       return SKILLID_BLACKSMITHY;
    QSTYPE_BOWCRAFTING:      return SKILLID_BOWCRAFT;
    QSTYPE_CARPENTING:       return SKILLID_CARPENTRY;
    QSTYPE_TINKERING:        return SKILLID_TINKERING;
    QSTYPE_TAILORING:        return SKILLID_TAILORING;
    QSTYPE_COOKING:          return SKILLID_COOKING;
    QSTYPE_INSCRIPTION:      return SKILLID_INSCRIPTION;
    QSTYPE_MAGICITEMMAKING:  return SKILLID_MAGICITEMMAKING;
    QSTYPE_MINE:             return SKILLID_MINING;
    QSTYPE_CHOP:             return SKILLID_LUMBERJACKING;
    QSTYPE_FISH:             return SKILLID_FISHING;
    QSTYPE_HERB:             return SKILLID_HERDING;
    QSTYPE_FARMING:          return SKILLID_FARMING;
    QSTYPE_CARTOGRAPHY:      return SKILLID_CARTOGRAPHY;
  endcase

  return error;

endfunction

//-----------------------------------------------------------------------------

function CreateQuestPrice(who, QuestGroup, QId)

  var QuestData := GetQuestData(QuestGroup);
  var QuestCfg  := ReadConfigFile(QUESTS_CFG);

  if(QuestData and QuestCfg)
    var Quest := QuestData.FindElement(QUEST_TITLE+QId);
    if(Quest)
      var pPack  := CreateItemAtTempLoc(UOBJ_BACKPACK);
      var Prices := Quest.GetProp(QPROP_PRICE);
      var Flags  := Quest.GetProp(QAPROP_FLAGS);

      if(!pPack)
        LogError("CreateQuestPrice: Nie udalo sie stworzyc plecaka na nagrode: ["+pPack+"]");
        return 0;
      endif

      if(Prices and Flags & QF_CUSTOM_PRICE)
        if(CreateQuestPriceFromTemp(pPack, Prices))
          if(MoveToPack(pPack, who))
            return 1;
          endif
        endif
      else
        if(!QuestCfg[QCFG_PRICES])
          LogError("CreateQuestPrice: Nie znaleziono elementu ["+QCFG_PRICES+"] w konfigu questow.");
        endif

        var Type    := Quest.GetProp(QPROP_TYPE);
        var SubType := Quest.GetProp(QPROP_SUBTYPE);
        var Level   := Quest.GetProp(QPROP_LEVEL);

        var LootGroup := GetConfigString(QuestCfg[QCFG_PRICES], QCFG_TYPE_TITLE+Type+"_"+SubType+"_"+Level);

        if(!LootGroup)
          LootGroup := GetConfigString(QuestCfg[QCFG_PRICES], QCFG_TYPE_TITLE+Type+"_"+Level);
        endif

        if(!LootGroup)
          LogError("CreateQuestPrice: Nie znaleziono nagrod w konfigu questow. Type: "+Type+" SubType: "+SubType+" Lvl: "+Level);
        else
          LootGroup := SplitWords(LootGroup);
          if(LootGroup.size() == 1)
            LootGroup := CInt(LootGroup[1]);
          else
            LootGroup := CInt(LootGroup[RandomInt(LootGroup.size())+1]);
          endif
        endif

        CreateLoot(pPack, LootGroup, QPRICES_CFG, 1 + Level + HaveChance(50));
        if(MoveToPack(pPack, who))
          return 1;
        endif
      endif
      DestroyItem(pPack);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CreateQuestPriceFromTemp(pPack, byref Prices)

  var ObjType, Amount, Item;
  
  foreach Price in Prices
    if(RandomInt(100) > Price.Chance)
      continue;
    endif

    if(Price.ToObjType)
      ObjType := Price.ObjType + RandomInt(Price.ToObjType - Price.ObjType);
    else
      ObjType := Price.ObjType;
    endif

    Amount := RandomDiceRoll(Price.Amount);

    if(IsStackableObj(ObjType))
      Item   := CreateItemInCont(pPack, ObjType, Amount);
    else
      var i;
      for(i:=1;i<=Amount;i:=i+1)
        Item   := CreateItemInCont(pPack, ObjType, 1);
        if(!Item)
          break;
        endif
      endfor
    endif

    if(!Item)
      LogError("CreateQuestPriceFromTemp: Nie udalo sie stworzyc nagrody ["+Price+"]: "+Item, LOGF_WORLD);
      return 0;
    endif

    if(Amount == 1)
      if(!IsStackableObj(ObjType))
        if(Price.Color)
          Item.color := Price.Color;
        endif
        if(Price.Name)
          SetName(Item, Price.Name);
        endif
      endif
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsValidQuestGroup(QuestGroup)

  var qCfg := ReadConfigFile(QUESTS_CFG);
  var Keys := GetConfigStringArray(qCfg["Groups"], "Group");

  if(QuestGroup in Keys)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetQuestGroupName(QuestGroup)

  var qCfg := ReadConfigFile(QUESTS_CFG);
  var Name := GetConfigString(qCfg["GroupNames"], QuestGroup);

  return ClStr(Name);

endfunction

//-----------------------------------------------------------------------------

function GetQuestTitle(QuestGroup, QuestId)

  var QuestData := GetQuestData(QuestGroup);
  var Quest     := QuestData.FindElement(QUEST_TITLE + QuestId);
  var Title     := Quest.GetProp(QPROP_TITLE);

  return Title;

endfunction

//-----------------------------------------------------------------------------
