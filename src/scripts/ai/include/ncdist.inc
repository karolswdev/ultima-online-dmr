//=============================================================================
//-----------------------------------------------------------------------------
// Normal Close Distance
//-----------------------------------------------------------------------------
//=============================================================================

function InitRun(opponent)

  if(opponent)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CloseDistance(opponent)

  if(me.frozen or me.paralyzed)
    return 1;
  endif

  case (Distance( me, opponent ))
    0:
             WalkAwayFrom(opponent);
    1: 
             TurnToward(opponent);
             return 1;
             
    default: return !RunToward(opponent);

  endcase

  return 1;

endfunction

//-----------------------------------------------------------------------------
