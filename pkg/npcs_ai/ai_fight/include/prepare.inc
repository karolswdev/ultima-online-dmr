//-----------------------------------------------------------------------------

include "proc_event";
include "process";
include "checks";

//-----------------------------------------------------------------------------

var _AI_Fight;

//-----------------------------------------------------------------------------

function AISys_Fight_Init()

  SetBitOn(_AI_Data.sysFlags, AI_SYS_FIGHT);

  AI_AddEvent(EVID_ENGAGED);
  AI_AddEvent(EVID_DAMAGED);
  AI_AddEvent(EVID_ENTEREDAREA, 15);

  return AISys_Fight_Prepare();

endfunction

//-----------------------------------------------------------------------------

function AISys_Fight_Prepare()

  _AI_Fight := struct;

  _AI_Fight.+opponent := 0;
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
