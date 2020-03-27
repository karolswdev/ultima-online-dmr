//-----------------------------------------------------------------------------
// Modified: 2005-05-31
//-----------------------------------------------------------------------------

include ":quests:quests";
include ":quests:questloc";
include ":quests:questnpc";

include "include/logerror";
include "include/unicode";
include "include/gump_utils";
include "include/coords";
include "include/findcity";

include ":housing:common";

//-----------------------------------------------------------------------------

/*QGLOCF_MURDER do QGLOCF_NEUTRAL 0x1 - 0x4*/
const QCRF_QGD_ONLY          := 0x00000008;

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Creation
//-----------------------------------------------------------------------------
//=============================================================================

function InitQuestGenData(byref GenData)

  GenData := struct;

  GenData.+Source     := 0;
  GenData.+Player     := 0;
  GenData.+GdType     := 0;
  GenData.+QuestTypes := array;
  GenData.+CrFlags    := 0;
  GenData.+Level      := 0;
  GenData.+Except     := array;
  GenData.+QuestGroup := QGROUP_DEFAULT;
  GenData.+City       := "";

endfunction

//-----------------------------------------------------------------------------

function GenerateQuestIDs(byref GenData, byref QuestIDs)

  var QData  := GetQuestData(GenData.QuestGroup);
  var QTypes := array;
  var Quest, GdType, Skill, QTarg, Cities;

  QuestIDs := array;

  if(GenData.Level < 0 or GenData.Level > 5)
    return;
  endif

  foreach QuestID in GetDataKeys(QData, QUEST_TITLE)
    QuestID   := CInt(QuestID);

    if(QuestID in GenData.Except)
      continue;
    endif

    Quest     := QData.FindElement(QUEST_TITLE+QuestID);

    if(!Quest)
      LogError("GenerateQuestIDs: Nie znaleziono QID ["+QuestID+"]!");
      continue;
    endif

    if(GenData.Level and GenData.Level != Quest.GetProp(QPROP_LEVEL))
      continue;
    endif

    if(RandomInt(100) > Quest.GetProp(QPROP_CHANCE))
      continue;
    endif

    GdType := Quest.GetProp(QPROP_GDTYPE);
    Cities := Quest.GetProp(QPROP_CITIES);

    if(GenData.GdType)
      if(GdType or GenData.CrFlags & QCRF_QGD_ONLY)
        if(GdType != GenData.GdType)
          continue;
        endif
      endif
    endif

    if(GenData.City and Cities and Cities.size())
      if(!(GenData.City in Cities))
        continue;
      endif
    endif

    QTypes[1] := Quest.GetProp(QPROP_TYPE);
    QTypes[2] := Quest.GetProp(QPROP_SUBTYPE);
    if(GenData.QuestTypes.size())
      if(!(QTypes in (GenData.QuestTypes)))
        continue;
      endif
    endif

    Skill := 1;

    if(GenData.Player)
      case(QTypes[1])
        QTYPE_CREATE:
        QTYPE_HARVEST:  Skill := GetQuestSkIDFromSubType(QTypes[2]);
                        if(Skill != error)
                          Skill := GetEffectiveSkill(GenData.Player, Skill);
                          foreach Stage in (Quest.GetProp(QPROP_STAGES))
                            if(!Skill)
                              break;
                            endif
                            QTarg := QData.FindElement(QSTAGE_TITLE+Stage);
                            if(QTarg.GetProp(QSTPROP_SUBTYPE) == QTypes[2])
                              if(QTarg.GetProp(QAPROP_FLAGS) & QSTF_TARG_GROUP)
                                QTarg := QTarg.GetProp(QSTPROP_TARG);
                                foreach Targ in QTarg
                                  if(Targ.SkVal <= Skill)
                                    break;
                                  elseif(_Targ_iter == QTarg.size())
                                    Skill := 0;
                                  endif
                                endforeach
                              else
                                QTarg := QTarg.GetProp(QSTPROP_TARG);
                                if(QTarg.SkVal > Skill)
                                  Skill := 0;
                                endif
                              endif
                            endif
                          endforeach
                        else
                          Skill := 1;
                        endif

        QTYPE_TAME:     if(GetBaseSkill(GenData.Player, SKILLID_TAMING) < QUEST_MINSK_VAL)
                          Skill := 0;
                        endif
      endcase
    endif

    if(!Skill)
      continue;
    endif

    QuestIDs.append(QuestID);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GenerateQuest(byref GenData)

  if(!GenData.Level)
    return 0;
  endif

  var GenID    := error;
  var GenTries := 0;
  var QuestIDs;

  GenerateQuestIDs(GenData, QuestIDs);

  if(!QuestIDs.size())
    GenID.errortext := "Nie znaleziono odpowiednich Questow.  Lvl: ["+GenData.Level+"] GdT: ["+GenData.GdType+"] Ex: ["+GenData.Except+"] Qt: ["+GenData.QuestTypes+"]";
  else
    while(GenTries < QUEST_MAXGEN_TRIES)
      GenID    := QuestIDs[RandomInt(QuestIDs.size())+1];
      if(GenID)
        GenID    := CreateQuest(GenData.QuestGroup, GenID, GenData.Source, GenData.Player, GenData.CrFlags);
        if(GenID)
          return GenID;
        endif
        GenTries := GenTries + 1;
      endif
    endwhile
  endif

  return GenID;

