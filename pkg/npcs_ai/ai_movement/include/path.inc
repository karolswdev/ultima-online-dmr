//-----------------------------------------------------------------------------

function AISys_Movement_GetPath(toX, toY)

  var toZ := GetStandZ(toX ,toY, Self().realm, GSF_GETMAPZ);

  if(toZ == error)
    AI_LogError("Movement", "GetPath: Lokacja ["+toX+","+toY+" "+Self().realm+"] jest niedostepna z: " + toZ);
    return error;
  endif

  var simplePath := 1;

  if(!CheckLosAt(Self(), toX, toY, toZ))
    simplePath := 0;
  elseif(!CheckSpecialLosAtPos(Self().x, Self().y, Self().z, Self().realm, toX, toY, toZ, Self().realm))
    simplePath := 0;
  endif

  var Path;
  
  if(simplePath)
    Path := GetPathFromLoc(Self().x, Self().y, Self().z, toX, toY, toZ, Self().realm);
  endif

  if(!Path)
    Path := FindPath(Self().x, Self().y, Self().z, toX, toY, toZ, Self().realm, 0, 25);
  endif

  return Path;

endfunction

//-----------------------------------------------------------------------------
