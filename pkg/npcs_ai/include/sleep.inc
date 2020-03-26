//-----------------------------------------------------------------------------

function AI_IsSleeping()

  return (_AI_Data.npcState == NPCSTATE_SLEEPING);

endfunction

//-----------------------------------------------------------------------------

function AI_EnterSleep()

  var Res := AISys_Handler_Trigger(0, AIHT_ENTER_SLEEP);
  var wakeRange := AI_DEF_WAKE_RANGE;

  // czy skrypcior zwrocil nam moze jakies parametry?

  if(Res)
    if(Res.wakeRange)
      wakeRange := Res.wakeRange;
    endif
  endif

  _AI_Data.npcState := NPCSTATE_SLEEPING;
  
  AI_DisableEvents();
  EnableEvents(EVID_ENTEREDAREA, wakeRange);
  EnableEvents(EVID_LEFTAREA, wakeRange);

  AI_Log("System", "Entered SleepState. Wakeup range: "+wakeRange);

endfunction

//-----------------------------------------------------------------------------

function AI_ExitSleep()

  DisableEvents(EVID_ENTEREDAREA);
  DisableEvents(EVID_LEFTAREA);
  AI_EnableEvents();

  AISys_Handler_Trigger(0, AIHT_EXIT_SLEEP);
  
  _AI_Data.npcState := NPCSTATE_WAITING;

  AI_Log("System", "Exited SleepState. Wakeup time!");
  
endfunction

//-----------------------------------------------------------------------------

function AI_CheckSleep()

  if(!AI_IsSleeping())

    if(_AI_Data.clockSleep + AI_NO_ACTION_SLEEP_WAIT < ReadGameClock())
      AI_EnterSleep();
    endif

  endif

endfunction

//-----------------------------------------------------------------------------

function AI_CheckWakeUp()

  _AI_Data.clockSleep := ReadGameClock();

  if(AI_IsSleeping())
    AI_ExitSleep();
  endif

endfunction

//-----------------------------------------------------------------------------
