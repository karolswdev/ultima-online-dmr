//-----------------------------------------------------------------------------

function IsInPvpAreaLoc(x,y,realm)
  
  var RegName := GetRegionNameAtLocation( x, y, realm );

  return find(RegName, "PvpArea", 1);

endfunction

//-----------------------------------------------------------------------------
