//-----------------------------------------------------------------------------

const OPEN_DOOR_SCRIPT       := ":doors:opendoor";

//-----------------------------------------------------------------------------

function IsOpened(door)

  return CInt(GetObjProperty(door,"IsOpened"));

endfunction

//-----------------------------------------------------------------------------

function IsLocked(door)

  return door.locked;

endfunction

//-----------------------------------------------------------------------------
