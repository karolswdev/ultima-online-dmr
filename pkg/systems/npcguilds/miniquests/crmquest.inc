//-----------------------------------------------------------------------------

include "include/gump_utils";

include ":miniquests:miniquests";
include ":npcguilds:shared";
include ":quests:include/questnpc";
include ":quests:include/questloc";

//-----------------------------------------------------------------------------

function InitMQuestGenData(byref Data)

  Data := struct;
  Data.+Source  := 0;
  Data.+ExCity  := 0;
  Data.+Flags   := 0;
  Data.+ExQID   := 0;

endfunction

//-----------------------------------------------------------------------------

function InitMQuestCrData(byref Data)

  Data := struct;
  Data.+Type      := 0;
  Data.+Loc       := 0;
  Data.+Targ      := 0;
  Data.+Source    := 0;
  Data.+QuestID   := 0;
  Data.+StartTime := 0;
  Data.+Flags     := 0;

endfunction

//-----------------------------------------------------------------------------

function InitMQuestTargItem(byref Targ)

  if(!Targ)
    Targ := struct;
  endif

  Targ.+ItemSer  := 0;
  Targ.+ItemName := 0;
  Targ.+ObjType  := 0;
  Targ.+Color    := 0;

endfunction

//-----------------------------------------------------------------------------

function InitMQuestTargNpc(byref Targ)

  if(!Targ)
    Targ := struct;
  endif

  Targ.+NpcSer   := 0;

endfunction

//-----------------------------------------------------------------------------

function GenerateMQuestIDs(byref Data, byref QuestIDs)

      QuestIDs    := array;
  var Err         := error;
  var MQuestData  := GetMQuestData();
  var Keys        := GetDataKeys(MQuestData, MQUEST_TITLE);
  var SrcTemp     := lower(Data.Source.npctemplate);
  var MQuest, NpcTemp;

  foreach Key in Keys
    MQuest := MQuestData.FindElement(MQUEST_TITLE+Key);
    if(MQuest)
      NpcTemp := MQuest.GetProp(MQPROP_NPCTEMP);
      if(!NpcTemp or lower(NpcTemp) == SrcTemp)
        QuestIDs.append(CInt(Key));
      endif
    endif
  endforeach

  if(!QuestIDs.size())
    Err.errortext := "Nie znaleziono Quest IDow dla ["+Data.Source.npctemplate+"] ExCIty: ["+Data.ExCity+"] ExID: ["+Data.ExQID+"] Flags: ["+Data.Flags+"]";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenerateMQuest(byref Data)

  var QuestIDs;
  var Res := GenerateMQuestIDs(Data, QuestIDs);
  if(!Res)
    return Res;
  else
    return QuestIDs[RandomInt(QuestIDs.size())+1];
  endif

endfunction

//-----------------------------------------------------------------------------

function CreateMQuest(QuestID, byref GenData)

  var Err        := error;
  var MQuestData := GetMQuestData();
  var MQuest     := MQuestData.FindElement(MQUEST_TITLE+QuestID);
  var CrData;

  InitMQuestCrData(CrData);

  if(!MQuest)
    Err.errortext := "Nie znaleziono MQuesta ID: ["+QuestID+"]";
    return Err;
  endif

  CrData.Type    := MQuest.GetProp(MQPROP_TYPE);

  case(CrData.Type)
    MQTYPE_CARRYITEM:   
    MQTYPE_TAKEITEM:      InitMQuestTargItem(CrData.Targ);
                          InitMQuestTargNpc(CrData.Targ);

    MQTYPE_GOTONPC:       InitMQuestTargNpc(CrData.Targ);
    default:              Err.errortext := "Nie prawidlowy typ ["+CrData.Type+"] Questa w ID ["+QuestID+"] !";
                          return Err;
  endcase

  Err := GenMQuestTarget(CrData, GenData, MQuest);

  if(!Err)
    Err.errortext := Err.errortext + " MQID: ["+QuestID+"]";
    return Err;
  endif

  CrData.Source    := GenData.Source.serial;
  CrData.QuestID   := QuestID;
  CrData.StartTime := ReadGameClock();

  return CrData;
 
endfunction

//-----------------------------------------------------------------------------

function GenMQuestTarget(byref CrData, byref GenData, byref MQuest)

  var Err    := error;
  var MQTarg := MQuest.GetProp(MQPROP_TARG);

  case(CrData.Type)
    MQTYPE_CARRYITEM:   
    MQTYPE_TAKEITEM:      Err := GenMQuestTargNpc(CrData, GenData, MQTarg, MQuest);
                          if(Err == 1)
                            GenMQuestTargItem(CrData, MQTarg);
                          endif

    MQTYPE_GOTONPC:       Err := GenMQuestTargNpc(CrData, GenData, MQTarg, MQuest);

    default:              Err.errortext := "Nie prawidlowy typ ["+CrData.Type+"]";
  endcase

  return Err;

endfunction

//-----------------------------------------------------------------------------

function GenMQuestTargNpc(byref CrData, byref GenData, byref MQTarg, byref MQuest)

  var LocGroup  := GetQuestLocGroup(MQuest.GetProp(MQPROP_LOC));

  if(!LocGroup)
    return LocGroup;
  endif

  InitQCrTargLoc(CrData.Loc);

  var Except    := array;
  var ExLocs    := array;

  if(GenData.Source.isa(POLCLASS_NPC))
    Except.append(GenData.Source.serial);
  endif

  GetQuestExLocsInCities(ExLocs, GenData.Flags, GenData.ExCity);

  var Npc := SeekNpcForQuest(LocGroup, MQTarg, ExLocs, Except);

  if(!Npc)
    var Res := error;
    Res.errortext := Npc.errortext + " MiniQuest: "+CrData.QuestID;
    return Res;
  endif

  CrData.Targ.NpcSer := Npc.serial;
  CrData.Loc.x       := Npc.x;
  CrData.Loc.y       := Npc.y;
  CrData.Loc.z       := Npc.z;
  CrData.Loc.realm   := Npc.realm;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GenMQuestTargItem(byref CrData, byref MQTarg)

  CrData.Targ.ObjType   := MQTarg.ObjType;
  CrData.Targ.Color     := MQTarg.Color;
  CrData.Targ.ItemName  := MQTarg.Name;
  return 1;

endfunction

//-----------------------------------------------------------------------------
