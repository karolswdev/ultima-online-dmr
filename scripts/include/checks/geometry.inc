//-----------------------------------------------------------------------------

function IsInRect(x, y, Start, End)
  
  if(x >= CInt(Start[1]) and y >= CInt(Start[2]) and x <= CInt(End[1]) and y <= CInt(End[2]))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
