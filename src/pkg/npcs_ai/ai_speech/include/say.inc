//-----------------------------------------------------------------------------

function AISys_Speech_Say(Source, SpID, Flags := 0)

  var Cfg  := ReadConfigFile(AI_SPEECH_SAY_CFG);
  var Elem := Cfg[SpID];

  if(!Elem)
    LogError("[AI] Speech Npc Say ID ["+SpID+"]: nie istnieje w cfg!");
    return 0;
  endif

  var Texts  := GetConfigStringArray(Elem, "Text");

  if(!Texts.size())
    LogError("[AI] Speech Npc Say ID ["+SpID+"]: brak zdefiniowanych tekstow!");
    return 0;
  endif

  var Text := Texts[RandomInt(Texts.size())+1];

  SpeechFormatText(Self(), Source, Text);

  Text := SplitWords(Text, "|");

  TurnToward(Source);

  foreach Line in Text
    if(Flags & AI_SPF_SAY_PRIVATE)
      PrintTextAbovePrivate(Self(), Line, Source);
    else
      Say(Line);
    endif
    sleepms(50);
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_JustSay(Source, Text, Flags := 0)

  TurnToward(Source);

  if(Flags & AI_SPF_SAY_PRIVATE)
    PrintTextAbovePrivate(Self(), Text, Source);
  else
    Say(Text);
  endif

endfunction

//-----------------------------------------------------------------------------
