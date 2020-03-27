//-----------------------------------------------------------------------------

function AISys_Movement_MoveToObj(Type, Obj)

  if(!Type or !Obj.isa(POLCLASS_UOBJECT))
    return;
  endif

  AISys_Movement_Stop();
  AI_Log("Movement", "Move to object: "+GetName(Obj));

      // no to probujemy dojsc do celu...
  _AI_Movement.moveType := Type;
  _AI_Movement.moveTarg := Obj;
  _AI_Movement.checkCount := AI_MOVEMENT_CHECK_COUNT;
  
      // nie chcemy, by anchor w tym przeszkadzal
  AISys_Movement_DisableAnchor();
  AISys_Movement_BindStuck();

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_MoveToLoc(Type, toX, toY)

  if(!Type or !toX or !toY)
    return;
  endif

  AISys_Movement_Stop();
  AI_Log("Movement", "Move to location: "+toX+" "+toY);

      // no to probujemy dojsc do celu...
  _AI_Movement.moveType := Type;
  _AI_Movement.checkCount := AI_MOVEMENT_CHECK_COUNT;

  _AI_Movement.moveTarg := struct;
  _AI_Movement.moveTarg.+x := toX;
  _AI_Movement.moveTarg.+y := toY;

      // nie chcemy, by anchor w tym przeszkadzal
  AISys_Movement_DisableAnchor();
  AISys_Movement_BindStuck();

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_Stop()

  if(!_AI_Movement.moveType)
    return;
  endif

  AI_Log("Movement", "Move Stop!");

  _AI_Movement.moveType     := 0;
  _AI_Movement.moveTarg     := 0;
  _AI_Movement.curPath      := error;
  _AI_Movement.curPathIndex := 0;
  _AI_Movement.errorCount   := 0;
  _AI_Movement.movePause    := 0;
  _AI_Movement.checkCount   := 0;
  _AI_Movement.stuckCount   := 0;
  _AI_Movement.stuckLoc     := 0;
  _AI_Movement.distCount    := 0;
  _AI_Movement.frozenCount  := 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_IsMoving()

  if(_AI_Movement.moveType)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_ResetPath()

  if(_AI_Movement.curPath)
    _AI_Movement.curPath      := error;
    _AI_Movement.curPathIndex := 1;

    AI_Log("Movement", "ResetPath.");
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_ProcessPath()

  if(!_AI_Movement.curPath or _AI_Movement.curPathIndex > _AI_Movement.curPath.size())
    AISys_Movement_FindPath();
  endif

  if(_AI_Movement.curPath and _AI_Movement.curPathIndex <= _AI_Movement.curPath.size())
    var Loc := _AI_Movement.curPath[_AI_Movement.curPathIndex];

    if(Loc.x == Self().x and Loc.y == Self().y)
      _AI_Movement.curPathIndex += 1;
    endif
  endif

  if(_AI_Movement.curPathIndex > _AI_Movement.curPath.size())
    AISys_Movement_FindPath();
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_FindPath()

  _AI_Movement.curPath := AISys_Movement_GetPath(_AI_Movement.moveTarg.x, _AI_Movement.moveTarg.y);

  if(!_AI_Movement.curPath)
    _AI_Movement.errorCount += 1;
    return 0;
  else
    _AI_Movement.errorCount := 0;
    _AI_Movement.curPathIndex := 1;
    
    AI_Log("Movement", "FindPath: ["+_AI_Movement.moveTarg.x+","+_AI_Movement.moveTarg.y+"] Points: "+_AI_Movement.curPath.size());
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_DestReached()

  var Dist := CoorDist(Self().x, Self().y, _AI_Movement.moveTarg.x, _AI_Movement.moveTarg.y);

  case(_AI_Movement.moveType)
  
    AI_MOVETYPE_FLEE:
    AI_MOVETYPE_RUN_AWAY:      if(Dist >= AI_MOVEMENT_SAFE_DISTANCE or _AI_Movement.distCount < AI_MOVEMENT_MIN_KEEP_DIST_COUNT)
                                 return 1;
                               endif
                             
    AI_MOVETYPE_KEEP_DISTANCE: if(Dist >= AI_MOVEMENT_GOOD_DISTANCE or _AI_Movement.distCount < AI_MOVEMENT_MIN_KEEP_DIST_COUNT)
                                 return 1;
                               endif

    default:                   if(Dist < 2)
                                 return 1;
                               endif

  endcase
    
  if(Dist > AI_MOVEMENT_MAX_PATH_DIST) 

    // jak sie za daleko oddali od celu, to niech wraca na anchora
    AISys_Movement_MoveToAnchor();
    AISys_Movement_Stop();
    return error;

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_SetPause(Time := AI_MOVEMENT_DEF_PAUSE_TIME)

  _AI_Movement.movePause := ReadGameClock() + Time;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_IsPaused()

  if(!AISys_Fight_InCombat())

    if(_AI_Movement.movePause > ReadGameClock())
      return 1;
    elseif(_AI_Movement.movePause > 0)
      _AI_Movement.movePause := 0;
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_Go()

  var Loc, Res;

  if(_AI_Movement.curPath)
    Loc := _AI_Movement.curPath[_AI_Movement.curPathIndex];
  else
    Loc := _AI_Movement.moveTarg;
  endif

  case(_AI_Movement.moveType)

    AI_MOVETYPE_FLEE:           if(Loc.isa(POLCLASS_UOBJECT))
                                  Res := WalkAwayFrom(Loc);
                                else
                                  Res := WalkAwayFromLocation(Loc.x, Loc.y);
                                endif

    AI_MOVETYPE_RUN_AWAY:
    AI_MOVETYPE_KEEP_DISTANCE:  if(Loc.isa(POLCLASS_UOBJECT))
                                  Res := RunAwayFrom(Loc);
                                else
                                  Res := RunAwayFromLocation(Loc.x, Loc.y);
                                endif

    AI_MOVETYPE_FOLLOW_RUN:     
    AI_MOVETYPE_RUN:            if(Loc.isa(POLCLASS_UOBJECT))
                                  Res := RunToward(Loc);
                                else
                                  Res := RunTowardLocation(Loc.x, Loc.y);
                                endif
    AI_MOVETYPE_FOLLOW_WALK:
    AI_MOVETYPE_WALK:           if(Loc.isa(POLCLASS_UOBJECT))
                                  Res := WalkToward(Loc);
                                else
                                  Res := WalkTowardLocation(Loc.x, Loc.y);
                                endif

  endcase

  if(_AI_Movement.curPath)

    if(Self().x == Loc.x and Self().y == Loc.y) // doszedl w lokacje, ktora powinien ??
      _AI_Movement.curPathIndex += 1;
    endif

  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_BindStuck()

  _AI_Movement.stuckLoc := struct{"x", "y"};
  
  _AI_Movement.stuckLoc.x := Self().x;
  _AI_Movement.stuckLoc.y := Self().y;

  _AI_Movement.stuckCount := 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_CheckStuck()

  if(_AI_Movement.errorCount > AI_MOVEMENT_MAX_ERRORS)
    AI_LogError("Movement", "Cant get path to ["+_AI_Movement.moveTarg.x+","+_AI_Movement.moveTarg.y+"]. ErrorCount ["+AI_MOVEMENT_MAX_ERRORS+"] reached!");
    AI_LogError("Movement", "Stop movement and return to anchor. Error: "+_AI_Movement.curPath.errortext);

    // mamy max limit bledow!
    AISys_Movement_Stop();
    AISys_Movement_MoveToAnchor();
    return 0;
  endif

  if(!_AI_Movement.stuckLoc)
    AISys_Movement_BindStuck();
  endif
    
  var LastDist := CoorDist(Self().x, Self().y, _AI_Movement.stuckLoc.x, _AI_Movement.stuckLoc.y);

  // jak nie oddalil sie nasz npc, to zwiekszamy count.
  if(LastDist < 3)
    _AI_Movement.stuckCount += 1;
  else
    AISys_Movement_BindStuck();
  endif

  if(_AI_Movement.stuckCount > AI_MOVEMENT_STUCK_COUNT)
    AI_LogError("Movement", "CheckStuck: NPC no movement since "+AI_MOVEMENT_STUCK_COUNT+" moves.");

    // nasz NPC sie nie poruszyl!
    AISys_Movement_Stop();
    AISys_Movement_MoveToAnchor();

    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_PrepareTrack()

  if(_AI_Movement.moveTarg.isa(POLCLASS_UOBJECT))

    if(_AI_Movement.curPath)

      var LastPoint := _AI_Movement.curPath[_AI_Movement.curPath.size()];

      // jesli MOB nam sie oddalil z konca ostatniej sciezki, trzeba ja przegenerowac.
      if(CoorDist(_AI_Movement.moveTarg.x, _AI_Movement.moveTarg.y, LastPoint.x, LastPoint.y) > 2)
        AISys_Movement_ResetPath();
        _AI_Movement.checkCount := AI_MOVEMENT_CHECK_COUNT;
      endif

    endif

    // czas by sprawdzic, czy NPC potrzebuje sciezki?
    if(_AI_Movement.checkCount >= AI_MOVEMENT_CHECK_COUNT)

      if(CoordLosAt(Self(), _AI_Movement.moveTarg))
        AISys_Movement_ResetPath();
      elseif(!_AI_Movement.curPath)
        AISys_Movement_FindPath();
      endif

      _AI_Movement.checkCount := 0;

    endif

    _AI_Movement.checkCount += 1;

    if(!AISys_Movement_CheckStuck())
      return 0;
    endif

    // jesli mamy sciezke, to niech NPC podaza nia
    if(_AI_Movement.curPath)
      return AISys_Movement_ProcessPath();
    endif

    return 1;

  else
    
    return AISys_Movement_ProcessPath();

  endif