endfunction

//-----------------------------------------------------------------------------

function CreateQuest(QuestGroup, Id, Source := 0, Player := 0, CrFlags := 0)

  var QuestItem := error;
  var QuestData := GetQuestData(QuestGroup);
  var Quest     := QuestData.FindElement(QUEST_TITLE+Id);
  var QuestCfg  := ReadConfigFile(QUESTS_CFG);
  var Title     := Quest.GetProp(QPROP_TITLE);

  if(!Quest)
    QuestItem.errortext := "Nie znaleziono Questa ID: "+Id+" w grupie '"+QuestGroup+"'!";
    return QuestItem;
  endif

  var QCrData, Res, StageElem;

  InitCrQuestData(QCrData);
  AssignCrQuestData(Quest, QCrData);

  QCrData.Id       := Id;
  QCrData.QuestGroup := QuestGroup;
  QCrData.+CrFlags := CrFlags;

  if(Source.serial)
    QCrData.Source := Source;
  endif

  if(IsPlayer(Player))
    QCrData.Player  := Player;
  endif

  foreach Stage in (QCrData.Stages)

    QCrData.CurStage := QCrData.CurStage + 1;

    StageElem := QuestData.FindElement(QSTAGE_TITLE+Stage.Id);
    if(!StageElem)
      QuestItem.errortext := "Nie znaleziono Etapu Questa ID: ["+Stage.Id+"] zapisanego w Quescie ID: ["+Id+"] !";
      return QuestItem;
    endif

    Res := CreateQuestStage(StageElem, Stage, QCrData);
    if(!Res)
      return Res;
    endif

  endforeach

  QCrData.CurStage := 1;

  if(!QCrData.MaxTime)
    QCrData.MaxTime := GetConfigInt(QuestCfg[QCFG_MAXQTIME], QCFG_LVL_TITLE+QCrData.Level);
    if(!QCrData.MaxTime)
      Res := error;
      Res.errortext := "Nie udalo sie pobrac MaxTime Questu! : ["+QCrData.MaxTime+"], Lvl: "+QCrData.Level;
      return Res;
    endif
  endif

  if(QCrData.Player)
    QCrData.Player := QCrData.Player.serial;
  endif

  if(QCrData.Source)
    QCrData.Source := QCrData.Source.serial;
  endif

  SetQuestElemUseInfo(QuestGroup, Quest, QGPROP_QUSEINFO, Player);

  QuestItem := CreateItemAtTempLoc(UOBJ_QUESTTICKET);
  if(Title)
    if(Title.size() != error)
      Title := ConvPlLetters(Title);
      ConvToAscii(Title);
    endif
    SetName(QuestItem, Title);
    SetTooltipProp(QuestItem, "Zadanie", GetQuestGroupName(QuestGroup));
  endif
  SetObjProperty(QuestItem, QGPROP_QCRINFO, QCrData);
  return QuestItem;

endfunction

//-----------------------------------------------------------------------------

function CreateQuestStage(byref StageElem, byref Stage, byref QCrData)

  var Res, LocGroup;
  AssignCrQuestStageData(StageElem, Stage);
  AssignQuestStageGuards(StageElem, Stage);

  var sLoc := StageElem.GetProp(QSTPROP_LOC);
