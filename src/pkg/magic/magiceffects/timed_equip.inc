//-----------------------------------------------------------------------------

function IsTimedEquipItem(Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  return CInt(ItemCfg[Item.objtype].TimedEquip);

endfunction

//-----------------------------------------------------------------------------

function CheckTimedEquipItem(who, Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  if(!ItemCfg[Item.objtype].TimedEquip)
    return 0;
  endif

  start_script(":magiceffects:timed_equip", array(who, Item));
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckTimedUnEquipItem(who, Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  if(!ItemCfg[Item.objtype].TimedEquip)
    return 0;
  endif

  var Mod := ItemCfg[Item.objtype].ModType;

  if(Mod)
    Mod := SplitWords(Mod);
    UndoMod(who, CInt(Mod[1]));
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
