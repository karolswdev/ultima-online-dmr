//-----------------------------------------------------------------------------

use storage;

//=============================================================================
//-----------------------------------------------------------------------------
// Finding Items
//-----------------------------------------------------------------------------
//=============================================================================

function FindItemBySerial(serial)

  serial := CInt(serial);
  if(!serial)
    return 0;
  endif

  return SystemFindObjectBySerial(serial);
  
endfunction

//--------------------------------------------------------------------------------
// Find Item In Container (ObjType or Name)
//--------------------------------------------------------------------------------

const NOCONTCHECK    := 1;
const ONLYROOT_ITEMS := 2;

//--------------------------------------------------------------------------------

function FindItemInContainer(Cont,ItemObj, Check := 0, rAmt := 0)

  if(!Cont.isa(POLCLASS_CONTAINER) and !IsBitOn(Check, NOCONTCHECK))
    return 0;
  endif

  if(!CInt(ItemObj) and ItemObj != 0)
    ItemObj := GetObjTypeByName(ItemObj);
  else
    ItemObj := Cint(ItemObj);
  endif
  
  if(!ItemObj)
    return 0;
  endif

  var Enu;

  if(IsBitOn(Check, ONLYROOT_ITEMS))
    Enu := ListRootItemsInContainer(Cont);
  else
    Enu := EnumerateItemsInContainer(Cont);
  endif
  
  foreach item in Enu
    if(item.objtype == ItemObj)
      if(rAmt)
        if(item.amount >= rAmt)
          return Item;
        endif
      else
        return item;
      endif
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function FindItemsInContainer(Cont, ItemObj, Check := 0, Color := -1)

  if(!Cont.isa(POLCLASS_CONTAINER) and !IsBitOn(Check, NOCONTCHECK))
    return 0;
  endif

  if(!CInt(ItemObj) and ItemObj != 0)
    ItemObj := GetObjTypeByName(ItemObj);
  else
    ItemObj := Cint(ItemObj);
  endif
  
  if(!ItemObj)
    return 0;
  endif

  var Enu;

  if(IsBitOn(Check, ONLYROOT_ITEMS))
    Enu := ListRootItemsInContainer(Cont);
  else
    Enu := EnumerateItemsInContainer(Cont);
  endif

  var items := array;
  
  foreach item in Enu
    if(item.objtype == ItemObj)
      // takie drobne zabezpieczenie, co by nie pomyslal o tym aby wyciagac scroille ze spellbooka ;)
      if(!item.container.isa(POLCLASS_SPELLBOOK))
        if(Color == -1 or item.color == Color)
          items.append(item);
        endif
      endif
    endif
  endforeach

  return items;

endfunction

//--------------------------------------------------------------------------------

function FindItemByNameInCont(Cont,Name)

  if(!Cont or !Name)
    return 0;
  endif

  foreach Item in EnumerateItemsInContainer(Cont)
    if(Item.desc == Name)
      return Item;
    endif
  endforeach
  return 0;

endfunction

//-----------------------------------------------------------------------------

function FindBoatBySerial(serial)

  serial := CInt(serial);
  if(!serial)
    return 0;
  endif

  return SystemFindBoatBySerial(serial);
  
endfunction

//--------------------------------------------------------------------------------

function FindItemInStorage(storage, name, objtype)

  var item := FindRootItemInStorageArea(storage, name);
  if(item)
    return item;
  endif

  item := CreateRootItemInStorageArea( storage, name, objtype );
  return item;

endfunction

//-------------------------------------------------------------------------------

function FindItemOwner(item)

  return FindPlayerBySerial(GetObjProperty(item,PROP_OWNER));
  
endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Finding Npc
//-----------------------------------------------------------------------------
//=============================================================================

function FindNpcBySerial(serial)

  serial := CInt(serial);
  if(!serial)
    return 0;
  endif

  return SystemFindObjectBySerial(serial);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Finding Player
//-----------------------------------------------------------------------------
//=============================================================================

function FindPlayerBySerial(serial)

  serial := CInt(serial);
  if(!serial)
    return 0;
  endif
  
  var Player := SystemFindObjectBySerial(serial);
  if(!Player)
    Player := SystemFindObjectBySerial(serial,SYSFIND_SEARCH_OFFLINE_MOBILES);
  endif
  
  return Player;
  
endfunction

//-----------------------------------------------------------------------------

function FindCharacterByName(name, RealNameOnly := 0)

  var lName := lower(name);

  foreach char in EnumerateOnlineCharacters()
    if((lower(char.name) == lName and !RealNameOnly) or lName == lower(GetRealName(char)))
      return char;
    endif
  endforeach

  var acct,char,i;
  foreach acctname in ListAccounts()
    acct := FindAccount(acctname);
    for(i:=1;i<=5;i:=i+1)
      char := acct.GetCharacter(i);
      if((lower(char.name) == lName and !RealNameOnly) or lName == lower(GetRealName(char)))
        return char;
      endif
    endfor
    sleepms(1);
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function FindCharIndex(who)

  for i := 1 to 5
    if(who.acct.GetCharacter(i) == who)
      return i;
    endif
  endfor

  return 0;

endfunction

//--------------------------------------------------------------------------------