//  var Flags := StageElem.GetProp(QAPROP_FLAGS);

  if(Stage.Flags & QSTF_SIMPLE_LOC)
    LocGroup := sLoc;
  else
    LocGroup := GetQuestLocGroup(sLoc);

    if(LocGroup == error)
      return LocGroup;
    endif
  endif

  Res := GenQuestStageTarget(StageElem, Stage, LocGroup, QCrData);
  if(!Res)
    return Res;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AssignQuestStageGuards(byref StageElem, byref Stage)

  var Guards := StageElem.GetProp(QSTPROP_GUARDS);
  var GuardElem;

  if(Guards.size() and Guards)
    SetBitOn(Stage.Flags, QSTF_GUARDS);
    Stage.Guards := array;
    InitCrQuestGuard(GuardElem);

    if(Stage.Flags & QSTF_RANDOM_GUARD)
      var i;
      for(i:=1;i<=Guards.size();i:=i+1)
        GuardElem.Template := Guards[RandomInt(Guards.size())+1];
        Stage.Guards.append(GuardElem);
      endfor
    else
      foreach GuardTemp in Guards
        GuardElem.Template := GuardTemp;
        Stage.Guards.append(GuardElem);
      endforeach
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GenQuestStageTarget(byref StageElem, byref Stage, byref LocGroup, byref QCrData)

  case(Stage.SubType)
    QSTYPE_NPC:              return GenQuestCrTargNpc(StageElem, Stage, LocGroup);
    QSTYPE_EXNPC:            return GenQuestCrTargExNpc(StageElem, Stage, LocGroup, QCrData);
    QSTYPE_ITEM:             return GenQuestCrTargItem(StageElem, Stage, LocGroup);
    QSTYPE_PLAYER:           return GenQuestCrTargPlayer(Stage, QCrData);
    QSTYPE_LOC:              return GenQuestCrTargLoc(StageElem, Stage, LocGroup);
    QSTYPE_FROMNPC:          return GenQuestCrTargFromNpc(StageElem, Stage, LocGroup, QCrData);
    QSTYPE_LOOTNPC:          return GenQuestCrTargLootNpc(StageElem, Stage, LocGroup);
    QSTYPE_ALCHEMY:          
    QSTYPE_BLACKSMITH:
    QSTYPE_BOWCRAFTING:
    QSTYPE_CARPENTING:
    QSTYPE_TINKERING:
    QSTYPE_TAILORING:
    QSTYPE_COOKING:
    QSTYPE_INSCRIPTION:
    QSTYPE_CARTOGRAPHY:
    QSTYPE_MAGICITEMMAKING:  return GenQuestCrTargSkill(StageElem, Stage, QCrData);
    QSTYPE_MINE:
    QSTYPE_CHOP:
    QSTYPE_FISH:
    QSTYPE_HERB:
    QSTYPE_FARMING:          return GenQuestCrTargMine(StageElem, Stage, LocGroup, QCrData);
    QSTYPE_HOUSE:            return GenQuestCrTargHouse(Stage, LocGroup, QCrData);
    QSTYPE_LOCKTAKE:         return GenQuestCrTargLockTake(StageElem, Stage, LocGroup);
  endcase

  var Err := error;
  Err.errortext := "Bledny PodTyp! ["+Stage.SubType+"] zapisany w Etapie Questa ID: "+Stage.Id;
  return Err;

endfunction

//-----------------------------------------------------------------------------

