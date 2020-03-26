//-----------------------------------------------------------------------------

include "ai/include/stsleep";

//-----------------------------------------------------------------------------

function main_AI_loop(next_wander := 0)

  var wanders := StartSleep();
  EnableMainEvents();

	Set_Event_Queue_Size(50);
        sleepmode();

  while(me)

	if ( Events_Waiting() > 15 )
		Clear_Event_Queue();
	endif

    DoWander();

    wanders := wanders + 1;

    if(!ev)
      if(wanders >= MAX_WANDERS_UNTIL_SLEEP)
        wanders := 0;
        sleepmode();
      else
        ev := wait_for_event(DEF_WAITTIME + RandomInt(10));
      endif
    endif
    
    NpcIdle();

    if(StdHandleEvent(ev))
      wanders := 0;
    endif
    
    check_anchor();
    
 sleepms(5);
 endwhile

endfunction

//-----------------------------------------------------------------------------

function StdHandleEvent(byref ev)

  case(ev.type)
    EVID_ENTEREDAREA:        if(ev.source and !ev.source.hidden)
                               if(CanFightWith(ev.source) and CanFightEntered(ev.source))
                                 RunAwayFrom(ev.source);
                                 ev := 0;
                                 return 1;
                               endif
                             endif
    EVID_PUSH:
    EVID_ENGAGED:
    EVID_DAMAGED:            if(ev.source)
                               Fight(ev.source);
                               ev := 0;
                               return 1;
                             endif
  endcase

  return StdMovNpcHandleEvent(ev);

endfunction

//-----------------------------------------------------------------------------

function CanHandleEvent(byref ev)

  case(ev.type)
    EVID_ENTEREDAREA:        
    EVID_PUSH:
    EVID_ENGAGED:
    EVID_DAMAGED:   
    EVID_RUN_FOLLOW:
    EVID_WALK_FOLLOW:
    EVID_RUN:                
    EVID_WALK:               return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(Mob)

  if(Mob == me)
    return 0;
  endif

  if(IsTamedTMF(mob))
    return 1;
  endif

  if(!CanFightWithPlayer(mob))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function look_around()

  return 1;

endfunction

//-----------------------------------------------------------------------------

function EnableMainEvents()

  DisableEvents(EVID_LEFTAREA);
  EnableEvents( EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents( EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);

endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()

  DisableEvents( EVID_ENGAGED);
  DisableEvents( EVID_DAMAGED);
  DisableEvents( EVID_ENTEREDAREA);

endfunction

//-----------------------------------------------------------------------------
