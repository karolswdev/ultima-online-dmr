//-----------------------------------------------------------------------------

function CheckNecroSpellInBook(necrobook,SpellID)

  if(necrobook.objtype != UOBJ_NECROBOOK)
    return 0;
  endif
  
  SpellID := CInt(SpellID);
  
  var nSpells := 0;
  
  if(SpellID > 64 and SpellID <= 72)
    nSpells := CInt(GetObjProperty(necrobook,"FirstNecroSpells"));
    SpellID := SpellID - 64;
  else
    nSpells := CInt(GetObjProperty(necrobook,"SecNecroSpells"));
    SpellID := SpellID - 72;
  endif
  
  if(!nSpells)
    return 0;
  else
    if((nSpells & CInt(Pow(2,SpellID-1))) > 0)
      return 1;
    endif
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function AddSpellToNecroBook(necrobook,SpellID)

  var nSpells := 0,Flag;

  if(SpellID > 64 and SpellID <= 72)
    Flag    := "FirstNecroSpells";
    SpellID := SpellID - 64;
  elseif(SpellID > 72 and SpellID <= 80)
    Flag    := "SecNecroSpells";
    SpellID := SpellID - 72;
  else
    return 0;
  endif
  
  nSpells := CInt(GetObjProperty(necrobook,Flag));
  
  nSpells := nSpells | CInt(Pow(2,SpellID-1));
  SetObjProperty(necrobook, Flag, nSpells);

  return 1;

endfunction

//-----------------------------------------------------------------------------