function GetQuestStageDefLoc(byref Stage, byref LocGroup)

  if(Stage.Flags & QSTF_SIMPLE_LOC)
    Stage.Loc := LocGroup; // jak lokacja jest prosta to w LocGroup bedzie po prostu struct x,y,z,realm.
    return 1;
  endif

  var Res      := error;
  var Tries    := 0;
  Stage.Loc    := 0;
  var LocRange;

  while(!Stage.Loc and Tries < QUEST_MAX_LOCTRIES)
    LocRange := GenQuestLocFromGroup(LocGroup);
    if(!LocRange)
      Res.errortext := "Nie udalo sie okreslic zasiegu lokacji w Etapie Questa ID: "+Stage.Id+" !";
      return Res;
    endif

    Stage.Loc := GenQuestStageLoc(LocRange, LocGroup);
    Tries     := Tries + 1;
  endwhile

  if(!Stage.Loc)
    Res.errortext := "Nie udalo sie okreslic lokacji z zasiegu ["+LocRange+"] w Etapie Questa ID: "+Stage.Id;
    return Res;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargNpc(byref StageElem, byref Stage, byref LocGroup, grId := 0)

  InitQCrTargNpc(Stage.Targ);

  var Flags    := StageElem.GetProp(QAPROP_FLAGS);
  var Res      := GetQuestStageDefLoc(Stage, LocGroup);
  var Targ     := StageElem.GetProp(QSTPROP_TARG);
  var gId      := 1;

  if(!Res)
    return Res;
  endif

  if(Flags & QSTF_TARG_GROUP)
    if(grId)
      gId := grId;
    else
      gId := RandomInt(Targ.size())+1;
    endif
    Targ := Targ[gId];
  endif

  if(!Targ)
    Res := error;
    Res.errortext := "Brak celu w Etapie Questa ID: "+Stage.Id+"!";
    return Res;
  endif

  Stage.Targ.Template := Targ.Template;
  Stage.Targ.Color    := Targ.Color;
  Stage.Targ.Name     := Targ.Name;

  return gId;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargExNpc(byref StageElem, byref Stage, LocGroup, byref QCrData, grId := 0)

  InitQCrTargExNpc(Stage.Targ);
  InitQCrTargLoc(Stage.Loc);

  var Targ      := StageElem.GetProp(QSTPROP_TARG);
  var Flags     := StageElem.GetProp(QAPROP_FLAGS);
  var Except    := array;
  var gId       := 1;
  var ExLocs    := array;

  if(Flags & QSTF_TARG_GROUP)
    if(grId)
      gId := grId;
    elseif(Targ.size())
      gId := RandomInt(Targ.size())+1;
    endif
    
    if(Targ.size())
      Targ := Targ[gId];
    else
      Targ := 0;
    endif
  endif

  if(!Targ.Template and !Targ.Script)
    if(QCrData.Source.isa(POLCLASS_NPC))
      Stage.Targ.Serial := QCrData.Source.serial;
      Stage.Loc.x       := QCrData.Source.x;
      Stage.Loc.y       := QCrData.Source.y;
      Stage.Loc.z       := QCrData.Source.z;
      Stage.Loc.realm   := QCrData.Source.realm;
      return gId;
    else
      var Res := error;
      Res.errortext := "Brak zrodlowego Npc w probie stworzenia Etapu Questa (QS Bez Temp&Sc) ID: "+Stage.Id+", Quest ID: "+QCrData.Id;
      return Res;
    endif
  endif

  GetQuestExLocsInCities(ExLocs, QCrData.CrFlags);

  if(QCrData.Source.isa(POLCLASS_NPC))
    Except.append(QCrData.Source.serial);
  endif

  foreach qStage in (QCrData.Stages)
    if(QCrData.CurStage == _qStage_iter)
      break;
    endif
    if(qStage.SubType == QSTYPE_EXNPC)
      Except.append(qStage.Targ.Serial);
    elseif(qStage.SubType == QSTYPE_FROMNPC)
      Except.append(qStage.Targ.NpcSerial);
    endif
  endforeach

  var Npc := SeekNpcForQuest(LocGroup, Targ, ExLocs, Except);

  if(!Npc)
    var Res := error;
    Res.errortext := Npc.errortext + " Etap Questa: "+ClStr(QCrData.QuestGroup)+":"+Stage.Id+" !";
    return Res;
  endif

  Stage.Targ.serial := Npc.serial;
  Stage.Loc.x       := Npc.x;
  Stage.Loc.y       := Npc.y;
  Stage.Loc.z       := Npc.z;
  Stage.Loc.realm   := Npc.realm;

  return gId;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargItem(byref StageElem, byref Stage, byref LocGroup, grId := 0)

  InitQCrTargItem(Stage.Targ);

  var Flags    := StageElem.GetProp(QAPROP_FLAGS);
  var Res      := GetQuestStageDefLoc(Stage, LocGroup);
  var Targ     := StageElem.GetProp(QSTPROP_TARG);
  var gId      := 1;

  if(!Res)
    return Res;
  endif

  if(Flags & QSTF_TARG_GROUP)
    if(grId)
      gId := grId;
    else
      gId := RandomInt(Targ.size())+1;
    endif
    Targ := Targ[gId];
  endif

  if(!Targ)
    Res := error;
    Res.errortext := "Brak celu w Etapie Questa ID: "+Stage.Id+"!";
    return Res;
  endif

  if(Targ.ToObjType)
    Targ.ObjType := Targ.ObjType + RandomInt(1 + Targ.ToObjType - Targ.ObjType);
  endif

  Stage.Targ.ObjType := Targ.ObjType;
  Stage.Targ.Color   := Targ.Color;
  Stage.Targ.Name    := Targ.Name;

  return gId;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargPlayer(byref Stage, byref QCrData)

  InitQCrTargPlayer(Stage.Targ);
  InitQCrTargLoc(Stage.Loc);

  var Players := array;
  var Accs    := ListAccounts();
  var Player, i, trPower, plPower;
  var pwMin, pwMax;

  if(QCrData.Player)
    plPower := GetQuestPlayerPower(QCrData.Player, QCrData.Type);
    pwMin   := (6 - QCrData.Level) * -1;
    pwMax   := QCrData.Level + 1;
  else
    plPower := -1;
  endif
  
  var Except   := array;

  foreach qStage in (QCrData.Stages)
    if(QCrData.CurStage == _qStage_iter)
      break;
    endif
    if(qStage.SubType == QSTYPE_PLAYER)
      Except.append(qStage.Targ.Serial);
    endif
  endforeach

  foreach Acc in Accs
    Acc := FindAccount(Acc);
    for(i:=1;i<=5;i:=i+1)
      Player := Acc.GetCharacter(i);
      if(Player and Player != QCrData.Player and !Player.cmdlevel)
        if(ReadGameClock() - CInt(GetObjProperty(Player, PROP_LASTLOG)) < QUEST_MAX_PL_LLOGTIME)
          if(plPower == -1)
            Players.append(Player);
          elseif(!(Player.serial in Except) and !IsFriend(QCrData.Player, Player))
            trPower := GetQuestPlayerPower(Player, QCrData.Type);
            if(trPower - plPower >= pwMin and trPower - plPower <= pwMax)
              Players.append(Player);
            endif
          endif
        endif
      endif
    endfor
  endforeach

  Player := 0;

  if(Players.size())
    Player := Players[RandomInt(Players.size())+1];
  endif

  if(!Player)
    var Err := error;
    Err.errortext := "Nie znaleziono Gracza spelniajacego wymagania w Etapie Questa ID: "+Stage.Id+", Targ: "+CInt(QCrData.Player.serial);
    return Err;
  endif

  Stage.Targ.Serial := Player.serial;
  Stage.Loc.x       := Player.x;
  Stage.Loc.y       := Player.y;
  Stage.Loc.z       := Player.z;
  Stage.Loc.realm   := Player.realm;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetQuestPlayerPower(Player, Type)

  var Power := 0;

  case(Type)
    QTYPE_KILL:   if(IsBard(Player))
                    Power := GetBaseDexterity(Player);
                  elseif(IsCharClass(Player, CLASS_MAGE))
                    Power := GetBaseIntelligence(Player);
                  elseif(IsCharClass(Player, CLASS_ARCHER))
                    Power := GetBaseDexterity(Player);
                  else
                    Power := GetBaseStrength(Player);
                  endif

    QTYPE_STEAL:  Power := GetBaseDexterity(Player);
  endcase
  
  return Power;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargLoc(byref StageElem, byref Stage, byref LocGroup)

  var Targ  := StageElem.GetProp(QSTPROP_TARG);
  var Flags := StageElem.GetProp(QAPROP_FLAGS);

  if(Flags & QSTF_TARG_GROUP)
    Targ := Targ[RandomInt(Targ.size())+1];
  endif

  if(!Targ)
    var Res := GetQuestStageDefLoc(Stage, LocGroup);
    if(!Res)
      return Res;
    endif
  else
    InitQCrTargLoc(Stage.Loc);
    Stage.Loc.x     := Targ.x;
    Stage.Loc.y     := Targ.y;
    Stage.Loc.z     := Targ.z;
    Stage.Loc.realm := Targ.realm;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargFromNpc(byref StageElem, byref Stage, byref LocGroup, byref QCrData)

  var Res      := GenQuestCrTargExNpc(StageElem, Stage, LocGroup, QCrData);
  if(!Res)
    return Res;
  endif

  var Flags    := StageElem.GetProp(QAPROP_FLAGS);
  var Targ     := StageElem.GetProp(QSTPROP_TARG);
  var NpcSer   := Stage.Targ.Serial;

  InitQCrTargFromNpc(Stage.Targ);

  if(Flags & QSTF_TARG_GROUP)
    Targ := Targ[Res];
  endif

  if(!Targ)
    Res := error;
    Res.errortext := "Brak celu w Etapie Questa ID: "+Stage.Id+"!";
    return Res;
  endif

  Stage.Targ.ObjType   := Targ.ObjType;
  Stage.Targ.Color     := Targ.Color;
  Stage.Targ.Name      := Targ.Name;
  Stage.Targ.NpcSerial := NpcSer;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargLootNpc(byref StageElem, byref Stage, byref LocGroup)

  var Res      := GenQuestCrTargNpc(StageElem, Stage, LocGroup);
  if(!Res)
    return Res;
  endif

  var Flags    := StageElem.GetProp(QAPROP_FLAGS);
  var qTarg    := StageElem.GetProp(QSTPROP_TARG);

  InitQCrTargLootNpc(Stage.Targ);

  if(Flags & QSTF_TARG_GROUP)
    qTarg := qTarg[Res];
  endif

  if(!qTarg)
    Res := error;
    Res.errortext := "Brak celu w Etapie Questa ID: "+Stage.Id+"!";
    return Res;
  endif

  Stage.Targ.ObjType   := qTarg.ObjType;
  Stage.Targ.Color     := qTarg.Color;
  Stage.Targ.Name      := qTarg.Name;
  Stage.Targ.Template  := qTarg.Template;
  Stage.Targ.NpcName   := qTarg.NpcName;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargLockTake(byref StageElem, byref Stage, byref LocGroup)

  var Flags    := StageElem.GetProp(QAPROP_FLAGS);
  var Res      := GetQuestStageDefLoc(Stage, LocGroup);
  var Targ     := StageElem.GetProp(QSTPROP_TARG);

  if(!Res)
    return Res;
  endif

  InitQCrTargLockTake(Stage.Targ);

  if(Flags & QSTF_TARG_GROUP)
    Targ := Targ[Randomint(Targ.size())+1];
  endif

  if(!Targ)
    Res := error;
    Res.errortext := "Brak celu w Etapie Questa ID: "+Stage.Id+"!";
    return Res;
  endif

  Stage.Targ.ObjType     := Targ.ObjType;
  Stage.Targ.ContObjType := Targ.ContObjType;
  Stage.Targ.Color       := Targ.Color;
  Stage.Targ.Name        := Targ.Name;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargSkill(byref StageElem, byref Stage, byref QCrData)

  InitQCrTargSkill(Stage.Targ);

  var Flags := StageElem.GetProp(QAPROP_FLAGS); 
  var Targ  := StageElem.GetProp(QSTPROP_TARG);
  var Res;

  if(Flags & QSTF_TARG_GROUP)
    Targ := Targ[RandomInt(Targ.size())+1];
  endif

  if(QCrData.Player and Targ.SkVal)
    var Skill := GetQuestSkIDFromSubType(Stage.SubType);
    if(Skill != error)
      Skill := GetEffectiveSkill(QCrData.Player, Skill);

      if(Targ.SkVal > Skill or !Skill)
        Res := error;
        Res.errortext := "Postac "+QCrData.Player.name+"/"+QCrData.Player.serial+" umie za malo na Etap Questa ID: "+Stage.Id;
        return Res;
      endif
    endif
  endif

  if(Targ.ToObjType)
    Targ.ObjType := Targ.ObjType + RandomInt(1 + Targ.ToObjType - Targ.ObjType);
  endif

  if(!Targ.ObjType)
    Res := error;
    Res.errortext := "Nie udalo sie okreslic celu ["+Targ+"] w Etapie Questa ID: "+Stage.Id;
    return Res;
  endif

  Stage.Targ.ObjType    := Targ.ObjType;
  Stage.Targ.SkVal      := Targ.SkVal;
  Stage.Targ.MatObjType := Targ.MatObjType;
  Stage.Targ.Exp        := CInt(Targ.ExpChance > RandomInt(100)+5);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargMine(byref StageElem, byref Stage, byref LocGroup, byref QCrData)

  InitQCrTargMine(Stage.Targ);

  var Res := GetQuestStageDefLoc(Stage, LocGroup);
  if(!Res)
    return Res;
  endif

  var Flags := StageElem.GetProp(QAPROP_FLAGS); 
  var Targ  := StageElem.GetProp(QSTPROP_TARG);

  if(Flags & QSTF_TARG_GROUP)
    Targ := Targ[RandomInt(Targ.size())+1];
  endif

  if(QCrData.Player and Targ.SkVal)
    var Skill := GetQuestSkIDFromSubType(Stage.SubType);
    if(Skill != error)
      Skill := GetEffectiveSkill(QCrData.Player, Skill);
      if(Targ.SkVal >= Skill or !Skill)
        Res := error;
        Res.errortext := "Postac "+QCrData.Player.name+"/"+QCrData.Player.serial+" umie za malo na Etap Questa ID: "+Stage.Id;
        return Res;
      endif
    endif
  endif

  if(Targ.ToObjType)
    Targ.ObjType := Targ.ObjType + RandomInt(1 + Targ.ToObjType - Targ.ObjType);
  endif

  if(!Targ)
    Res := error;
    Res.errortext := "Nie udalo sie okreslic celu ["+Targ+"] w Etapie Questa ID: "+Stage.Id;
    return Res;
  endif

  Stage.Targ.ObjType   := Targ.ObjType;
  Stage.Targ.SkVal     := Targ.SkVal;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestCrTargHouse(byref Stage, byref LocGroup, byref QCrData)

  InitQCrTargHouse(Stage.Targ);

  var x, y, LocRange;
  var Houses   := array;
  var Res      := error;
  var Tries    := 0;
  var Realm    := 0;
  var Except   := array;

  foreach qStage in (QCrData.Stages)
    if(QCrData.CurStage == _qStage_iter)
      break;
    endif
    if(qStage.SubType == QSTYPE_HOUSE)
      Except.append(qStage.Targ.Serial);
    endif
  endforeach
  
  while(!Houses.size() and Tries < QUEST_MAX_LOCTRIES)

    LocRange := GenQuestLocFromGroup(LocGroup);
    if(!LocRange)
      Res.errortext := "Nie udalo sie okreslic zasiegu lokacji w Etapie Questa ID: "+Stage.Id+" !";
      return Res;
    endif

    Realm := GetRealmById(LocRange[5]);
    if(!Realm)
      Res.errortext := "Bledny ID krainy w lokacji, RlmID: ["+LocRange[5]+"] - Etap Questa ID: "+Stage.Id+" !";
      return Res;
    elseif(Realm == REALM_MALAS or Realm == REALM_TOKUNO)
      continue;
    endif

    for(x:=LocRange[1];x<LocRange[3];x:=x+64)
      for(y:=LocRange[2];y<LocRange[4];y:=y+64)
        foreach House in ListMultisInBox(x, y, -127, x+64, y+64, 127, Realm)
          if(House.isa(POLCLASS_MULTI))
            if(QCrData.Player)
              if(!IsHouseCoOwner(QCrData.Player, House) and !IsHouseOwner(QCrData.Player, House) and !IsHouseFriend(QCrData.Player, House))
                if(!(House.serial in Except))
                  Houses.append(House);
                endif
              endif
            elseif(!(House.serial in Except))
              Houses.append(House);
            endif
          endif
        endforeach
      endfor
    endfor

  endwhile

  if(!Houses.size())
    Res.errortext := "Nie udalo sie znalezc Domu w LocRange ["+LocRange+"] w Etapie Questa ID: "+Stage.Id;
    return Res;
  endif

  var House := Houses[RandomInt(Houses.size())+1];

  if(!House)
    Res.errortext := "Nie udalo sie okreslic Domu ["+House+"] w Etapie Questa ID: "+Stage.Id+" !";
    return Res;
  endif

  Stage.Targ.Serial := House.serial;
  Stage.Loc.x       := House.x;
  Stage.Loc.y       := House.y;
  Stage.Loc.z       := House.z;
  Stage.Loc.realm   := House.realm;
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenQuestStageLoc(byref LocRange, byref Group)

  var Flags := Group.GetProp(QAPROP_FLAGS);
  
  var Loc;
  var Tries    := 0;
  var Realm    := GetRealmById(LocRange[5]);

  if(!Realm or Realm == REALM_MALAS or Realm == REALM_TOKUNO)
    return 0;
  endif

  InitQCrTargLoc(Loc);
  Loc.realm := Realm;
 
  while((!Loc.x or !Loc.y) and Tries < QUEST_MAX_LOCTRIES)
    Loc.x := LocRange[1] + RandomInt(LocRange[3] - LocRange[1]);
    Loc.y := LocRange[2] + RandomInt(LocRange[4] - LocRange[2]);
    
    if(Flags & QLF_NOCHECK_Z)
      Loc.z := GetMapInfo(Loc.x, Loc.y, Loc.realm);
    elseif(Flags & QLF_WORLD_Z)
      Loc.z := GetStand(Loc.x, Loc.y, Loc.realm);
    else
      Loc.z := GetStand(Loc.x, Loc.y, Loc.realm, GSF_GETMAPZ);
    endif

    if(Loc.z.z == error)
      Loc.x := 0;
      Loc.y := 0;
    elseif(!(Flags & QLF_ACP_MULTI))
      if(Loc.z.multi)
        Loc.x := 0;
        Loc.y := 0;
      endif
    endif

    Loc.z := Loc.z.z;
    Tries := Tries + 1;
  endwhile

  if(Tries <= QUEST_MAX_LOCTRIES and Loc.x and Loc.y)
    return Loc;
  else
    return 0;
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Creation Target Initialization
//-----------------------------------------------------------------------------
//=============================================================================

