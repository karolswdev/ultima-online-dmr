//-----------------------------------------------------------------------------

var hatelist := error;

//-----------------------------------------------------------------------------

function main_AI_loop()

  hatelist := GetHateList();

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
      if(wanders >= 60)
        wanders := 0;
        sleepmode();
      else
        look_around();
        ev := wait_for_event(DEF_WAITTIME + RandomInt(15));
      endif
    endif
    
    if(StdHandleEvent(ev))
      wanders := 0;
    endif

    check_anchor();
    AnimalIdle();
  sleepms(5);
  endwhile
  
endfunction

//-----------------------------------------------------------------------------

function StdHandleEvent(byref ev)

  if(!ev)
    return 0;
  endif

  case(ev.type)

    EVID_PUSH:
    EVID_DAMAGED: 
    EVID_ENGAGED:            if(MeHaveFlag("RunLikeHell"))
                               RunLikeHell(ev.source);
                               ev := 0;
                               return 1;
                             else
                               Fight(ev.source);
                               ev := 0;
                               return 1;
                             endif
                           
    EVID_ENTEREDAREA:        if(CheckFight(ev))
                               ev := 0;
                               return 1;
                             endif
                             
    EVID_DOUBLECLICKED:      if(Distance(me, ev.source) < 4 and RandomInt(100) < 40)
                               if(RandomInt(40) == 20)
                                 NpcIdle();
                               else
                                 PlayIdleSound();
                               endif
                             endif

                             ev := 0;
                             return 1;
                             
  endcase

   return StdMovNpcHandleEvent(ev);

endfunction

//-----------------------------------------------------------------------------

function CanHandleEvent(byref ev)

  case(ev.type)
    EVID_PUSH:
    EVID_DAMAGED:
    EVID_ENGAGED:     
    EVID_ITEM_GIVEN:  
    EVID_RUN_FOLLOW:
    EVID_WALK_FOLLOW:
    EVID_DOUBLECLICKED:
    EVID_RUN:
    EVID_WALK:
    EVID_ENTEREDAREA:        return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckFight(ev)

  if(ev.source and !ev.source.hidden)
    if(CanFightWith(ev.source) and CanFightEntered(ev.source))
      Fight(ev.source);
      return 1;
    elseif(IsPlayer(ev.source))
      if(RandomInt(100) < 20)
        PlayIdleSound();
      endif
    endif
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function CanFightWith(mob)

  if(Mob != me)
    if(mob.isa(POLCLASS_NPC) and hatelist)
      if(mob.npctemplate in hatelist)
        return 1;
      elseif(mob.script in hatelist)
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function look_around()

  if(hatelist)
    foreach critter in ListMobilesInLineOfSight(me, Specs[NSPEC_THRESHOLD])
      if(CanFightWith(critter))
        Fight(critter);
        return 1;
      endif
    endforeach
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function EnableMainEvents()
  DisableEvents(EVID_SPEECH + EVID_LEFTAREA);
  EnableEvents (EVID_FLEE);
  EnableEvents(EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
  EnableEvents (EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents (EVID_DOUBLECLICKED);
endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()
  DisableEvents(EVID_ENGAGED + EVID_DAMAGED + EVID_ENTEREDAREA);
endfunction

//-----------------------------------------------------------------------------
