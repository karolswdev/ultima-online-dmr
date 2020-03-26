//-----------------------------------------------------------------------------
// Obchod, czyli krecenie sie Npeca
//-----------------------------------------------------------------------------

include "include/const/misc";

//-----------------------------------------------------------------------------

function AISys_Movement_InitRound()

  var Points := GetProperty(PROP_ROUND_DESTS);

  if(Points and Points.size())

    _AI_Movement.Round := struct;

    _AI_Movement.Round.+points := Points;
    _AI_Movement.Round.+curLoc;
    _AI_Movement.Round.+nextPoint := 1;
    _AI_Movement.Round.+curPath;
    _AI_Movement.Round.+curIndex;
    _AI_Movement.Round.+errorCount := 0;
    _AI_Movement.Round.reached := 0;

    _AI_Movement.Round.curLoc    := GetProperty("WalkDest");
    _AI_Movement.Round.nextPoint := CInt(GetProperty("NextWalkPoint"));
    _AI_Movement.Round.reached   := AISys_Movement_RoundReached();

    AI_Log("Movement", "Rounding prepared ["+Points.size()+"]");

    //AISys_Movement_SetRoundPause(CInt(DEF_WALK_PAUSE_TIME / 2) + RandomInt(DEF_WALK_PAUSE_TIME));

  else

    _AI_Movement.Round := 0;

  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_IsRoundEnabled()

  if(_AI_Movement.Round.points)
    return 1;
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_ProceedRoundPath()

  if(CInt(_AI_Movement.Round.nextPoint) <= 0)
    _AI_Movement.Round.nextPoint := 1;
  elseif(_AI_Movement.Round.nextPoint > _AI_Movement.Round.points.size())
    _AI_Movement.Round.nextPoint := 1;
  endif

  // jesli mamy zdefiniowana sciezke - znaczy, ze poprzedni punkt byl ok.
  // jesli nie mamy aktualnego punktu - sciezka nowka
  if(_AI_Movement.Round.reached or !_AI_Movement.Round.curLoc) 
    _AI_Movement.Round.curLoc    := _AI_Movement.Round.points[_AI_Movement.Round.nextPoint];
    _AI_Movement.Round.nextPoint += 1;
    _AI_Movement.Round.reached := 0;
    
    SetProperty("NextWalkPoint", _AI_Movement.Round.nextPoint);
    SetProperty("WalkDest", _AI_Movement.Round.curLoc);

    AISys_Movement_Stop();
  endif

  if(!AISys_Movement_IsMoving())
    AISys_Movement_MoveToLoc(AI_MOVETYPE_WALK, _AI_Movement.Round.curLoc[1], _AI_Movement.Round.curLoc[2]);
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_RoundReached()

  var CurDest := _AI_Movement.Round.curLoc;

  if(CurDest)
    var Dist := coordist(Self().x, Self().y, CurDest[1], CurDest[2]);

    return (Dist < 2);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_ProcessRound()

  if(AISys_Movement_IsRounding())

    if(AISys_Movement_RoundReached() and !_AI_Movement.Round.reached)
      // no to doszlismy!
      // zrzucmy anchora i wlaczmy go!

      AI_Log("Movement", "RoundPoint "+ToMin(_AI_Movement.Round.nextPoint-1, 1)+"/"+_AI_Movement.Round.points.size()+" reached. ["+_AI_Movement.Round.curLoc[1]+","+_AI_Movement.Round.curLoc[2]+"]");

      AISys_Movement_DropAnchor();
      AISys_Movement_EnableAnchor();

      // osiagnelismy cel! aktualny punkcior!
      _AI_Movement.Round.reached := 1;

      // maly postoj w chodzeniu
      AISys_Movement_SetRoundPause(SHORT_WALK_PAUSE + RandomInt(SHORT_WALK_PAUSE*3));

    else

      AISys_Movement_ProceedRoundPath();

    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_IsRounding()

  if(AISys_Movement_IsRoundEnabled())

    if(AISys_Fight_InCombat())
      return 0;
    endif

    var Pause := GetProperty("#RoundPause");

    if(Pause != error)
      if(CInt(Pause) > ReadGameClock())
        return 0;
      endif

      EraseObjProperty(Self(), "#RoundPause");
    endif

    return 1;

  endif

  return error;
  
endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_SetRoundPause(Time := DEF_WALK_PAUSE_TIME)

  SetProperty("#RoundPause", ReadGameClock() + Time);

endfunction

//-----------------------------------------------------------------------------
