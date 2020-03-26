//-----------------------------------------------------------------------------

function process_combat_event()

  if((GetHp(me) * 100 / GetMaxHp(me)) < GetFleePoint())
    Flee();
  endif
  
endfunction

//-----------------------------------------------------------------------------

function post_combat()

  DisableCombatEvents();
  EnableMainEvents();
  sleepms(250);
  //look_around();
  
endfunction

//-----------------------------------------------------------------------------

function prepare_for_fight()

  DisableMainEvents();
  EnableCombatEvents();
  
endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  if(((GetHp(me) * 100) / GetMaxHp(me)) >= GetFleePoint())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
