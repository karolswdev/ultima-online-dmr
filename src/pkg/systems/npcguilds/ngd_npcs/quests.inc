//-----------------------------------------------------------------------------

include "include/items/containers";
include "include/move";

include ":quests:quest_ctrl";

include ":quests:crquest";
include ":npcguilds:guildsdata";
include ":npcguilds:guilds";

//-----------------------------------------------------------------------------

function TakeFinishedQuest(Npc, who)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  if(!PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket)
    return 0;
  endif

  var Ticket := FindItemBySerial(PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket);

  if(!Ticket)
    NpcSpeechJustSay(Npc, who, "Widze ze nie wykonal"+ggm(who,3)+" ostatniego zadania.. nie dobrze.");
    if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_TEST_QUEST)
      FailTestQuest(Npc, who);
      return 1;
    else

      var QuestData := GetQuestData(PlayerData.Guilds[NpcData.GuildId].LastQuest.QuestGroup);
      var QuestElem := QuestData.FindElement(QUEST_TITLE + PlayerData.Guilds[NpcData.GuildId].LastQuest.QId);
      if(QuestElem)
        FailGivenQuest(Npc, who, CInt(QuestElem.GetProp(QPROP_LEVEL)), CInt(QuestElem.GetProp(QPROP_STAGES).size()));
      endif

      return 1;
    endif
  endif
  
  var TicketData := GetObjProperty(Ticket, QGPROP_QCRINFO);

  if(GetTopCont(Ticket) != who.backpack)
    NpcSpeechJustSay(Npc, who, "Nie widze bys mial"+ggm(who,4)+" wskazowki zadania przy sobie.");
    return 1;
  endif

  if(PlayerData.Guilds[NpcData.GuildId].LastQuest.From != Npc.serial)
    NpcSpeechJustSay(Npc, who, "Nie dostal"+ggm(who,3)+" tego zadania odemnie.");
    return 1;
  endif

  if(!TicketData)
    LogError("TakeFinishedQuest: NPC: "+Npc.serial+" Bilet: "+Ticket.serial+" nie posiada QuestData!");
    NpcSpeechJustSay(Npc, who, "*hmm..*");
    return 1;
  endif

  if(TicketData.Player != who.serial)
    NpcSpeechJustSay(Npc, who, "Nie dawal"+ggm(Npc,8)+" tego zadania tobie! Won!");
    return 1;
  elseif(TicketData.Flags & QF_FAILED)
    if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_TEST_QUEST)
      FailTestQuest(Npc, who);
      NpcSpeechJustSay(Npc, who, "Widze ze jednak nie udalo Ci sie wykonac powierzonego zadania, zegnam wiec...");
    else
      FailGivenQuest(Npc, who, TicketData.Level, TicketData.Stages.size());
      NpcSpeechJustSay(Npc, who, "Ah.. przykro mi, ze nie udalo Ci sie wykonac ostatniego zadania.");
    endif
    return 1;
  elseif(TicketData.Flags & QF_COMPLETED)
    if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_TEST_QUEST)
      var StartGuildId := GetSimilarNpcGuild(who, NpcData.GuildId);
      
      if(StartGuildId)
        NpcSpeechJustSay(Npc, who, "Gratuluje! Udalo Ci sie zakonczyc zadanie, zostal"+ggm(who,3)+" przyjet"+ggm(who,6)+" do naszej Gildii!");
        NpcSpeechJustSay(Npc, who, "Widze, ze mial"+ggm(who,3)+" juz doczynienia z inna gildia o podobnym charakterze, przyda nam sie Twoje doswiadczenie!");
      elseif(PlayerData.Guilds[NpcData.GuildId].Exp > 0)
        NpcSpeechJustSay(Npc, who, "Gratuluje! Udalo Ci sie zakonczyc zadanie, zostal"+ggm(who,3)+" przyjet"+ggm(who,6)+" ponownie do naszej Gildii! Nie zawiedz nas!");
      else
        NpcSpeechJustSay(Npc, who, "Gratuluje! Udalo Ci sie zakonczyc zadanie, zostal"+ggm(who,3)+" przyjet"+ggm(who,6)+" do naszej Gildii!");
      endif
      CompleteTestQuest(Npc, who, StartGuildId);
    else
      CompleteGivenQuest(Npc, who, TicketData.Level, TicketData.Stages.size());
      NpcSpeechJustSay(Npc, who, "Ciesze sie, ze udalo Ci sie zakonczyc to zadanie.");
    endif
    SendQuestTicketEvent(Ticket, Npc, EVQID_FINISH, 0, who);
    return 1;
  else
    NpcSpeechJustSay(Npc, who, "Hm.. na co czekasz? Wykonaj szybko to zadanie...");
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function FailTestQuest(Npc, who)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  SetBitOff(PlayerData.Guilds[NpcData.GuildId].Flags, NGPF_TEST_QUEST);
  PlayerData.Guilds[NpcData.GuildId].Delay := ReadGameClock();
  PlayerData.Guilds[NpcData.GuildId].LastQuest.From   := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Time   := 0;

  var GuildData  := GetNGdData();
  var Guild := GuildData.FindElement(NGUILD_TITLE+NpcData.GuildId);
  LoseGuildExp(who, PlayerData, NpcData.GuildId, Guild, 2000);

  SetObjProperty(who, PROP_NGDDATA, PlayerData);

