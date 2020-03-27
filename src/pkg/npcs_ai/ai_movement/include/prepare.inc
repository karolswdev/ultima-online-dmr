//-----------------------------------------------------------------------------

include "proc_event";
include "process";
include "const";
include "moving";

include "anchor";
include "path";
include "rounding";
include "wander";

include ":doors:common";

//-----------------------------------------------------------------------------

var _AI_Movement;

//-----------------------------------------------------------------------------

function AISys_Movement_Init()

  SetBitOn(_AI_Data.sysFlags, AI_SYS_MOVEMENT);

  if(!AISys_Movement_Prepare())
    return 0;
  endif

  // inicjuj chodzenie, jesli takowe NPC ma zdefiniowane
  AISys_Movement_InitRound();
  AISys_Movement_InitAnchor();

  // wlacz anchora npecowi, ale tylko gdy nie mamy wlaczonego chodzenia.
  if(!AISys_Movement_IsRoundEnabled())
    AISys_Movement_EnableAnchor();
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_Prepare()

  _AI_Movement := struct;

  _AI_Movement.+clockWander  := ReadGameClock();
  _AI_Movement.+wanderTime   := AI_DEF_WANDER_WAIT;
  _AI_Movement.+Round        := 0;
  _AI_Movement.+waitTime     := 1;

  _AI_Movement.+moveType     := 0;
  _AI_Movement.+moveTarg     := 0;
  _AI_Movement.+curPath      := error;
  _AI_Movement.+curPathIndex := 0;
  _AI_Movement.+errorCount   := 0;
  _AI_Movement.+checkCount   := 0;
  _AI_Movement.+distCount    := 0;
  _AI_Movement.+frozenCount  := 0;
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
