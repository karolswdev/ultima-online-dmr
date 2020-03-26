//-----------------------------------------------------------------------------

include ":ai_speech:const";
include "include/unicode";

//-----------------------------------------------------------------------------

function CheckWordsMatch(byref Text, byref Words)

  if(lower(typeof(Words)) == "array")

    var Cnt := 0;

    foreach Word in Words
      if(Text[Word])
        Cnt += 1;
      endif
    endforeach

    return (Cnt == Words.size());

  else

    if(Text[Words])
      return 1;
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SpeechFormatText(Npc, Source, byref Text)

  if(Text["%gn"])
    var Pos := find(Text,"%gn",0)+3;
    var Id  := CInt(Text[Pos, len(Text)]);
    if(!Id)
      Text["%gn"] := "";
    else
      Text["%gn"+Id] := ggm(Npc, Id);
    endif
  endif
  
  if(Text["%gs"])
    var Pos := find(Text,"%gs",0)+3;
    var Id  := CInt(Text[Pos, len(Text)]);
    if(!Id)
      Text["%gs"] := "";
    else
      Text["%gs"+Id] := ggm(Source, Id);
    endif
  endif

  if(Text["%ns"])
    Text["%ns"] := GetName(Source);
  endif
  
  if(Text["%nn"])
    Text["%nn"] := GetName(Npc);
  endif

endfunction

//-----------------------------------------------------------------------------

function NpcSpeechJustSay(Npc, Source, Text, Flags := 0)

  if(Flags & AI_SPF_SAY_PRIVATE)
    UniPrintTextAbovePrivate(Npc, Text, Source);
  else
    UniPrintTextAbove(Npc, Text);
  endif

endfunction

//-----------------------------------------------------------------------------

function NpcSpeechSay(Npc, Source, SpID, Flags := 0)

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

  SpeechFormatText(Npc, Source, Text);
  Text := SplitWords(Text, "|");

  foreach Line in Text
    if(Flags & AI_SPF_SAY_PRIVATE)
      PrintTextAbovePrivate(Npc, Line, Source);
    else
      PrintTextAbove(Npc, Line);
    endif
    sleepms(50);
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------
