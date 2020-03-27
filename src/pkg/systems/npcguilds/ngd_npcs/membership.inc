//-----------------------------------------------------------------------------

const CPM_NOCHECK_MEMBER     := 0x00000001;
const CPM_NOCHECK_LEVEL      := 0x00000002;

//-----------------------------------------------------------------------------

function CanNGnSpeakWith(Npc, who)

  var NpcGuild := GetNpcGuild(Npc);
  var Res := CheckPlayerGuildStatus(who, NpcGuild);

  if(!Res)
    NpcSpeechSay(Npc, who, "NgdHatred");
  elseif(Res == 2)
    NpcSpeechSay(Npc, who, "NgdNotWelcome");
  else
    return 1;
  endif

  return 0;

endfunction
          
//-----------------------------------------------------------------------------

function CheckPlayerMembership(Npc, who, Flags := 0)

  if(!IsGuildNpc(Npc))
    return 0;
  endif

  var PlayerData := GetObjProperty(who, PROP_NGDDATA);
  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  if(!PlayerData)
    NpcSpeechJustSay(Npc, who, "Nie wyglada na to bys nalezal"+ggm(who,4)+" do mojej Gildii.");
    return 0;
  elseif(!PlayerData.Guilds.exists(NpcData.GuildId))
    NpcSpeechJustSay(Npc, who, "Nie wyglada na to bys byl"+ggm(who,4)+" czlonkiem mojej Gildii.");
    return 0;
  endif

  if(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_MEMBER)
    if(!CanNGnSpeakWith(Npc, who))
      return 0;
    endif
  endif

  PlayerData.Guilds[NpcData.GuildId].LastVisit := ReadGameClock();
  SetObjProperty(who, PROP_NGDDATA, PlayerData);

  if(PlayerData.Guilds[NpcData.GuildId].Delay + NGD_PLAYER_DELAY > ReadGameClock())
    NpcSpeechJustSay(Npc, who, "Nie jestes czlonkiem mojej Gildii, ostatnio nie poszlo Ci zbyt dobrze.");
    return 0;
  elseif(!(Flags & CPM_NOCHECK_MEMBER))
    if(!(PlayerData.Guilds[NpcData.GuildId].Flags & NGPF_MEMBER))
      NpcSpeechJustSay(Npc, who, "Nie jestes czlonkiem mojej Gildii.");
      return 0;
    endif
  endif
  
  if(!(Flags & CPM_NOCHECK_LEVEL))
    if(PlayerData.Guilds[NpcData.GuildId].Level < NpcData.Level)
      NpcSpeechJustSay(Npc, who, "Jestes za krotko w naszej Gildii, przykro mi.");
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CountGuildsMembership(who)

  var Count := 0;
  var PlayerData := GetObjProperty(who, PROP_NGDDATA);

  foreach GuildPlData in (PlayerData.Guilds)
    if(GuildPlData.Flags & NGPF_MEMBER or GuildPlData.Flags & NGPF_TEST_QUEST)
      Count := Count + 1;
    endif
  endforeach

  return Count;

endfunction

//-----------------------------------------------------------------------------
