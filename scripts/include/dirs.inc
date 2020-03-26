//-----------------------------------------------------------------------------

include "include/client/misc";

//=============================================================================
//-----------------------------------------------------------------------------
// Directions
//-----------------------------------------------------------------------------
//=============================================================================

function GetDirFromObjToObj(FromObj,ToObj)

  return GetDirFromLocToLoc(FromObj.x,FromObj.y,ToObj.x,ToObj.y);

endfunction

//-----------------------------------------------------------------------------

function GetDirName(Dir)

  case(Dir)
    DIR_SE:    return "SE";
    DIR_NE:    return "NE";
    DIR_SW:    return "SW";
    DIR_NW:    return "NW";
    DIR_N:     return "N";
    DIR_S:     return "S";
    DIR_W:     return "W";
    DIR_E:     return "E";
    default:   return "";
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetDirFromLocToLoc(Fx,Fy,Tx,Ty)

  if(!Fy or !Fx or !Ty or !Tx)
    return error;
  endif
  
  if(Tx > Fx and Ty > Fy)
    return DIR_SE;
  elseif(Tx > Fx and Ty < Fy)
    return DIR_NE;
  elseif(Tx < Fx and Ty > Fy)
    return DIR_SW;
  elseif(Tx < Fx and Ty < Fy)
    return DIR_NW;
  elseif(Tx == Fx and Ty > Fy)
    return DIR_S;
  elseif(Tx == Fx and Ty < Fy)
    return DIR_N;
  elseif(Tx < Fx and Ty == Fy)
    return DIR_W;
  elseif(Tx > Fx and Ty == Fy)
    return DIR_E;
  endif
  
  return error;

endfunction

//-----------------------------------------------------------------------------

function IsFacingObject(Mob, Obj, Dir)

  var MobDir := GetDirFromObjToObj(Mob, Obj);

  if(MobDir == Dir)
    return 1;
  endif

  var DirLeft := (Dir - 1);
  var DirRight := (Dir + 1);

  if(DirLeft < 0)
    DirLeft += DIR_MAX;
  endif

  if(DirRight > DIR_MAX)
    DirRight -= DIR_MAX;
  endif

  // dajemy mozliwosc skosnej walidacji 
  // - jesli mob bedzie na skosie z jednej lub drugiej strony, to tez facinguje w nasz obiekt!
  if(MobDir == DirLeft or MobDir == DirRight)
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetDirDesc(Dir)

  case(Dir)
    DIR_SE:    return "poludniowy wschod";
    DIR_NE:    return "polnocny wschod";
    DIR_SW:    return "poludniowy zachod";
    DIR_NW:    return "polnocny zachod";
    DIR_N:     return "polnoc";
    DIR_S:     return "poludnie";
    DIR_W:     return "zachod";
    DIR_E:     return "wschod";
    default:   return "";
  endcase

endfunction

//-----------------------------------------------------------------------------

function TurnCharToward(who,Obj)

  if(Obj.isa(POLCLASS_UOBJECT))
    var DirTo := GetDirFromObjToObj(who,Obj);

    if(DirTo != error)
      if(who.facing != DirTo)
        who.facing := DirTo;
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function TurnCharTowardLocation(who,Loc)

  if(Loc.x and Loc.y)
    var DirTo := GetDirFromObjToObj(who,Loc);
    if(DirTo != error)
      if(who.facing != DirTo)
        who.facing := DirTo;
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GetReverseDir(Dir)

  case(Dir)
    DIR_SE:    return DIR_NW;
    DIR_NE:    return DIR_SW;
    DIR_SW:    return DIR_NE;
    DIR_NW:    return DIR_SE;
    DIR_N:     return DIR_S;
    DIR_S:     return DIR_N;
    DIR_W:     return DIR_E;
    DIR_E:     return DIR_W;
    default:   return error;
  endcase
  
endfunction

//-----------------------------------------------------------------------------

function GetOneDirBack(Dir)

  if(Dir == DIR_N)
    return DIR_NW;
  elseif(Dir <= DIR_NW and Dir >= DIR_N)
    return Dir - 1;
  endif
  
  return -1;

endfunction

//-----------------------------------------------------------------------------

function GetOneDirForward(Dir)

  if(Dir == DIR_NW)
    return DIR_N;
  elseif(Dir <= DIR_NW and Dir >= DIR_N)
    return Dir + 1;
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function GetDirLocMods(Dir)

  case(Dir)
    DIR_SE:    return {1,1};
    DIR_NE:    return {1,-1};
    DIR_SW:    return {-1,1};
    DIR_NW:    return {-1,-1};
    DIR_N:     return {0,-1};
    DIR_S:     return {0,1};
    DIR_W:     return {-1,0};
    DIR_E:     return {1,0};
  endcase
  
  return {0,0};

endfunction

//-----------------------------------------------------------------------------
