//-----------------------------------------------------------------------------

function PutRuneInBag (bag, rune_objtype)

  var rune_array := GetObjProperty (bag, "runes");
  if (!rune_array or !rune_array.size())
    rune_array := {};
  endif

  foreach rune in rune_array
    if (cint(rune) == cint(rune_objtype) )
      return 0;
    endif
  endforeach

  rune_array.append (rune_objtype);
  SetObjProperty (bag, "runes", rune_array);
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function TakeRuneOutOfBag (bag, rune_objtype)

  var rune_array := GetObjProperty (bag, "runes");
  if (!rune_array)
    return 0;
  endif

  var new_rune_array := {};

  foreach rune in rune_array
    if (rune and cint(rune) != cint (rune_objtype))
      new_rune_array.append (rune);
    endif
  endforeach

  SetObjProperty (bag, "runes", new_rune_array);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckForRuneInBag (bag, rune_objtype)

  if(!GetObjProperty (bag, "runes"))
    return 0;
  endif
  
  if(rune_objtype in GetObjProperty (bag, "runes"))
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------
