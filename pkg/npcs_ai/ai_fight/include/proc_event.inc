//-----------------------------------------------------------------------------

function AISys_Fight_ProcEvent(Event)

  case(Event.type)
    
    EVID_ENGAGED:            ;

    default:                 return EVRES_UNKNOWN;
  endcase

endfunction

//-----------------------------------------------------------------------------

