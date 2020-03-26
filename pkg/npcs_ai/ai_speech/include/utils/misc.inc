//-----------------------------------------------------------------------------

include ":ai_speech:const";

//-----------------------------------------------------------------------------

function CanNpcSpeekWith(Npc, Source)

  if((Distance(Npc, Source) > AI_SPEECH_DEF_RANGE or !CheckLineOfSight(Npc, Source) or DistZ(Npc, Source) >= 20) or Source.hidden or Source.dead)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SpokenMobName(Npc, text)

  if(text[lower(Npc.name[1,find(Npc.name," ",1)-1])])
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMobWithSpokenNameNear(x, y, z, realm, text, range := AI_SPEECH_DEF_RANGE)

  foreach Mob in ListMobilesNearLocation(x, y, z, range, realm)

    if(SpokenMobName(Mob, text))
      return Mob;
    endif

  endforeach

  return error;

endfunction

//-----------------------------------------------------------------------------
