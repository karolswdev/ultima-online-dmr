//-----------------------------------------------------------------------------

include ":miniquests:const";
include ":ngd_npcs:common";

//-----------------------------------------------------------------------------

function GetNpcMiniQuestData(Npc)

  var Data := GetObjProperty(Npc, MQPROP_DATA);

  if(!Data)
  
    var MQCfg := ReadConfigFile(MQUESTS_CFG);

    Data := struct;
    Data.+Delay        := ReadGameClock() + MQCfg[MQCFG_GLOBAL].NormDelay + RandomInt(MQCfg[MQCFG_GLOBAL].RandDelay);
    Data.+NextLookTime := 0;
    Data.+LastLooked   := array;
    Data.+QuestID      := 0;
    Data.+Flags        := 0;
    Data.+Player       := 0;
    Data.+QuestData    := 0;

    SetObjProperty(Npc, MQPROP_DATA, Data);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetMiniQuestByNpc(Npc)

  var Data := GetNpcMiniQuestData(Npc);

  if(Data.Flags & MQNF_QUEST_CREATED)
    var MQuestData := GetMQuestData();
    var Quest := MQuestData.FindElement(MQUEST_TITLE+Data.QuestID);

    if(!Quest)
      LogError("Npc ["+Npc.serial+"] posiadal nieistniejacy MQuestID ["+Data.QuestID+"]");
      ResetMQuestData(Npc);
    endif

    return Quest;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ResetMQuestData(Npc)

  var NpcData := GetNpcMiniQuestData(Npc);
  var Player  := FindPlayerBySerial(NpcData.Player);
  var MQCfg   := ReadConfigFile(MQUESTS_CFG);

  if(Player)
    EraseObjProperty(Player, MQPROP_DATA);
  endif

  NpcData.Delay      := ReadGameClock() + MQCfg[MQCFG_GLOBAL].NormDelay + RandomInt(MQCfg[MQCFG_GLOBAL].RandDelay);
  NpcData.QuestData  := 0;
  NpcData.Player     := 0;
  NpcData.LastLooked := array;
  NpcData.Flags      := 0;

  SetObjProperty(Npc, MQPROP_DATA, NpcData);

endfunction

//-----------------------------------------------------------------------------

function CanNpcHaveMiniQuest(Npc)

  var MQCfg := ReadConfigFile(MQUESTS_CFG);

  if(IsGuildNpc(Npc))
    if(!MQCfg[MQCFG_GLOBAL].NGdEnabled)
      return 0;
    endif
  elseif(!MQCfg[MQCFG_GLOBAL].AllEnabled)
    return 0;
  endif

  return GetNpcConfig(Npc).CityNpc;

endfunction

//-----------------------------------------------------------------------------
