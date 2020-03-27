//-----------------------------------------------------------------------------

function GetTreeConfig(ObjType)

  var cfgFile := ReadConfigFile(":lumberjacking:trees");

  return cfgFile[ObjType];

endfunction

//-----------------------------------------------------------------------------

function IsTreeObjType(Obj)

  var cfgTree := GetTreeConfig(Obj);

  if(cfgTree)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetKindOfLog(obj)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  return CInt(ItemCfg[obj].LogMod);
 
endfunction

//-----------------------------------------------------------------------------

function IsTreeNear(x, y, realm, Range)

  foreach Item in ListStaticsNearLocation(x, y, LIST_IGNORE_Z, Range, 0, realm)
    if(IsTreeObjType(Item.objtype))
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
