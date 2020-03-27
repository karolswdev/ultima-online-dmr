//-----------------------------------------------------------------------------

function CheckNpcGuildSpeech(who, text)

  text := lower(text);

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;

  if(text["wita"])

    if(run_script(":ngd_npcs:hello", Data) > 0)
      return 1;
    endif

  elseif(text["awans"])

    if(run_script(":ngd_npcs:promotion", Data) > 0)
      return 1;
    endif

  elseif(CheckSpokenName(text) and text["zadanie"])
    
    if(run_script(":ngd_npcs:process_quest", Data) > 0)
      return 1;
    endif

  elseif(text["zapis"] and text["gildi"])

    start_script(":ngd_npcs:sign_in", Data);

  elseif(text["wypis"] and text["gildi"])

    start_script(":ngd_npcs:resign", Data);

  elseif(text["wybra"] and text["umiejetnosc"])

    start_script(":ngd_npcs:choose_skill", Data);

  endif


  return 0;

endfunction


//-----------------------------------------------------------------------------