endfunction

//-----------------------------------------------------------------------------

function CompleteTestQuest(Npc, who, StartGuildId := error)

  var NpcGuild := GetNpcGuild(Npc);
  var StartGuild := 0;
  var Members := NpcGuild.GetProp(GDPROP_MEMBERS);

  if(StartGuildId)
    var Data := GetNGdData();
    StartGuild := Data.FindElement(NGUILD_TITLE+StartGuildId);
  endif

  Members.append(who.serial);
  NpcGuild.SetProp(GDPROP_MEMBERS, Members);

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData    := GetObjProperty(Npc, PROP_NGDDATA);

  if(StartGuild and PlayerData.Guilds.exists(StartGuildId)) 
    var bok := 0;
    PlayerData.Guilds[NpcData.GuildId].Exp := PlayerData.Guilds[StartGuildId].Exp;
    PlayerData.Guilds[NpcData.GuildId].Level := PlayerData.Guilds[StartGuildId].Level;
    if(PlayerData.MaxSkID != error)
      var SkIDs := NpcGuild.GetProp(GDPROP_SKIDS);
      if(PlayerData.MaxSkID in SkIDs)
        SetMaxSkillValue(who, PlayerData.MaxSkID, DEF_SK_MAX_VALUE + PlayerData.Guilds[NpcData.GuildId].Level*10, MSKT_NPCGUILD);
        SetBitOn(PlayerData.Guilds[NpcData.GuildId].Flags, NGPF_MAXSK_CHOOSEN);
        SetBitOff(PlayerData.Guilds[StartGuildId].Flags, NGPF_MAXSK_CHOOSEN);
        SendSysMessage(who, "Wybrana przez Ciebie umiejetnosc do doskonalenia z poprzedniej gildii zostala przypisana do biezacej gildii.", FONT_NORMAL, COLOR_WHITE);
        bok := 1;
      endif
    endif
    
    if(!bok)
      SendSysMessage(who, "Pamietaj by wybrac umiejetnosc do doskonalenia!", FONT_NORMAL, COLOR_WHITE);
    endif
  endif

  SetBitOff(PlayerData.Guilds[NpcData.GuildId].Flags, NGPF_TEST_QUEST);
  SetBitOn(PlayerData.Guilds[NpcData.GuildId].Flags, NGPF_MEMBER);
  PlayerData.Guilds[NpcData.GuildId].JoinTime         := ReadGameClock();
  PlayerData.Guilds[NpcData.GuildId].LastQuest.From   := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Time   := 0;
  GainGuildExp(who, PlayerData, NpcData.GuildId, NpcGuild, NGD_EXP_MODIFIER * 10);
  SetObjProperty(who, PROP_NGDDATA, PlayerData);

  var sTitle := GetPlayerNGDTitleByData(PlayerData);
  SetTitleSuffix(who, sTitle);

endfunction

//-----------------------------------------------------------------------------

function FailGivenQuest(Npc, who, QLvl, QStNum)

  var NpcGuild   := GetNpcGuild(Npc);
  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData    := GetObjProperty(Npc, PROP_NGDDATA);

  PlayerData.Guilds[NpcData.GuildId].LastQuest.From   := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket := 0;
  if(!LoseGuildExp(who, PlayerData, NpcData.GuildId, NpcGuild, (QLvl*2) * QStNum * NGD_EXP_MODIFIER))
    ResignPlayerFromGuild(who, NpcData.GuildId);
  else
    SetObjProperty(who, PROP_NGDDATA, PlayerData);
  endif

endfunction

//-----------------------------------------------------------------------------

function CompleteGivenQuest(Npc, who, QLvl, QStNum)

  var NpcGuild   := GetNpcGuild(Npc);
  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData    := GetObjProperty(Npc, PROP_NGDDATA);

  PlayerData.Guilds[NpcData.GuildId].LastQuest.From   := 0;
  PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket := 0;
  GainGuildExp(who, PlayerData, NpcData.GuildId, NpcGuild, (QLvl*2) * QStNum * NGD_EXP_MODIFIER);
  SetObjProperty(who, PROP_NGDDATA, PlayerData);

endfunction

//-----------------------------------------------------------------------------

