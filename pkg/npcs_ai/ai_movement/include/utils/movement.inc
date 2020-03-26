//--------------------------------------------------------------------------

include ":ai_movement:const";

//--------------------------------------------------------------------------

const NPCMOVE_WAIT           := 0x01;

//--------------------------------------------------------------------------

function SendMovement(Type, Npc, Source, Flags := 0)

  var evId := 0;
  var srcLoc := struct{"x", "y"};
  var destSource;
  
  srcLoc.x := Source.x;
  srcLoc.y := Source.y;

  case(Type)

    AI_MOVETYPE_RUN:            evId := EVID_RUN;
                                destSource := srcLoc;

    AI_MOVETYPE_WALK:           evId := EVID_WALK;
                                destSource := srcLoc;

    AI_MOVETYPE_FOLLOW_RUN:     evId := EVID_RUN;
                                destSource := Source;

    AI_MOVETYPE_FOLLOW_WALK:    evId := EVID_WALK;
                                destSource := Source;

    AI_MOVETYPE_FLEE:           evId := EVID_FLEE;
                                destSource := Source;

    AI_MOVETYPE_RUN_AWAY:       evId := EVID_RUN_AWAY;
                                destSource := Source;

    AI_MOVETYPE_KEEP_DISTANCE:  evId := EVID_KEEP_DISTANCE;
                                destSource := Source;

  endcase

  if(!evId)
    return error;
  endif

  var Event := struct;

  Event.+source := destSource;
  Event.+type   := evId;

  SendEvent(Npc, Event);

  if(Flags & NPCMOVE_WAIT)
    
    var Count := 0;

    while(Count < 50)
      
      if(coordist(Npc.x, Npc.y, destSource.x, destSource.y) < 2)
        return 1;
      endif

      Count += 1;

      sleepms(500);

    endwhile

  else

    return 1;

  endif

  return 0;

endfunction

//--------------------------------------------------------------------------
