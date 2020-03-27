//-----------------------------------------------------------------------------

function main_AI_loop()

  var wanders := StartSleep();

  EnableMainEvents();

	Set_Event_Queue_Size(50);
        sleepmode();

  while(me)

	if ( Events_Waiting() > 15 )
		Clear_Event_Queue();
	endif

    if(!ev)
      if(wanders >= MAX_WANDERS_UNTIL_SLEEP)
        wanders := 0;
        sleepmode();
      else
        ev := os::wait_for_event(DEF_WAITTIME + RandomInt(10));
      endif
    endif
    
    NpcIdle();

    if(StdHandleEvent(ev))
      wanders := 0;
    endif
    
  sleepms(5);
  endwhile

endfunction

//-----------------------------------------------------------------------------

function StdHandleEvent(byref ev)

  if(!ev)
    return 0;
  endif

  case(ev.type)
    EVID_ENTEREDAREA:        if(ev.source and !ev.source.hidden)
                               if(CanFightWith(ev.source) and CanFightEntered(ev.source))
                                 Fight(ev.source);
                                 ev := 0;
                                 return 1;
                               endif
                             endif

    EVID_PUSH:
    EVID_ENGAGED:
    EVID_DAMAGED:            Fight(ev.source);
                             ev := 0;
                             return 1;
  endcase

  ev := 0;
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanHandleEvent(byref ev)

  case(ev.type)
    EVID_ENTEREDAREA:        
    EVID_PUSH:
    EVID_ENGAGED:
    EVID_DAMAGED:            return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(Mob)

  if(mob != me)
    if(!CanFightWithPlayer(mob))
      return 0;
    endif

    if((!mob.npctemplate or IsTamedTMF(mob)) and !IsFamily(mob))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function look_around()

  if(MeHaveFlag("SpecialSight"))
    foreach npc in ListMobilesNearLocation(me.x,me.y,me.z, Specs[NSPEC_THRESHOLD], me.realm)
      if(CanFightWith(Npc))
        Fight(npc);
        break;
      endif
    endforeach
  else
    foreach npc in ListMobilesInLineOfSight(me, Specs[NSPEC_THRESHOLD])
      if(CanFightWith(Npc))
        Fight(npc);
        break;
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function EnableMainEvents()
  DisableEvents(EVID_SPEECH + EVID_LEFTAREA + EVID_DISENGAGED + EVID_OPPONENT_MOVED);
  EnableEvents(EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents(EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()
  DisableEvents(EVID_ENGAGED + EVID_DAMAGED + EVID_ENTEREDAREA);
endfunction

//-----------------------------------------------------------------------------
