//================================================================================
//--------------------------------------------------------------------------------
// Creation / Destruction
//--------------------------------------------------------------------------------
//================================================================================

function VerifyObjType(byref objtype)

  var Script := GetProcess(GetPid());
  var Controller := Script.controller;

  if(TypeOf(objtype) != "Struct")

    if(!CInt(objtype) and objtype)
      objtype := GetObjTypeByName(objtype);
    endif

    objtype := CInt(objtype);

    if(IsStaff(Controller))
      var iDesc := GetItemDescriptor(objtype);

      if(iDesc)
        objtype := iDesc;
      endif
    endif

  endif

  if(IsStaff(Controller))
    objtype.CProps[PROP_CREATED] := Controller.serial;
  endif

endfunction

//--------------------------------------------------------------------------------

function VerifyItemCreation(Item)

  var Script := GetProcess(GetPid());
  var Controller := Script.controller;

  if(IsStaff(Controller) and !IsGmCreated(Item))
    SetPropCreated(Controller, Item);
  endif

endfunction

//--------------------------------------------------------------------------------

function CreateItemAtLoc( x, y, z, objtype, amount := 1, realm := _DEFAULT_REALM)

  VerifyObjType(objtype);

  x       := CInt(x);
  y       := CInt(y);
  amount  := CInt(amount);
  
  if(!x or !y or z == error or !ObjType or !amount)
    return error;
  endif
  
  z := CInt(z);
  
  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif
  
  var Item := CreateItemAtLocation(x,y,z,objtype,amount,realm);

  VerifyItemCreation(Item);

  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateItemInPack(who, objtype, amount := 1)

  VerifyObjType(objtype);

  amount  := CInt(amount);

  if(!who.isa(POLCLASS_MOBILE) or !objtype or !amount)
    return error;
  endif

  var Item := CreateItemInBackPack(who, objtype, amount);
  
  if(!Item)
    SendSysMessage(who,"Nie masz juz miejsca w plecaku.",FONT_NORMAL,COLOR_RED);
    return CreateItemAtLoc(who.x,who.y,who.z,objtype,amount,who.realm);
  endif
  
  VerifyItemCreation(Item);

  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateItemInCont( container, objtype, amount := 1, who := 0)

  VerifyObjType(objtype);

  amount  := CInt(amount);

  if(!objtype)
    return error{"errortext" := "Bledny objtype: "+objtype};
  endif

  if(amount <= 0)
    return error{"errortext" := "Nieprawidlowa ilosc: "+amount};
  endif

  if(!container.isa(POLCLASS_CONTAINER))
    return error{"errortext" := "Przedmiot serial: "+container.serial+" nie jest pojemnikiem!"};
  endif

  var Item := CreateItemInContainer(container,objtype,amount);

  if(!Item)
    if(who)
      SendSysMessage(who,"W pojemniku nie ma juz miejsca.",FONT_NORMAL,COLOR_RED);
      return CreateItemAtLoc(who.x,who.y,who.z,objtype,amount,who.realm);
    endif
  endif
  
  VerifyItemCreation(Item);

  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateItemInInv( container, objtype, amount := 1 )

  VerifyObjType(objtype);

  amount  := CInt(amount);

  if(!container.isa(POLCLASS_CONTAINER) or !objtype or !amount)
    return error;
  endif
  
  var Item := CreateItemInInventory(container,objtype,amount);
  
  VerifyItemCreation(Item);

  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateInvisItem( x, y, z, objtype, realm)

  var Item := CreateItemAtLoc (x, y, z, UOBJ_INVISITEM, 1, Realm);

  if(Item and objtype)
    Item.graphic := objtype;
    SetName(Item,GetDescForItem(objtype));
    Item.invisible := 0;
  endif
  
  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateItemCopyAtLoc( x, y, z, Item, realm := _DEFAULT_REALM)

  if(!Item.isa(POLCLASS_ITEM))
    return 0;
  endif

  x       := CInt(x);
  y       := CInt(y);
  
  if(!x or !y or z == error)
    return error;
  endif
  
  z := CInt(z);
  
  if(!Realm)
    Realm := _DEFAULT_REALM;
  endif
  
  var newItem := CreateItemCopyAtLocation(x, y, z, Item, realm);
  
  VerifyItemCreation(newItem);

  return newItem;

endfunction

//--------------------------------------------------------------------------------

function CreateItemAtTempLoc(objtype, Amt := 1)

  return CreateItemAtLoc(CR_TEMP_ITEM_X, CR_TEMP_ITEM_Y, CR_TEMP_ITEM_Z, objtype, Amt, CR_TEMP_ITEM_REALM);

endfunction

//--------------------------------------------------------------------------------

function DestroyItemArray(byref Arr)

  foreach item in Arr
    if(item.isa(POLCLASS_ITEM))
      DestroyItem(item);
    endif
  endforeach

endfunction

//-------------------------------------------------------------------------
// Checks
//-------------------------------------------------------------------------

function SetPropCreated(who,item)

  EraseObjProperty(Item, PROP_QUEST_ITEM);
  SetObjProperty(item, PROP_CREATED, who.serial);

endfunction

//-----------------------------------------------------------------------------

function SetPropQuestItem(who,item)

  SetObjProperty(item, PROP_QUEST_ITEM, who.serial);

endfunction

//-----------------------------------------------------------------------------

function RemovePropQuestItem(Item)

  EraseObjProperty(item, PROP_QUEST_ITEM);

endfunction

//-----------------------------------------------------------------------------

function ClearScriptObjCreator()

  SetScriptController(0);

endfunction

//-----------------------------------------------------------------------------

function SetScriptObjCreator(who)

  // taka zaslepka, jakby cos trzeba bylo jeszcze dodac
  SetScriptController(who); 

endfunction

//-----------------------------------------------------------------------------

function IsGmCreated(Obj)

    return GetObjProperty(Obj, PROP_CREATED) != error;

endfunction

//-----------------------------------------------------------------------------

function IsQuestItem(Obj)

    return GetObjProperty(Obj, PROP_QUEST_ITEM) != error;

endfunction

//-------------------------------------------------------------------------

function GetObjCreator(Obj)

    return FindPlayerBySerial(GetObjProperty(Obj, PROP_CREATED));

endfunction

//-------------------------------------------------------------------------

function GetObjQuestMaster(Obj)

    return FindPlayerBySerial(GetObjProperty(Obj, PROP_QUEST_ITEM));

endfunction

//-------------------------------------------------------------------------

function RemoveObjCreator(Obj)

  EraseObjProperty(Obj, PROP_CREATED);
  EraseObjProperty(Obj, PROP_QUEST_ITEM);

endfunction

//-------------------------------------------------------------------------

function HasGmCreated(who, Obj)

  if(IsGmCreated(Obj))
    if(GetObjProperty(Obj, PROP_CREATED) == who.serial)
      return 1;
    endif

    return 0;
  endif

  return -1;

endfunction

//--------------------------------------------------------------------------------
