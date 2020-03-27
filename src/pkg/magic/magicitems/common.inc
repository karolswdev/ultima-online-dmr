//--------------------------------------------------------------------------------

function IsMagicItem(item)

  if(GetObjProperty(item,"MagicItem"))
    return 1;
  endif
  
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function IsIdentified(item)

  return Cint(GetObjProperty(item,"Identified"));
  
endfunction

//--------------------------------------------------------------------------------

function GetMagicMods(item)

  return GetObjProperty(item,"MagicItem");
  
endfunction

//--------------------------------------------------------------------------------

function GetMagicItemLevel(item)

  return Cint(GetMagicMods(item)[3]);

endfunction

//--------------------------------------------------------------------------------

function IsMagicWand(item)

  if(item.objtype >= UOBJ_WAND_START and item.objtype <= UOBJ_WAND_END)
    return 1;
  else 
    return 0;
  endif 

endfunction

//--------------------------------------------------------------------------------

function IsUndefinedMagicItem(item)

  if(!IsIdentified(item) and (IsMagicWand(item) or IsMagicItem(item)))
    return 1;
  else
    return 0;
  endif

endfunction