function InitCrQuestData(byref QData)

  QData := struct;

  QData.+Stages   := array;
  QData.+CrTime   := 0;
  QData.+CrGTime  := 0;
  QData.+Level    := 0;
  QData.+Id       := 0;
  QData.+QuestGroup := error;
  QData.+Type     := 0;
  QData.+SubType  := 0;
  QData.+Source   := 0;
  QData.+Player   := 0;
  QData.+MaxTime  := 0;
  QData.+CurStage := 0;
  QData.+Flags    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitCrQuestStageData(byref QStageData)

  QStageData := struct;

  QStageData.+Id      := 0;
  QStageData.+Type    := 0;
  QStageData.+SubType := 0;
  QStageData.+Targ    := 0;
  QStageData.+Guards  := 0;
  QStageData.+Loc     := 0;
  QStageData.+Flags   := 0;
  QStageData.+EndTime := 0;

endfunction

//-----------------------------------------------------------------------------

function InitCrQuestGuard(byref GuardElem)

  GuardElem := struct;

  GuardElem.+Template := 0;
  GuardElem.+Serial   := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargItem(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+Color     := 0;
  Targ.+Name      := 0;
  Targ.+Serial    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargNpc(byref Targ)

  Targ := struct;

  Targ.+Template  := 0;
  Targ.+Color     := 0;
  Targ.+Name      := 0;
  Targ.+Serial    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargExNpc(byref Targ)

  Targ := struct;

  Targ.+Serial    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargFromNpc(byref Targ)

  Targ := struct;

  Targ.+ObjType    := 0;
  Targ.+Color      := 0;
  Targ.+Name       := 0;
  Targ.+ItemSerial := 0;
  Targ.+NpcSerial  := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargLootNpc(byref Targ)

  Targ := struct;

  Targ.+ObjType    := 0;
  Targ.+Color      := 0;
  Targ.+Name       := 0;
  Targ.+Template   := 0;
  Targ.+NpcName    := 0;
  Targ.+ItemSerial := 0;
  Targ.+NpcSerial  := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargSkill(byref Targ)

  Targ := struct;

  Targ.+ObjType    := 0;
  Targ.+MatObjType := 0;
  Targ.+SkVal      := 0;
  Targ.+Exp        := 0;
  Targ.+Serial     := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargMine(byref Targ)

  Targ := struct;

  Targ.+ObjType   := 0;
  Targ.+SkVal     := 0;
  Targ.+Serial    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargPlayer(byref Targ)

  Targ := struct;

  Targ.+Serial    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargHouse(byref Targ)

  Targ := struct;

  Targ.+Serial := 0;

endfunction

//-----------------------------------------------------------------------------

function InitQCrTargLockTake(byref Targ)

  Targ := struct;

  Targ.+ContObjType := 0;
  Targ.+ObjType     := 0;
  Targ.+Color       := 0;
  Targ.+Name        := 0;
  Targ.+Serial      := 0;
  Targ.+ItemSerial  := 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Quest Creation
//-----------------------------------------------------------------------------
//=============================================================================

function AssignCrQuestData(byref Quest, byref QCrData)

  QCrData.Stages  := array;

  foreach Stage in (Quest.GetProp(QPROP_STAGES))
    QCrData.Stages[QCrData.Stages.size()+1]  := 0;
    InitCrQuestStageData(QCrData.Stages[QCrData.Stages.size()]);
    QCrData.Stages[QCrData.Stages.size()].Id := Stage;
  endforeach

  QCrData.CrTime  := PolCore().systime;
  QCrData.CrGTime := ReadGameClock();
  QCrData.Level   := Quest.GetProp(QPROP_LEVEL);
  QCrData.Type    := Quest.GetProp(QPROP_TYPE);
  QCrData.SubType := Quest.GetProp(QPROP_SUBTYPE);
  QCrData.MaxTime := Quest.GetProp(QPROP_MAXTIME);
  QCrData.Flags   := Quest.GetProp(QAPROP_FLAGS);

endfunction

//-----------------------------------------------------------------------------

function AssignCrQuestStageData(byref StageElem, byref Stage)

  Stage.Type    := StageElem.GetProp(QPROP_TYPE);
  Stage.SubType := StageElem.GetProp(QPROP_SUBTYPE);
  Stage.Targ    := 0;
  Stage.Guards  := 0;
  Stage.Loc     := 0;
  Stage.Flags   := StageElem.GetProp(QAPROP_FLAGS);
  Stage.EndTime := 0;

endfunction

//-----------------------------------------------------------------------------
