//-----------------------------------------------------------------------------

function AISys_Movement_Process()

  // obchod dla npecow miastowych
  if(AISys_Movement_IsRoundEnabled())
    AISys_Movement_ProcessRound();
  endif
  
  // jak nie chodzi, to maly wander!
  if(!AISys_Movement_IsMoving())
    AISys_Movement_Wander();
  endif

  AISys_Movement_ProcessMoving();

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_Attention()

  AISys_Movement_SetPause();

  if(AISys_Movement_IsRoundEnabled())
    AISys_Movement_SetRoundPause();
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_GetWaitTime()

  if(AISys_Movement_IsMoving())
    return _AI_Movement.waitTime; // no to chodzimy
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------
