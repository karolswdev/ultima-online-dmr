//-----------------------------------------------------------------------------

use npc;

//-----------------------------------------------------------------------------

include "include/std";
include "include/buildings";

//-----------------------------------------------------------------------------

include ":npcs_ai:prepare";
include ":npcs_ai:const";
include ":npcs_ai:operations";
include ":npcs_ai:events";
include ":npcs_ai:debug";
include ":npcs_ai:sleep";

include ":npcs_ai:utils/guards";

include ":ai_speech:prepare";
include ":ai_movement:prepare";
include ":ai_fight:prepare";
include ":ai_merchant:prepare";
include ":ai_handler:prepare";

//-----------------------------------------------------------------------------

function AI_System()

  SetLogRunCycles(LST_AI);

  var Event;

  AI_EnableEvents();

  SetNpcState(NPCSTATE_WAITING);
  AI_Log("System", "Active. Waiting for events.");

  while(Self())

    AI_DefineWaitTime();

    Event := wait_for_event(_AI_Data.waitTime);

    if(Event)
      AI_CheckWakeUp(); // obudzmy naszego npeca, jesli spi!

      AI_Log("System", "Event ["+GetEventName(Event.type)+"]");
      AI_ProcessEvent(Event);
    endif

    if(!AI_IsSleeping()) // jak nie spi, to czas przetworzyc inne sub systemy
      AI_Process();
    endif

    AI_CheckSleep();

  endwhile

  AI_Log("System", "Shutdown!");

endfunction

//-----------------------------------------------------------------------------

function AI_Process()

  AISys_Handler_Process();

  if(AISysEnabled(AI_SYS_MOVEMENT))
    AISys_Movement_Process();
  endif

  if(AISysEnabled(AI_SYS_SPEECH))
    AISys_Speech_Process();
  endif

  if(AISysEnabled(AI_SYS_MERCHANT))
    AISys_Merchant_Process();
  endif
  
  if(AISysEnabled(AI_SYS_FIGHT))
    AISys_Fight_Process();
  endif
  
endfunction

//-----------------------------------------------------------------------------

function AI_ProcessEventResult(byref finalRes, Res)

  if(Res == EVRES_SUCCESS)
    return 1;
  elseif(Res == EVRES_PROCESS)
    finalRes := EVRES_PROCESS;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AI_ProcessEvent(Event)

  var finalRes := EVRES_UNKNOWN;
  var Res;

  Res := AI_TriggerHandleEvent(Event);

  if(AI_ProcessEventResult(finalRes, Res))
    return;
  endif

  if(AISysEnabled(AI_SYS_MOVEMENT))
    Res := AISys_Movement_ProcEvent(Event);

    if(AI_ProcessEventResult(finalRes, Res))
      return;
    endif
  endif

  if(AISysEnabled(AI_SYS_SPEECH))
    Res := AISys_Speech_ProcEvent(Event);
    
    if(AI_ProcessEventResult(finalRes, Res))
      return;
    endif
  endif
  
  if(AISysEnabled(AI_SYS_MERCHANT))
    Res := AISys_Merchant_ProcEvent(Event);
    
    if(AI_ProcessEventResult(finalRes, Res))
      return;
    endif
  endif

  if(AISysEnabled(AI_SYS_FIGHT))
    Res := AISys_Fight_ProcEvent(Event);

    if(AI_ProcessEventResult(finalRes, Res))
      return;
    endif
  endif

  // zaden sub system nie przetworzyl eventa? moze event potrzebuje domyslnej akcji?
  if(finalRes == EVRES_UNKNOWN)
    AI_DefaultHandleEvent(Event);
  endif

endfunction

//-----------------------------------------------------------------------------

function AI_TriggerHandleEvent(Event)

  case(Event.type)
    EVID_ITEM_GIVEN:         if(AISys_Handler_Trigger(Event.source, AIHT_ITEM_GIVEN, Event.item))
                               return EVRES_SUCCESS;
                             endif

    EVID_QUEST_MSG:          if(AISys_Handler_Trigger(Event.source, AIHT_QUEST_MSG, Event))
                               return EVRES_SUCCESS;
                             endif
  endcase

  return EVRES_PROCESS;

endfunction

//-----------------------------------------------------------------------------

function AI_DefaultHandleEvent(Event)

  case(Event.type)
     
    EVID_ITEM_GIVEN:         MoveToPack(Event.item, Event.source);

  endcase

endfunction

//-----------------------------------------------------------------------------

function AI_MinWaitTime(curTime, newTime)

  if(newTime != error and newTime < curTime)
    return newTime;
  else
    return curTime;
  endif

endfunction

//-----------------------------------------------------------------------------

function AI_DefineWaitTime()

  if(AI_IsSleeping())
    _AI_Data.waitTime := AI_SLEEP_EVENT_WAIT;
    return;
  endif


  var curTime := AI_DEF_EVENT_WAIT;

  curTime := AI_MinWaitTime(curTime, AISys_Handler_GetWaitTime());

  if(AISysEnabled(AI_SYS_SPEECH))
    curTime := AI_MinWaitTime(curTime, AISys_Speech_GetWaitTime());
  endif

  if(AISysEnabled(AI_SYS_MOVEMENT))
    curTime := AI_MinWaitTime(curTime, AISys_Movement_GetWaitTime());
  endif

  if(AISysEnabled(AI_SYS_FIGHT))
    curTime := AI_MinWaitTime(curTime, AISys_Fight_GetWaitTime());
  endif

  _AI_Data.waitTime := curTime;

endfunction

//-----------------------------------------------------------------------------

function AI_Attention()

  AISys_Movement_Attention();

endfunction

//-----------------------------------------------------------------------------
