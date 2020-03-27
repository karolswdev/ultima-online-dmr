//-----------------------------------------------------------------------------

include "ai/include/tamed";

//-----------------------------------------------------------------------------

var hatelist := error;

//-----------------------------------------------------------------------------

function main_AI_loop()

  hatelist := GetHateList();

  var wanders   := StartSleep();
  var next_loot := ReadGameClock() + 10;
  
  EnableMainEvents();

  ev := 0;

	Set_Event_Queue_Size(50);
        sleepmode();

  while(me)

	if ( Events_Waiting() > 15 )
		Clear_Event_Queue();
	endif
  
    wanders := wanders + 1;
    
    if(!ev)
      if(MeHaveFlag("TreeForm") and wanders > 20)
        wanders := 0;
        TreeFormChange();
      elseif(wanders >= MAX_WANDERS_UNTIL_SLEEP)
        wanders := 0;
        sleepmode();
      else
        if(!me.hidden)
          if(GetEffectiveSkill(me, SKILLID_HIDING) > 0)
            me.hidden := 1;
          endif
        endif
        look_around();
        ev := wait_for_event(DEF_WILD_WTIME + RandomInt(5));
      endif
    endif

    if(!ev)
      if(me.multi.isa(POLCLASS_BOAT) and me.MoveMode["S"])
        var Boat := me.multi;
        if(MoveMeOffBoat(ev) and Boat)
          RunAwayFromLoc(Boat.x, Boat.y);
        endif
      endif
      
      if(ReadGameClock() > next_loot and !ev)
        grabloot(ev);
        next_loot := ReadGameClock() + 10;
      endif
    endif
    
    if(StdHandleEvent(ev))
      wanders := 0;
    else
      NpcIdle();
    endif
    
    if(!check_anchor())
      DoWander();
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
    EVID_ENTEREDAREA:        if(ev.source and !ev.source.hidden and !ev.source.concealed)
                               if(CanFightEntered(ev.source))
                                 if(CanFightWith(ev.source))
                                   DoMod(me, MOD_RUN_SPEED, RUNSPEED_FAST, 5);
                                   Fight(ev.source);
                                   ev := 0;
                                   return 1;
                                 endif
                               endif
                             endif

    EVID_ENGAGED:           
    EVID_DAMAGED:            Fight(ev.source);
                             ev := 0;
                             return 1;

    EVID_REBORN:             Reborn();
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
    EVID_REBORN:
    EVID_ENTEREDAREA:        return 1;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanFightWith(mob)
  
  if(mob != me)
    if(!CanFightWithPlayer(mob))
      return 0;
    endif

    if((!mob.npctemplate or IsTamedTMF(mob)) and !IsFamily(mob))
      return 1;
    endif

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

  if(MeHaveFlag("SpecialSight"))
    foreach npc in ListMobilesNearLocation(me.x, me.y, me.z, Specs[NSPEC_THRESHOLD], me.realm)
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
  EnableEvents (EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents (EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()
  DisableEvents(EVID_ENGAGED + EVID_DAMAGED + EVID_ENTEREDAREA);
endfunction

//-----------------------------------------------------------------------------

function MoveMeOffBoat(byref ev)

  var boat := me.multi;

  if(!boat.isa(POLCLASS_BOAT) or me.frozen or me.paralyzed)
    return 0;
  endif

  var fp    := boat.footprint;
  var Dir   := DIR_N;
  var xMod  := 0;
  var yMod  := 0;
  var turns := 0;

  if(Abs(fp.xmax - fp.xmin) < Abs(fp.ymax - fp.ymin))
    if(Abs(me.x - fp.xmin) < Abs(me.x - fp.xmax))
      Dir  := DIR_W;
      xMod := -1;
    else
      Dir  := DIR_E;
      xMod := 1;
    endif
  else
    if(Abs(me.y - fp.ymin) < Abs(me.y - fp.ymax))
      Dir  := DIR_N;
      yMod := -1;
    else
      Dir  := DIR_S;
      yMod := 1;
    endif
  endif

  while(me.multi.serial == boat.serial)
    if(!Move(Dir))
      MoveObject(me, me.x+xMod, me.y+yMod, me.z, me.realm, MOVECHAR_FORCELOCATION);

      if(me.multi.serial != boat.serial)
        Dir  := GetReverseDir(Dir);
        xMod *= -1;
        yMod *= -1;
        Face(Dir);
        MoveObject(me, me.x+xMod, me.y+yMod, me.z, me.realm, MOVECHAR_FORCELOCATION);
        turns += 1;

        if(turns == 2)
          if(!Move(Dir))
            MoveObject(me, me.x+xMod, me.y+yMod, me.z, me.realm, MOVECHAR_FORCELOCATION);
          endif
          case(Dir)
            DIR_N:
            DIR_S:  yMod := 0;
                    if(Abs(me.x - fp.xmin) < Abs(me.x - fp.xmax))
                      Dir  := DIR_W;
                      xMod := -1;
                    else
                      Dir  := DIR_E;
                      xMod := 1;
                    endif
            DIR_E:
            DIR_W:  xMod := 0;
                    if(Abs(me.y - fp.ymin) < Abs(me.y - fp.ymax))
                      Dir  := DIR_N;
                      yMod := -1;
                    else
                      Dir  := DIR_S;
                      yMod := 1;
                    endif
          endcase
        elseif(turns >= 4)
          if(!Move(Dir))
            MoveObject(me, me.x+xMod, me.y+yMod, me.z, me.realm, MOVECHAR_FORCELOCATION);
          endif
          LogNotice("[Killpcsloop] MoveMeOffBoat: Npc ["+me.npctemplate+"/"+me.serial+"] nie mogl zejsc z lodzi ["+CInt(boat.serial)+"], Loc: "+me.x+", "+me.y+", "+me.z+", "+me.realm, LOGF_SCRIPTS);
          return 0;
        endif
      endif
    endif

    ev := wait_for_event(0);
    if(CanHandleEvent(ev))
      return 0;
    endif

    Sleepms(1);
  endwhile

  return 1;

endfunction

//-----------------------------------------------------------------------------
