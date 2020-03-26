//-----------------------------------------------------------------------------

function AISys_Speech_AddTrigger(Word, ScriptName, Flags, HdlFlags := 0)

  var Trigger := struct;
  Trigger.+word     := Word;
  Trigger.+script   := ScriptName;
  Trigger.+flags    := Flags;
  Trigger.+hdlflags := HdlFlags;

  _AI_Speech.Triggers.append(Trigger);

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_Triggered(Source, normText)

  var lowerText := lower(normText);

  // sprawdzmy, czy gracz okresla sie do konkretnego npeca po imieniu

  var SpokenToMob := GetMobWithSpokenNameNear(Source.x, Source.y, Source.z, Source.realm, lowerText);
  var Triggers := array;

  if(SpokenToMob and SpokenToMob != Self())
    return Triggers;
  endif

  foreach Trigger in (_AI_Speech.Triggers)

    // wymagamy podania imienia ?

    if(IsBitOn(Trigger.flags, AI_SPT_SPOKE_NPC_NAME))
      if(!SpokenToMob)
        continue;
      endif
    endif

    // czy wielkosc znakow jest wazna ?
    
    if(IsBitOn(Trigger.flags, AI_SPT_CASE_SENSITIVE))
      if(CheckWordsMatch(normText, Trigger.word))
        Triggers.append(Trigger);
      endif
    else
      if(CheckWordsMatch(lowerText, Trigger.word))
        Triggers.append(Trigger);
      endif
    endif

  endforeach

  return Triggers;

endfunction

//-----------------------------------------------------------------------------
