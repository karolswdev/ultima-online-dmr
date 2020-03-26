//-----------------------------------------------------------------------------

include ":miniquests:crmquest";

//-----------------------------------------------------------------------------

function CreateNpcMiniQuest(Npc)

  var MQNpcData := GetNpcMiniQuestData(Npc);
  var NpcGuild  := GetNpcGuild(Npc);
  var City      := FindCity(Npc);
  var CityAl    := GetCityAlignment(City);
  var GenData;

  InitMQuestGenData(GenData);

  GenData.Source  := Npc;
  GenData.ExCity  := 0;
  GenData.Flags   := QGLOCF_NEUTRAL;
  GenData.ExQID   := MQNpcData.QuestID;
  GenData.ExCity  := City;

  if(NpcGuild)
    case(NpcGuild.GetProp(GDPROP_STATUS))
      NGDSTATUS_ORDER:     SetBitOn(GenData.Flags, QGLOCF_GOOD);
      NGDSTATUS_CHAOS:     SetBitOn(GenData.Flags, QGLOCF_MURDER);
      NGDSTATUS_NEUTRAL:   SetBitOn(GenData.Flags, QGLOCF_NEUTRAL);
    endcase

  elseif(City)

    case(CityAl)
      CITYAL_GOOD:   SetBitOn(GenData.Flags, QGLOCF_GOOD);
      CITYAL_EVIL:   SetBitOn(GenData.Flags, QGLOCF_MURDER);
    endcase
  endif

  var QuestID := GenerateMQuest(GenData);
  var MQuest := 0;

  if(!QuestID)
    LogError("CreateNpcMiniQuest: ["+Npc.serial+"] "+QuestID.errortext, LOGF_WORLD);
    MQNpcData.Delay := ReadGameClock() + 600;
  else
    MQNpcData.QuestData := CreateMQuest(QuestID, GenData);

    if(MQNpcData.QuestData)
      MQNpcData.QuestID := QuestID;

      if(!MQNpcData.Flags)
        MQNpcData.+Flags := 0;
      endif

      SetBitOn(MQNpcData.Flags, MQNF_QUEST_CREATED);
      var MQuestData := GetMQuestData();
      MQuest := MQuestData.FindElement(MQUEST_TITLE+MQNpcData.QuestID);
    else
      LogError("CreateNpcMiniQuest: ["+Npc.serial+"] "+MQNpcData.QuestData.errortext, LOGF_WORLD);
      MQNpcData.Delay     := ReadGameClock() + 600;
      MQNpcData.QuestData := 0;
    endif

  endif
  
  SetObjProperty(Npc, MQPROP_DATA, MQNpcData);

  return MQuest;

endfunction

//-----------------------------------------------------------------------------

function CrMQuestItem(Npc, byref QData)

  if(!(QData.Flags & MQNF_QTARG_CREATED))
    var Item := CreateItemAtTempLoc(UOBJ_MQUESTITEM);
    if(!Item)
      LogError("CrMQuestItem: ["+Npc.serial+"] Nie udalo sie stworzyc tymczasowego przedmiotu MQuesta ID: ["+QData.QuestID+"]!");
      return 0;
    else
      var ItemCfg  := ReadConfigFile(ITEMS_CFG);
      var ItemName := QData.Targ.ItemName;
      var Graphic  := CInt(ItemCfg[QData.Targ.ObjType].Graphic);

      if(!ItemName)
        ItemName := GetDescForItem(QData.Targ.ObjType);
      endif

      if(!Graphic)
        Graphic := CInt(QData.Targ.ObjType);
      endif

      Item.graphic   := Graphic;
      SetName(Item, ItemName);
      Item.invisible := 0;
      QData.Targ.ItemSer := Item.serial;
      SetBitOn(QData.Flags, MQNF_QTARG_CREATED);
      return Item;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CreateMQuestReward(Npc, who)

  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  if(NpcData)
    var PlayerData := GetObjProperty(who, PROP_NGDDATA);
    if(PlayerData.Guilds.exists(NpcData.GuildId))
      if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_MEMBER)

        var NpcGuild := GetNpcGuild(Npc);
        GainGuildExp(who, PlayerData, NpcData.GuildId, NpcGuild, NGD_MQUEST_REPGAIN);
        SetObjProperty(who, PROP_NGDDATA, PlayerData);
      endif
    endif
  endif

  var rPack  := CreateItemInPack(who, UOBJ_BACKPACK);

  var lGroup := 31;

  if(RandomInt(100) < 70)
    lGroup := 14;
  elseif(RandomInt(100) < 50)
    lGroup := 18;
  elseif(RandomInt(100) < 40)
    lGroup := 17;
  elseif(RandomInt(100) < 4)
    lGroup := 21;
  endif

  CreateLoot(rPack, lGroup);

endfunction

//-----------------------------------------------------------------------------

function FailMQuest(Npc, who)

  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  if(NpcData)
    var PlayerData := GetObjProperty(who, PROP_NGDDATA);
    if(PlayerData.Guilds.exists(NpcData.GuildId))
      if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_MEMBER)
        var NpcGuild := GetNpcGuild(Npc);
        LoseGuildExp(who, PlayerData, NpcData.GuildId, NpcGuild, NGD_MQUEST_REPGAIN);
        SetObjProperty(who, PROP_NGDDATA, PlayerData);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SayMQNpcText(Npc, who, byref QData, byref MDQuest, PropName)

  var Text := MDQuest.GetProp(PropName);

  if(!Text)
    var NpcData := GetNpcMiniQuestData(Npc);
    LogError("SayMQNpcText: ["+Npc.serial+"] nie znaleziono tekstu ["+PropName+"] w Quescie ID ["+NpcData.QuestID+"]: "+MDQuest);
    NpcSpeechJustSay(Npc, who, "Aj.. zapomnial"+ggm(Npc,8)+" co to mial"+ggm(Npc,8)+" powiedziec...");
  else
    ConvertMQText(Text, QData, who);
    foreach Line in Text
      NpcSpeechJustSay(Npc, who, Line);
      sleepms(len(Line) * 70);
      if(Distance(Npc, who) > 2) 
        NpcSpeechJustSay(Npc, who, "*rozglada sie*");
        break;
      endif
    endforeach
    sleep(1);
  endif

endfunction

//-----------------------------------------------------------------------------
