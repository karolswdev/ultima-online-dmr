//-----------------------------------------------------------------------------
// uzywany w GetMobileClass
//-----------------------------------------------------------------------------

const POLCLASS_PLAYER        := 100;

//=============================================================================
//-----------------------------------------------------------------------------
// Mobile Basics
//-----------------------------------------------------------------------------
//=============================================================================

function GetCreatedAt(who)

  if(CInt(GetObjProperty(who,PROP_CREATEDAT)))
    return CInt(GetObjProperty(who,PROP_CREATEDAT));
  else
    return who.createdat;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMobileClass(Obj)

  if(Obj.isa(POLCLASS_CORPSE))

    // jesli ma propa npctemplate, sa to zwloki npeca
    if(GetObjProperty(Obj, PROP_NPCTEMP))
      return POLCLASS_NPC;
    else
      return POLCLASS_PLAYER;
    endif

  elseif(Obj.isa(POLCLASS_NPC))

    return POLCLASS_NPC;

  elseif(IsPlayer(Obj))

      return POLCLASS_PLAYER;

  elseif(Obj.isa(POLCLASS_ITEM))

    return POLCLASS_ITEM;

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
