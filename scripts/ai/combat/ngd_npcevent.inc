//-----------------------------------------------------------------------------

include "include/attributes/main";
include "include/poison";

//-----------------------------------------------------------------------------

function process_combat_event()

  return 0;

endfunction

//-----------------------------------------------------------------------------

function prepare_for_fight()

  DisableMainEvents();
  EnableCombatEvents();

endfunction

//-----------------------------------------------------------------------------

function post_combat()

  DisableCombatEvents();
  EnableMainEvents();
  sleep(1);
  move_to_anchor();

endfunction

//-----------------------------------------------------------------------------

function process_flee_event()

  return 0;

endfunction

//-----------------------------------------------------------------------------
