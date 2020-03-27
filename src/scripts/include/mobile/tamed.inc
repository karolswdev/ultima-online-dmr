//-----------------------------------------------------------------------------

function GetMaster(Obj)

  if(Obj.master)
    return Obj.master;
  elseif(Obj.master.serial)
    return FindPlayerBySerial(Obj.master.serial);
  else
    return FindPlayerBySerial(GetObjProperty(Obj, PROP_MASTER));
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMasterSerial(critter)

  if(critter.master.serial)
    return critter.master.serial;
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function IsSleeping(Npc)

  return CInt(GetObjProperty(Npc,"Sleeping"));

endfunction

//-----------------------------------------------------------------------------
