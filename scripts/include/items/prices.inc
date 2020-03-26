//-----------------------------------------------------------------------------

include ":magicitems:common";
include "include/items/made";
include "include/objtypes/skills";
include "include/getobj";
include ":crafting:mat_groups";

//-----------------------------------------------------------------------------

function GetItemBuyPrice(Item, RepLvl := 0)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Cost    := CInt(ItemCfg[Item.objtype].VendorBuysFor);
  var i;

  if(item.quality)
    cost := cost * item.quality;
  endif

  if(IsMagicItem(Item))
    if(IsIdentified(item))
      i := Cint((i * GetMagicItemLevel(item)) * 2);
    endif
  else
    var BaseElem := 0;
    var ItemElem := GetMatItemCfgFromMade(Item, BaseElem);

    if(ItemElem)
      cost := CInt(CDbl(cost) * Max((CDbl(ItemElem.VendorBuysFor) / Max(CDbl(BaseElem.VendorBuysFor), 1.0)) * 0.8, 1.0));
    else
      var Perc := GetItemMakeModPerc(Item);
      cost += (cost * 5 * Perc);
    endif
  endif

  if(RepLvl)
    RepLvl := RepLvl * 3;
    cost := cost * ((100 + RepLvl) * 0.01);
  endif

  if(item.hp and item.maxhp)
    i := ((item.hp * 10) / item.maxhp);
    cost := CInt((cost * i) / 10);
  endif

  return CInt(cost);

endfunction

//-------------------------------------------------------------------------------

function GetItemSellPrice(Item, RepLvl := 0)

  __ParamUnused(RepLvl);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  return CInt(ItemCfg[Item.objtype].VendorSellsFor);

endfunction

//-----------------------------------------------------------------------------

function CountItemsSellPrice(Items, RepLvl := 0)

  var Price := 0, Amt;
  
  foreach Item in Items
    Amt  := CInt(Item[2]);
    Item := FindItemBySerial(Item[1]);
    if(Item)
      Price := Price + (GetItemSellPrice(Item, RepLvl) * Amt);
    endif
  endforeach
  
  return Price;

endfunction

//-----------------------------------------------------------------------------