endfunction

//-----------------------------------------------------------------------------

function AISysMovement_OpenDoors()

  var Npc := Self();
  var cfgElem := GetNpcConfig(Npc);

  if(cfgElem.OpensDoors)
    foreach door in ListItemsNearLocation(Npc.x, Npc.y, Npc.z, 1, Npc.realm)
      if(door.isa(POLCLASS_DOOR))
        if(!IsOpened(door) and !IsLocked(door))
          start_script(OPEN_DOOR_SCRIPT, door);
        endif
      endif
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_CanGo()

  var Res;

  case(_AI_Movement.moveType)

    AI_MOVETYPE_FLEE:              Res := (_AI_Movement.distCount <= 0);
                                   _AI_Movement.distCount -= 1 + RandomInt(2); // aby i bylo troche losowosci!

                                   if(RandomInt(100) < 40) // maly bajer, aby czasem sie zatrzymywal podczas uciekania
                                     Res := 0;
                                   endif

    AI_MOVETYPE_RUN_AWAY:          
    AI_MOVETYPE_KEEP_DISTANCE:     Res := (_AI_Movement.distCount <= 0);
                                   _AI_Movement.distCount -= 1 + RandomInt(2); // aby i bylo troche losowosci!

    default:                       Res := AISys_Movement_PrepareTrack(); // przygotujmy sciezke, jesli jest potrzebna

  endcase

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_CheckFrozen()

  if(_AI_Movement.frozenCount > AI_MOVEMENT_FROZEN_MAX_COUNT)
    return error; // przerywany chodzenie
  endif

  if(Self().frozen or Self().paralyzed)
    _AI_Movement.frozenCount += 1;
    return 1;
  else
    _AI_Movement.frozenCount := 0;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_ProcessMoving()

  // tutaj wszystko sie przetwarza

  if(AISys_Movement_IsMoving())

    _AI_Movement.waitTime := 1;

    // sprawdzmy, czy przypadkiem nie rozlecial nam sie sledzony obiekt
    if(!_AI_Movement.moveTarg)
      AISys_Movement_Stop();
      return;
    endif

    var Res := AISys_Movement_CheckFrozen();

    if(Res)
      return;
    elseif(Res == error)
      AISys_Movement_Stop();
      return;
    endif

    if(AISys_Movement_IsPaused())
      return;
    endif

    Res := AISys_Movement_DestReached();

    if(Res == error)

      AI_Log("Movement", "Moved too far! Dist: ["+CoorDist(Self().x, Self().y, _AI_Movement.moveTarg.x, _AI_Movement.moveTarg.y)+"] Moving to anchor if exists.");

    elseif(Res)

      // jesli mamy dojsc do lokacji, to zatrzymujemy movement
      // w przeciwnym wypadku czekamy az sie cel oddali by podazac za nim

      if(!_AI_Movement.moveTarg.isa(POLCLASS_UOBJECT))
        AISys_Movement_Stop(); // doszlismy do celu, konczymy.
      else
        AISys_Movement_ResetPath(); // w przeciwnym wypadku robimy reset sciezki
        AISys_Movement_BindStuck();
        _AI_Movement.distCount := AI_MOVEMENT_KEEP_DIST_COUNT;
      endif

    else

      var Res := AISys_Movement_CanGo();

      if(Res) // mozemy isc?
        
        // niech sobie otworzy drzwi, jak umie
        AISysMovement_OpenDoors();

        Res := AISys_Movement_Go();

        if(Res)
          _AI_Movement.waitTime := 0;
        endif
      endif

    endif

  endif

endfunction

//-----------------------------------------------------------------------------
