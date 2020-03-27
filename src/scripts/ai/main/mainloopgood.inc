//-----------------------------------------------------------------------------

var Memb;

//-----------------------------------------------------------------------------

function main_AI_loop()

  var wanders     := StartSleep();
  var next_wander := ReadGameClock() + 60;
  var next_loot   := ReadGameClock() + 20;

  EnableMainEvents();
  
	Set_Event_Queue_Size(50);
        sleepmode();

  while(me)

	if ( Events_Waiting() > 15 )
		Clear_Event_Queue();
	endif
   
    check_anchor();

    if(ReadGameClock() > next_wander)
      DoWander();
      next_wander := ReadGameClock() + 15 + RandomInt(45);
      wanders := wanders + 3;
    endif

    if(!ev)
      if(MeHaveFlag("TreeForm") and wanders > 20)
        wanders := 0;
        TreeFormChange();
      elseif(wanders >= MAX_WANDERS_UNTIL_SLEEP)
        wanders := 0;
        sleepmode();
      elseif(!ev)
        ev := wait_for_event(DEF_WILD_WTIME + RandomInt(5));
      endif
    endif
    
    if(StdHandleEvent(ev))
      wanders := 0;
    endif
    
    NpcIdle();

    if(ReadGameClock() > next_loot)
      grabloot(ev);
      next_loot := ReadGameClock() + 10 + RandomInt(10);
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
    EVID_SPEECH:             if(CanFightWith(ev.source))
                               Fight(ev.source);
                               ev := 0;
                               return 1;
                             endif

    EVID_PUSH:
    EVID_ENGAGED:
    EVID_DAMAGED:            Fight(ev.source);
                             ev := 0;
                             return 1;

    EVID_ENTEREDAREA:        if(ev.source and !ev.source.hidden)
                               if(CanFightWith(ev.source) and CanFightEntered(ev.source))
                                 DoMod(me, MOD_RUN_SPEED, RUNSPEED_FAST, 5);
                                 Fight(ev.source);
                                 ev := 0;
                                 return 1;
                               endif
                             endif
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
    EVID_RUN:
    EVID_WALK:
    EVID_ENTEREDAREA:        return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(npc)

  if(npc == me)
    return 0;
  endif

  if(!IsPlayer(npc))
    if(IsTamed(npc))
      var Master := GetMaster(npc);
      if(Master.criminal or Master.murderer)
        SetObjProperty(Master, "WasCrim",1);
        SetCriminal(Master, 1, "CanFightWith", "Npc "+me.name+" ["+me.serial+"] zobaczyl je"+ggm(Master,19)+" zwierzaka "+npc.name+" ["+npc.serial+"], gdy Master byl: Krim: "+Master.criminal+", Murd: "+Master.murderer+". Zapamietan"+ggm(Master,6)+" przez Npca.");
        AddToRemember(Master);
        return 1;
      endif
    endif
  else
    
    if(!CanFightWithPlayer(npc))
      return 0;
    endif

    if(npc.cmdlevel and !GetObjProperty(npc, "Possess"))
      return 0;
    endif

    if(npc.criminal or npc.murderer or Remember(npc))
      SetObjProperty(npc,"WasCrim",1);
      SetCriminal(npc, 1, "CanFightWith", "Npc "+me.name+" ["+me.serial+"] zobaczyl "+ggm(npc,16)+", gdy byl"+ggm(npc,4)+": Krim: "+npc.criminal+", Murd: "+npc.murderer+", Pamietan"+ggm(npc,6)+": "+Remember(npc)+". Zapamietan"+ggm(npc,6)+" przez Npca.");
      AddToRemember(Npc);
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
  DisableEvents(EVID_SPEECH + EVID_DISENGAGED + EVID_OPPONENT_MOVED + EVID_LEFTAREA);

  EnableEvents(EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents(EVID_SPEECH, Specs[NSPEC_THRESHOLD]);
  EnableEvents(EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()
  DisableEvents(EVID_SPEECH + EVID_ENGAGED + EVID_DAMAGED + EVID_ENTEREDAREA);
  DisableEvents(EVID_ITEM_GIVEN);
endfunction

//-----------------------------------------------------------------------------

function AddToRemember(who)

  Memb := GetObjProperty(me, PROP_NPC_MEMBER);
  if(!Memb)
    Memb := dictionary;
  endif
  
  Memb[who.serial] := ReadGameClock() + NPC_MEMBER_TIME;
  SetObjProperty(me, PROP_NPC_MEMBER,Memb);

endfunction

//-----------------------------------------------------------------------------

function Remember(who)

  Memb := GetObjProperty(me, PROP_NPC_MEMBER);
  
  if(CInt(Memb[who.serial]) > ReadGameClock())
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
