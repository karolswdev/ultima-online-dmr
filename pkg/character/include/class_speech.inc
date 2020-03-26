//-----------------------------------------------------------------------------

function CheckCharClassSpeech(who, text)

  text := lower(text);

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(text["udoskonali"] and text["umiejetnosc"] and CheckSpokenName(text))

    TurnToward(who);
  
    var Skills := GetMerchantPerfectSkillArrayForClass(me, GetCharClass(who));

    if(!Skills.size())
      Say("Przykro mi, ale Ja nie potrafie niczego Cie nauczyc...");
      return 1;
    endif

    Say("Wybierz zatem umiejetnosc, jaka Cie interesuje.");

    if(run_script(":character:npc_maxskills", Data) > 0)
      return 1;
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
