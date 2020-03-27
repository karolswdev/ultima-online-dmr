//-----------------------------------------------------------------------------

include ":itemsets:shared";

//-----------------------------------------------------------------------------

function CheckItemSetControl(Item)

  if(GetObjProperty(Item, PROP_ISET_CTRL_ID) == error)
    return 1;
  endif

  return CheckItemSetExistance(Item);

endfunction

//-----------------------------------------------------------------------------

function CheckItemSetExistance(Item)

  var ctrlId := CInt(GetObjProperty(Item, PROP_ISET_CTRL_ID));

  var cElem  := GetIsetCtrlElem(ctrlId);

  if(!cElem)
    LogError("Znaleziono przedmiot, ktorego IS-Ctrl nie istnieje, a ma przypisany ID! "+GetName(Item)+"/"+hex(Item.objtype)+" CtrlID: "+ctrlId, LOGF_SCRIPTS);
    return 0;
  endif

  var Items := cElem.GetProp("Items");
  var myIdx := RetIndex(Items, Item.serial);

  if(!myIdx)
    Items.append(Item.serial);
    cElem.SetProp("Items", Items);
    cElem.SetProp("ItemCount", Items.size());
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
