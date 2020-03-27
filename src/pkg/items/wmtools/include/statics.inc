//-----------------------------------------------------------------------------

include ":baseitems:common";

//-----------------------------------------------------------------------------

function IsStaticAtLocation(Loc, ObjType)

  var Statics := ListStaticsAtLocation(Loc.x, Loc.y, LIST_IGNORE_Z, 0, Loc.realm);

  foreach Static in Statics
    if(Static.objtype == ObjType)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetStaticName(ObjType)

  var cfgItem := GetItemConfig(ObjType);

  if(cfgItem.desc)
    return cfgItem.desc;
  elseif(cfgItem.name)
    return cfgItem.name;
  else
    cfgItem := GetTileConfig(ObjType);

    if(cfgItem.Desc)
      return cfgItem.Desc;
    endif
  endif

  return "<unknown>";

endfunction

//-----------------------------------------------------------------------------

function CanBeStaticItem(Item)

  if(!Item.isa(POLCLASS_ITEM))
    return RetError("To nie przedmiot.");
  endif

  if(GetItemBINr(Item))
    return RetError("Ten przedmiot to BI.");
  endif
  
  if(Item.container)
    return RetError("Przedmiot znajduje sie w pojemniku.");
  endif

  if(CheckIsHouseItem(Item))
    return RetError("Przedmiot znajduje sie w multi (domu).");
  endif
  
  var ItemCfg   := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[Item.objtype].CantBeBase)
    return RetError("Ten przedmiot nie moze byc BI.");
  endif

  if(Item.objtype > 0x7FFF)
    return RetError("Za duzy ObjType, > 0x7FFF");
  endif

  if(GetObjProperty(Item, "Spawnnet"))
    return RetError("Ten przedmiot jest ze spawnu.");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
