//-----------------------------------------------------------------------------
// Modified: 2005-05-04
//-----------------------------------------------------------------------------

include "common";

//-----------------------------------------------------------------------------

function SetItemStc(Item,who := 0)

  if(!Item or !Item.isa(POLCLASS_ITEM))
    return 0;
  endif

  var Res := CanBeStcItem(Item);
  if(!Res)
    if(who)
      SendSysMessage(who, Res.errortext, FONT_NORMAL, COLOR_RED);
    endif
    return Res;
  endif

  Item.movable := 0;
  
  if(who.acctname)
    SetObjProperty(Item,"Static",who.acctname);
  else
    SetObjProperty(Item,"Static",1);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DelItemStc(Item)

  if(!Item)
    return 0;
  endif

  if(!IsStc(Item))
    return 0;
  endif
  
  EraseObjProperty(Item,"Static");
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function CanBeStcItem(Item)

  if(!Item.isa(POLCLASS_ITEM))
    return RetError("To nie przedmiot.");
  endif

  if(IsStc(Item))
    return RetError("To jest juz Static.");
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
