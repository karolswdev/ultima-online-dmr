//-----------------------------------------------------------------------------

include ":itemsets:shared";

//-----------------------------------------------------------------------------

function GetIsetCtrlIds()

  var Data := GetIsetCtrlData();
  var Keys := Data.Keys();

  var gIdx := RetIndex(Keys, ISC_ELEM_GLOBAL);

  if(gIdx)
    Keys.erase(gIdx);
  endif

  return Keys;

endfunction

//-----------------------------------------------------------------------------

function GetIsetCtrlNextId()

  var Data := GetIsetCtrlData();

  var gElem := Data.FindElement(ISC_ELEM_GLOBAL);

  if(!gElem)
    gElem := Data.CreateElement(ISC_ELEM_GLOBAL);
  endif

  var NextId := CInt(gElem.GetProp("NextId"))+1;

  gElem.SetProp("NextId", NextId);

  return NextId;

endfunction

//-----------------------------------------------------------------------------

function AddItemSetControl(who, Set, Items)

  var CtrlData := GetIsetCtrlData();

  if(!CtrlData)
    return CtrlData;
  endif

  var NextId   := GetIsetCtrlNextId();

  if(!NextId)
    return NextId;
  endif

  var newElem  := CtrlData.CreateElement(CStr(NextId));

  if(!newElem)
    return newElem;
  endif

  foreach Item in Items
    SetObjProperty(Item, PROP_ISET_CTRL_ID, NextId);
    Item := Item.serial;
  endforeach

  newElem.SetProp("SetName", Set.Name);
  newElem.SetProp("Items", Items);
  newElem.SetProp("Creator", who.acctname);
  newElem.SetProp("AddTime", PolCore().systime);
  newElem.SetProp("AddGameTime", ReadGameClock());
  newElem.SetProp("ItemCount", Items.size());

  return nextId;

endfunction

//-----------------------------------------------------------------------------

function MoveIsetCtrlToPack(who, ctrlId)

  var cElem := GetIsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola IS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Items := cElem.GetProp("Items");
  var Item;
  var Res := struct;
  Res.+rootCount := 0;
  Res.+itemCount := 0;
  Res.+nexCount  := 0;

  // na koncu beda root itemy
  Items.reverse();

  foreach ItemSer in Items

    Item := FindItemBySerial(ItemSer);

    if(!Item)
      Res.nexCount += 1;
      continue;
    endif

    if(GetTopCont(Item) != who.backpack)

      var itmRes := MoveToPack(Item, who);

      if(!itmRes)
        return error{"errortext" := "Nie udalo sie przeniesc przedmiotu: "+GetName(Item)+"/"+hex(Item.serial)+"! Zbadaj w czym problem! Res: "+itmRes.errortext};
      endif

      Res.rootCount += 1;

    endif

    EraseObjProperty(Item, PROP_ISET_CTRL_ID);
    
    if(!IsGmCreated(Item))
      SetPropCreated(who, Item);
    endif
    Res.itemCount += 1;

  endforeach

  var rmRes := RemoveIsetCtrl(ctrlId);

  if(!rmRes)
    return rmRes;
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function CheckIsetCtrl(cElem)
  
  var Item;
  var Items := cElem.GetProp("Items");

  foreach ItemSer in Items

    Item := FindItemBySerial(ItemSer);

    if(Item)
      if(!IsGmCreated(Item))
        return error{"errortext" := "Przedmiot "+GetName(Item)+"/"+hex(Item.serial)+" nie zostal oznaczony flaga stworzyciela i ciagle istnieje!"};
      endif
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteIsetCtrlItems(ctrlId)

  var cElem := GetIsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola IS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Res  := struct;

  Res.+itemCount := 0;
  Res.+nexCount  := 0;
  
  var Items := cElem.GetProp("Items");

  foreach ItemSer in Items

    var Item := FindItemBySerial(ItemSer);

    if(Item)
      if(!DestroyItem(Item))
        return error{"errortext" := "Nie udalo sie usunac przedmiotu "+GetName(Item)+"/"+hex(Item.serial)+"! Zbadaj dlaczego."};
      endif

      Res.itemCount += 1;

    else

      Res.nexCount += 1;

    endif

  endforeach

  return Res;

endfunction

//-----------------------------------------------------------------------------

function RemoveIsetCtrl(ctrlId)

  var cElem := GetIsetCtrlElem(ctrlId);

  if(!cElem)
    return error{"errortext" := "Kontrola IS o ID "+ctrlId+" nie istnieje!"};
  endif

  var Data := GetIsetCtrlData();

  var cRes := CheckIsetCtrl(cElem);
  if(!cRes)
    return cRes;
  endif

  var dRes := Data.DeleteElement(CStr(ctrlId));

  if(!dRes)
    return error{"errortext" := "Nie udalo sie usunac IS kontroli! Blad: ["+dRes+"] Powiadom o tym administratora."};
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------
