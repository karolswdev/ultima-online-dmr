//-----------------------------------------------------------------------------

function FindNearestNpc(x, y, realm, range, Templates)

  var minDist := -1;
  var retNpc := error;

  foreach Obj in ListObjectsInBox(x-Range, y-Range, -127, x+Range, y+Range, 127, realm)
    if(Obj.isa(POLCLASS_NPC) and (lower(Obj.npctemplate) in Templates))
      if(minDist == -1 or coordist(x, y, Obj.x, Obj.y) > minDist)
        retNpc := Obj;
      endif
    endif
  endforeach

  return retNpc;

endfunction

//-----------------------------------------------------------------------------