function GiveQuest(Npc, who)

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData    := GetObjProperty(Npc, PROP_NGDDATA);

  var GdData := PlayerData.Guilds[NpcData.GuildId];

  if(!(GdData.Flags & NGPF_MEMBER))
    NpcSpeechJustSay(Npc, who, "Nie jestes czlonkiem mojej Gildii.");
    return 0;
  elseif(GdData.LastQuest.Ticket)
    NpcSpeechJustSay(Npc, who, "Dostal"+ggm(who,3)+" juz zadanie, na co czekasz?");
    return 0;
  elseif(GdData.LastQuest.Time + GetNgdQuestDelay(GdData.Level+1) > ReadGameClock())
    NpcSpeechJustSay(Npc, who, "Przyjdz pozniej, a znajde jakies zadanie dla Ciebie.");
    return 0;
  endif

  var Ticket := CreateQuestForPlayer(Npc, who);

  if(!Ticket)
    NpcSpeechJustSay(Npc, who, "Nie mam aktualnie dla Ciebie zadnego zadania.");
    PlayerData.Guilds[NpcData.GuildId].LastQuest.Time := ReadGameClock() - GetNgdQuestDelay(PlayerData.Guilds[NpcData.GuildId].Level+1) + 600;
  else
    var TicketData := GetObjProperty(Ticket, QGPROP_QCRINFO);
    NpcSpeechJustSay(Npc, who, "Prosze, instrukcje sa na tym liscie, przeczytaj je.");

    PlayerData.Guilds[NpcData.GuildId].LastQuest.Ticket := Ticket.serial;
    PlayerData.Guilds[NpcData.GuildId].LastQuest.QuestGroup := TicketData.QuestGroup;
    PlayerData.Guilds[NpcData.GuildId].LastQuest.QId    := TicketData.Id;
    PlayerData.Guilds[NpcData.GuildId].LastQuest.From   := Npc.serial;
    PlayerData.Guilds[NpcData.GuildId].LastQuest.Time   := ReadGameClock();
  endif
  
  SetObjProperty(who, PROP_NGDDATA, PlayerData);

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CreateQuestForPlayer(Npc, who)

  var NpcGuild   := GetNpcGuild(Npc);
  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData    := GetObjProperty(Npc, PROP_NGDDATA);

  var GenQuestData;
  InitQuestGenData(GenQuestData);

  GenQuestData.Source     := Npc;
  GenQuestData.Player     := who;
  GenQuestData.GdType     := NpcGuild.GetProp(GDPROP_TYPE);
  GenQuestData.QuestTypes := NpcGuild.GetProp(GDPROP_QTYPES);
  GenQuestData.Level      := PlayerData.Guilds[NpcData.GuildId].Level+1;
  GenQuestData.City       := FindCity(Npc);

  GenQuestData.QuestGroup := QGROUP_NPCGUILDS;

  if(GenQuestData.Level < 1)
    GenQuestData.Level := 1;
  elseif(GenQuestData.Level > 5)
    GenQuestData.Level := 5;
  endif

  case(NpcGuild.GetProp(GDPROP_STATUS))
    NGDSTATUS_ORDER:     SetBitOn(GenQuestData.CrFlags, QGLOCF_GOOD);
    NGDSTATUS_CHAOS:     SetBitOn(GenQuestData.CrFlags, QGLOCF_MURDER);
    NGDSTATUS_NEUTRAL:   SetBitOn(GenQuestData.CrFlags, QGLOCF_NEUTRAL);
                         SetBitOn(GenQuestData.CrFlags, QGLOCF_GOOD);
  endcase

  if(PlayerData.Guilds[NpcData.GuildId].LastQuest.QId)
    GenQuestData.Except.append(PlayerData.Guilds[NpcData.GuildId].LastQuest.QId);
  endif

  var GdFlags := NpcGuild.GetProp(GDAPROP_FLAGS);

  if(GdFlags & NGF_QGD_ONLY)
    SetBitOn(GenQuestData.CrFlags, QCRF_QGD_ONLY);
  endif

  foreach Type in GetConfigStringArray(GetNpcConfig(Npc), NGD_QTYPE)
    Type    := SplitWords(Type);
    Type[1] := CInt(Type[1]);
    Type[2] := CInt(Type[2]);
    GenQuestData.QuestTypes.append(Type);
  endforeach

  var Ticket := GenerateQuest(GenQuestData);

  if(!Ticket)
    if(Ticket.errortext)
      LogError("CreateQuestForPlayer: GenQuest NPC: "+Npc.serial+" Player: "+who.serial+", Err: "+Ticket.errortext, LOGF_WORLD);
    endif
    return 0;
  endif

  MoveToPack(Ticket, who);
  return Ticket;

endfunction

//-----------------------------------------------------------------------------
