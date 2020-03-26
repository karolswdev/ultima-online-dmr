//-----------------------------------------------------------------------------

function AISys_Movement_ProcEvent(Event)

  case(Event.type)
    
    EVID_WALK:               AISys_Movement_MoveToLoc(AI_MOVETYPE_WALK, Event.source.x, Event.source.y);

    EVID_RUN:                AISys_Movement_MoveToLoc(AI_MOVETYPE_RUN, Event.source.x, Event.source.y);

    EVID_WALK_FOLLOW:        AISys_Movement_MoveToObj(AI_MOVETYPE_FOLLOW_WALK, Event.source);

    EVID_RUN_FOLLOW:         AISys_Movement_MoveToObj(AI_MOVETYPE_FOLLOW_RUN, Event.source);

    EVID_RUN_AWAY:           AISys_Movement_MoveToObj(AI_MOVETYPE_RUN_AWAY, Event.source);

    EVID_FLEE:               AISys_Movement_MoveToObj(AI_MOVETYPE_FLEE, Event.source);

    EVID_KEEP_DISTANCE:      AISys_Movement_MoveToObj(AI_MOVETYPE_KEEP_DISTANCE, Event.source);

    EVID_FOLLOW_STOP:        AISys_Movement_Stop();

    default:                 return EVRES_UNKNOWN;
  endcase

endfunction

//-----------------------------------------------------------------------------
