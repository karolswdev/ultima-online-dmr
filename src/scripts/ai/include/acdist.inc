//=============================================================================
//-----------------------------------------------------------------------------
// Archer Close Distance
//-----------------------------------------------------------------------------
//=============================================================================

include "smartrun";

//-----------------------------------------------------------------------------

function InitRun(opponent)

  runaway    := 0;
  InitSmartRunMob(opponent);
  return 0;

endfunction

//-----------------------------------------------------------------------------

function CloseDistance(opponent)

  if(!CheckLineOfSight(me,opponent))
    return !SmartRunToward(opponent);
  endif
  
  if(Distance(me,opponent) > 4 and Distance(me,opponent) < MinMax(CInt(GetItemConfig(me.weapon).maxrange),6,12))
    SmartRunMobSucceded(opponent);
    TurnToward(opponent);
    return 1;
  endif

  if( runaway < 100 )
    if(Distance(me, opponent) <= 4)
      if(RunAwayFrom(opponent))
        runaway := runaway + 1;
        return 0;
      endif
    else
      runaway := 0;
      return !RunToward(opponent);
    endif
  else
    case (Distance( me, opponent ))
      0:        
               WalkAwayFrom(opponent);
               return 0;
      1:
               SmartRunMobSucceded(opponent);
               TurnToward(opponent);
               return 1;
      default:
               OpenDoors();
               return !SmartRunToward(opponent);
    endcase
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
