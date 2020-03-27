//-----------------------------------------------------------------------------

const TEMPORARY_STORAGE      := "Temporary";
const TEMPORARY_BOXNAME      := "TempItems";
const UOBJ_TEMPORARY_BOX     := 0xEF00; 
const UOBJ_TEMPORARY_ITEM    := 0xEF01;

//-----------------------------------------------------------------------------

function GetTemporaryBox()

  var storage := FindStorageArea( TEMPORARY_STORAGE );
  if (!storage)
    storage := CreateStorageArea( TEMPORARY_STORAGE );
  endif

  if (!storage)
    LogError("[GetTemporaryBox] Nie udalo sie stworzyc globalnego storage dla temporary!", LOGF_SCRIPTS);
    return 0;
  endif

  var tempBox := FindRootItemInStorageArea(storage, TEMPORARY_BOXNAME);

  if(!tempBox)
    tempBox := CreateRootItemInStorageArea(storage, TEMPORARY_BOXNAME, UOBJ_TEMPORARY_BOX );
    LogNotice("[GetTemporaryBox] Tworzenie nowej skrzynki miedzywymiarowej.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
  endif
  
  if(!tempBox)
    LogError("[GetTemporaryBox] Nie udalo sie stworzyc skrzyni miedzywymiarowej: "+tempBox, LOGF_SCRIPTS);
  endif
  
  return tempBox;
  
endfunction

//-----------------------------------------------------------------------------

function PullTempItem(ObjType, bMove := 1)

  var tempBox := GetTemporaryBox();

  set_critical(1);
  // zonglowanie propsami - musi byc w krytycznej sekcji, aby zapewnic brak rozpierdolu w postaci double-hard-attack by script ;)

  var lastItem := FindItemBySerial(GetObjProperty(tempBox, "#LastItem"));
  if(lastItem)
    var Prev := GetObjProperty(lastItem, "#prevItem");
    if(Prev)
      SetObjProperty(tempBox, "#LastItem", Prev);
      EraseObjProperty(lastItem, "#prevItem");
    else
      EraseObjProperty(tempBox, "#LastItem");
    endif
  else
    EraseObjProperty(tempBox, "#LastItem");
  endif
  SetObjProperty(tempBox, "#LastPull", PolCore().systime);

  if(lastItem)
    SetObjProperty(tempBox, "PullCount", CInt(GetObjProperty(tempBox, "PullCount"))+1);
    SetObjProperty(tempBox, "#CurPullCount", CInt(GetObjProperty(tempBox, "#CurPullCount"))+1);
  else
    SetObjProperty(tempBox, "CreateCount", CInt(GetObjProperty(tempBox, "CreateCount"))+1);
    SetObjProperty(tempBox, "#CurCreateCount", CInt(GetObjProperty(tempBox, "#CurCreateCount"))+1);
  endif

  set_critical(0);

  if(!lastItem)
    lastItem := CreateItemAtTempLoc(UOBJ_TEMPORARY_ITEM);
  endif

  lastItem.graphic := GetGraphicForItem(ObjType);
  lastItem.color := GetColorForItem(ObjType);
  SetName(lastItem, GetDescForItem(ObjType));
  lastItem.movable := 0;
  
  if(bMove)
    MoveObject(lastItem, CR_TEMP_ITEM_X, CR_TEMP_ITEM_Y, CR_TEMP_ITEM_Z, CR_TEMP_ITEM_REALM, MOVEOBJECT_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);
  endif

  return lastItem;

endfunction

//-----------------------------------------------------------------------------

function PullTempItemAtLoc(x, y, z, ObjType, Realm)

  var lastItem := PullTempItem(ObjType, 0);

  var Res := MoveObject(lastItem, x, y, z, Realm, MOVEOBJECT_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);
  if(!Res)
    return Res;
  endif

  return lastItem;

endfunction

//-----------------------------------------------------------------------------

function ReturnTempItemDelayed(Item, Delay)

  start_script("::ret_temp", array(Item, Delay));

endfunction

//-----------------------------------------------------------------------------

function IsTempItem(Item)

  return (Item.objtype == UOBJ_TEMPORARY_ITEM);

endfunction

//-----------------------------------------------------------------------------

function ReturnTempItem(byref Item)

  if(Item.objtype != UOBJ_TEMPORARY_ITEM)
    return 0;
  endif

  var tempBox := GetTemporaryBox();
  Item.graphic := 1;
  Item.color := 0;
  Item.movable := 1;
  
  if(!MoveItemToContainer(Item, tempBox)) // bedzie max? no to grzyb, trzeba usunac.
    DestroyItem(Item);
    return -1;
  endif

  set_critical(1);
  var curLast := GetObjProperty(tempBox, "#LastItem");

  if(curLast)
    SetObjProperty(Item, "#prevItem", curLast);
  endif

  SetObjProperty(tempBox, "#LastItem", Item.serial);
  SetObjProperty(tempBox, "#LastReturn", PolCore().systime);
  SetObjProperty(tempBox, "ReturnCount", CInt(GetObjProperty(tempBox, "ReturnCount"))+1);
  SetObjProperty(tempBox, "#CurReturnCount", CInt(GetObjProperty(tempBox, "#CurReturnCount"))+1);
  set_critical(0);
  
  Item := RetError("item returned to temporary storage"); // czyscimy referencje co by sie nic wiecej nie odwolywalo do itema!

  return 1;

endfunction

//-----------------------------------------------------------------------------
