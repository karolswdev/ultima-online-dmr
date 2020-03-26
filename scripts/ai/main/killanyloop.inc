//-----------------------------------------------------------------------------

include "ai/include/tamed";

//-----------------------------------------------------------------------------

function main_AI_loop()

  var wanders   := StartSleep();
  var next_loot := ReadGameClock() + 10;
  
  EnableMainEvents();

	Set_Event_Queue_Size(50);
        sleepmode();

  while(me)

	if ( Events_Waiting() > 15 )
		Clear_Event_Queue();
	endif

    if(IsSummoned(me))
      if(CInt(GetObjProperty(me, "summonedtime")) and (CInt(GetObjProperty(me, "summonedtime")) + 600 < ReadGameClock()))
        KillSummonedCreature(me);
        exit;
      endif
    endif

    check_anchor();
    DoWander();
    wanders := wanders + 1;

    if(!ev)
      if(wanders >= MAX_WANDERS_UNTIL_SLEEP)
        wanders := 0;
        sleepmode();
      else
        look_around();
        ev := wait_for_event(DEF_WILD_WTIME + RandomInt(5));
      endif

      if(!ev)
        if(ReadGameClock() > next_loot)
          grabloot(ev);
          next_loot := ReadGameClock() + 5 + RandomInt(10);
        endif
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
    EVID_ENTEREDAREA:        if(CanFightEntered(ev.source) and !ev.source.hidden)
                               if(CanFightWith(ev.source))
                                 DoMod(me, MOD_RUN_SPEED, RUNSPEED_FAST, 5);
                                 Fight(ev.source);
                                 ev := 0;
                                 return 1;
                               endif
                             endif

    EVID_ENGAGED:
    EVID_DAMAGED:            Fight(ev.source);
                             ev := 0;
                             return 1;
  endcase

  return StdMovNpcHandleEvent(ev);

endfunction

//-----------------------------------------------------------------------------

function CanHandleEvent(byref ev)

  case(ev.type)
    EVID_DAMAGED:
    EVID_ENGAGED:     
    EVID_ITEM_GIVEN:  
    EVID_RUN_FOLLOW:
    EVID_WALK_FOLLOW:
    EVID_RUN:
    EVID_WALK:
    EVID_ENTEREDAREA:        return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(Mob)

  if(Mob == me)
    return 0;
  elseif(Mob.isa(POLCLASS_NPC) and Mob.master != 0)
    if(Mob.master.serial == me.serial)
      return 0;
    endif
  else
    if(!CanFightWithPlayer(mob))
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function look_around()

  if(MeHaveFlag("SpecialSight"))
    foreach npc in ListMobilesNearLocation(me.x,me.y,me.z, Specs[NSPEC_THRESHOLD], me.realm)
      if(CanFightWith(npc))
        Fight(npc);
        break;
      endif
    endforeach
  else
    foreach npc in ListMobilesInLineOfSight(me, Specs[NSPEC_THRESHOLD])
      if(CanFightWith(npc))
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
