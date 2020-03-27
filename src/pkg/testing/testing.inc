//-----------------------------------------------------------------------------

function CanInteract(Tester, Obj)

  var Serial;

  if(Obj.isa(POLCLASS_NPC))
    Serial := CInt(GetObjProperty(Obj, PROP_OWNER));
    if(IsTamed(Obj))
      if(Serial != Tester.serial and GetMaster(Obj) != Tester)
        return 0;
      endif
    endif
    return 1; // na innych npecach moze.
  elseif(Obj.isa(POLCLASS_ITEM))
    Serial := CInt(GetObjProperty(Obj, PROP_CREATED));
    if(!Serial or Serial == Tester.serial)
      return 1;
    endif
  elseif(Tester == Obj)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
