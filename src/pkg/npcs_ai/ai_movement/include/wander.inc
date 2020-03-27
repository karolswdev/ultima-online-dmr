//-----------------------------------------------------------------------------

function AISys_Movement_Wander()

  if(!IsSetFlag(Self(), NPCF_NO_WANDER))  

    if(_AI_Movement.clockWander > ReadGameClock())

      AISys_Handler_Trigger(0, AIHT_ON_WANDER);

      _AI_Movement.clockWander := ReadGameClock() + _AI_Movement.wanderTime + RandomInt(_AI_Movement.wanderTime / 2);
      Wander();

    endif

  endif

endfunction

//-----------------------------------------------------------------------------
