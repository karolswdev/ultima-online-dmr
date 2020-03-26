//-----------------------------------------------------------------------------

include ":npcguilds:const";

//-----------------------------------------------------------------------------

function IsGuildNpc(Npc)

  var Data := GetObjProperty(Npc, PROP_NGDDATA);

  if(!Data or !Data.GuildId)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetNpcGuild(Npc)

  var NpcData := GetObjProperty(Npc, PROP_NGDDATA);

  if(!NpcData or !NpcData.GuildId)
    return 0;
  endif

  var GuildData := GetNGdData();
  var NpcGuild  := GuildData.FindElement(NGUILD_TITLE + NpcData.GuildId);

  if(!NpcGuild)
    LogError("InitNgdData: Nie znaleziono Gildii ID: "+NpcData.GuildId+" zapisanej na NPC: "+Npc.serial);
    return 0;
  endif

  return NpcGuild;

endfunction

//-----------------------------------------------------------------------------
