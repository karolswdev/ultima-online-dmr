//-----------------------------------------------------------------------------

include "include/attributes/main";

//-----------------------------------------------------------------------------

function sleepmode()

  var sModeOn := 1;

  if(GetEffectiveSkill(me, SKILLID_HIDING) > 0)
    me.hidden := 1;
  endif

  DisableMainEvents();
  DisableEvents(EVID_LEFTAREA + EVID_SPEECH);
  EnableEvents(EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
  EnableEvents(EVID_ENGAGED);
  EnableEvents(EVID_DAMAGED);

  while(me and sModeOn)

	if ( Events_Waiting() > 25 )
		Clear_Event_Queue();
	endif

    ev := wait_for_event(DEF_WAITTIME + RandomInt(30));

    if(RandomInt(100) < 15)
      PlayIdleSound();
    endif

    case(ev.type)
      EVID_ENTEREDAREA:      if(ev.source and !ev.source.hidden)
                               if(IsPlayer(ev.source))
                                 sModeOn := 0;
                               elseif(CanFightEntered(ev.source))
                                 if(CanFightWith(ev.source))
                                   UnHide(me);
                                   sModeOn := 0;
                                 endif
                               endif
                             endif

      EVID_ENGAGED:
      EVID_DAMAGED:          if(ev.source)
                               UnHide(me);
                               sModeOn := 0;
                             endif

      EVID_WALK_FOLLOW:
      EVID_RUN_FOLLOW:
      EVID_RUN:                
      EVID_WALK:             sModeOn := 0;

      default:               CheckAnimalSpecs();
                             CheckLifeTimers();
    endcase

  endwhile
  
  EnableMainEvents();

endfunction

//-----------------------------------------------------------------------------
