//=============================================================================
//-----------------------------------------------------------------------------
// Smart Close Distance
//-----------------------------------------------------------------------------
//=============================================================================

include "smartrun";

//-----------------------------------------------------------------------------

function InitRun(opponent)
  
  if(AiFlags & AIF_IMMOBILE)
    return;
  endif

  InitSmartRunMob(opponent);

endfunction

//-----------------------------------------------------------------------------

function CloseDistance(opponent)

  if(AiFlags & AIF_IMMOBILE)
    sleepms(50);
    return 1;
  endif

  if(me.frozen or me.paralyzed)
    return 1;
  endif

  if(!CheckLineOfSight(me, opponent))
    return !SmartRunToward(opponent);
  endif

  case (Distance( me, opponent ))
    0:
             WalkAwayFrom(opponent);
    1:
             SmartRunMobSucceded(opponent);
             TurnToward(opponent);
             return 1;
             
    default: return !SmartRunToward(opponent);

  endcase

  return 1;

endfunction

//-----------------------------------------------------------------------------
