//-----------------------------------------------------------------------------

function GetBulkLevel(Item)

  var Lvl := GetObjProperty(Item, "BulkLevel");

  if(Lvl == error)
    return error;
  endif

  return CInt(Lvl);

endfunction

//-----------------------------------------------------------------------------
