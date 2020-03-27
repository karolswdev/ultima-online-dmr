//--------------------------------------------------------------------------------

include "include/items/containers";

//================================================================================
//--------------------------------------------------------------------------------
// Item Uses
//--------------------------------------------------------------------------------
//================================================================================

const PROP_MAXITEM_USES      := "MaxIUses";

//--------------------------------------------------------------------------------

function GetSumItemsUses(byref Items)

  var Sum := 0;

  foreach Item in Items
    Sum += GetItemUses(Item);
  endforeach

  return Sum;

endfunction

//--------------------------------------------------------------------------------

function GetItemUses(Item)

  if(!Item)
    return 0;
  endif

  if(IsStackableObj(Item.objtype))
    return Item.amount;
  endif

  var Uses := GetObjProperty(Item, PROP_MAXITEM_USES);

  if(!Uses)
    var iCfg := ReadConfigFile(ITEMS_CFG);
    Uses := CInt(iCfg[Item.objtype].MaxUses);
    if(Uses <= 1)
      return 1;
    endif
    SetObjProperty(Item, PROP_MAXITEM_USES, Uses);
  endif

  return Uses;

endfunction

//--------------------------------------------------------------------------------

function SubItemUses(Item, Amt := 1)

  if(!Item.isa(POLCLASS_ITEM))
    return 0;
  endif

  Amt := CInt(Amt);

  if(IsStackableObj(Item.objtype))
    var Empty := CreateEmptyItem(Item, Amt);

    if(!SubtractAmount(Item, Amt))
      if(Empty)
        DestroyItem(Empty);
      endif
      return 0;
    endif

    return 1;
  endif

  var Uses := GetObjProperty(Item, PROP_MAXITEM_USES);
  var iCfg := ReadConfigFile(ITEMS_CFG);

  if(!Uses)
    Uses := CInt(iCfg[Item.objtype].MaxUses);
    if(!Uses)
      Uses := 1;
    endif
  endif

  if(Uses - Amt < 0)
    return 0;
  endif

  Uses := Uses - Amt;

  if(Uses <= 0)
    var Empty := CreateEmptyItem(Item, 1);

    if(!DestroyItem(Item))
      if(Empty)
        DestroyItem(Empty);
      endif
      return 0;
    endif
  else
    return SetItemUses(Item, Uses);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function CreateEmptyItem(Item, Amt)

  var iCfg  := ReadConfigFile(ITEMS_CFG);
  var Empty := iCfg[Item.objtype].EmptyItem;

  if(Empty)
    if(Item.container)
      Empty := CreateItemInCont(Item.container, Empty, Amt);
      if(!Empty)
        var TopCont := GetTopCont(Item);
        Empty := CreateItemAtLoc(TopCont.x, TopCont.y, TopCont.z, Empty, Amt, TopCont.realm);
      endif
    else
      Empty := CreateItemAtLoc(Item.x, Item.y, Item.z, Empty, Amt, Item.realm);
    endif

    if(!Empty)
      return 0;
    endif
  endif

  return Empty;

endfunction

//--------------------------------------------------------------------------------

function GetItemMaxUses(Item)

  if(IsStackableObj(Item.objtype))
    return Item.amount;
  endif

  var iCfg := ReadConfigFile(ITEMS_CFG);
  
  if(!iCfg[Item.objtype].MaxUses)
    return 1;
  endif

  return CInt(iCfg[Item.objtype].MaxUses);

endfunction

//--------------------------------------------------------------------------------

function GetObjMaxUses(Obj)

  var iCfg := ReadConfigFile(ITEMS_CFG);
  
  if(!iCfg[Obj].MaxUses)
    return 1;
  endif

  return CInt(iCfg[Obj].MaxUses);

endfunction

//--------------------------------------------------------------------------------

function AddItemUses(Item, Amt)

  if(IsStackableObj(Item.objtype))
    return AddAmount(Item, Amt);
  endif

  var Uses := GetItemUses(Item);

  if(Uses + Amt > GetItemMaxUses(Item))
    return 0;
  endif

  return SetObjProperty(Item, PROP_MAXITEM_USES, Uses + Amt);

endfunction

//--------------------------------------------------------------------------------

function GetPortionDesc(Item, Amt)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var cfgItem := ItemCfg[Item.objtype];

  if(cfgItem.Hunger != error or cfgItem.Protein != error or cfgItem.Vitamin != error or cfgItem.Sugar)
    return "porcj"+Gam(Amt, GAM_I);
  elseif(Amt == 1)
    return "uzycie";
  elseif(Amt < 5)
    return "uzycia";
  elseif(Amt)
    return "uzyc";
  endif

endfunction

//--------------------------------------------------------------------------------

function SetItemUses(Item, Uses)

  if(!IsStackableObj(Item.objtype))
    var Name := GetName(Item);
    var aStr := FindStrInBracets(Name, 1);
    if(aStr)
      Name[aStr] := "("+Uses+" "+GetPortionDesc(Item, Uses)+")";
    else
      Name := Name + " ("+Uses+" "+GetPortionDesc(Item, Uses)+")";
    endif
    SetName(Item, Name);
    return SetObjProperty(Item, PROP_MAXITEM_USES, ToMax(Uses, GetItemMaxUses(Item)));
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------
