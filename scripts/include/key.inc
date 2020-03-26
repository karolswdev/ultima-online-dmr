//--------------------------------------------------------------------------------

include "include/getobj";

//================================================================================
//--------------------------------------------------------------------------------
// Keys
//--------------------------------------------------------------------------------
//================================================================================

function AllocLockId()
  var lockid := GetGlobalProperty( "nextlockid" );
  if (!lockid)
    lockid := 1;
  endif
  SetGlobalProperty( "nextlockid", lockid+1 );
  return lockid;
endfunction

//--------------------------------------------------------------------------------

function FindKey(who, what)

  var lockid := GetObjProperty( what, "lockid" );
  if(!lockid)
    return 0;
  endif

  foreach item in EnumerateItemsInContainer( who.backpack )
    if(item.graphic >= UOBJ_KEYSTART and item.graphic <= UOBJ_KEYEND)
      if(GetObjProperty( item, "lockid" ) == lockid)
        return item;
      endif
    endif
    sleepms(2);
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function SetLock(item,diff)

  SetObjProperty(Item, "MLockPickDiff", diff);
  Item.locked := 1;
    
endfunction

//--------------------------------------------------------------------------------

function SetItemLocked(item,key,diff)

  var lockid := AllocLockId();
  SetObjProperty(key, "lockid",lockid);
  SetObjProperty(item,"lockid",lockid);
  SetLock(item,diff);
  
endfunction

//--------------------------------------------------------------------------------

function SetItemLockID(byref item,lockid)

  SetObjProperty(item,"lockid",lockid);
  
endfunction

//--------------------------------------------------------------------------------

function GetItemLockID(item)

  return CInt(GetObjProperty(item,"lockid"));

endfunction

//--------------------------------------------------------------------------------

function IsKeyRingLockID(KeyRing,lockid)

  foreach KeyProps in GetObjProperty(KeyRing,"KeysInside")
    if(KeyProps[2] == lockid)
      return 1;
    endif
  endforeach
  
  return 0;

endfunction

//--------------------------------------------------------------------------------

function EraseItemLockID(item)

  EraseObjProperty(item,"lockid");
  item.locked := 0;
  
endfunction

//--------------------------------------------------------------------------------

function UnLockItem(item)

  Item.locked := 0;

endfunction

//--------------------------------------------------------------------------------

function GetLockDiff(Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  return Cint(ItemCfg[Item.objtype].LockPickDiff);
  
endfunction

//--------------------------------------------------------------------------------

function GetTreasureLvl(Item)

  return Cint(GetObjProperty(item,"level"));

endfunction

//--------------------------------------------------------------------------------

function SetTreasureLvl(Item,lvl)

  SetObjProperty(item,"level",cint(lvl));

endfunction

//--------------------------------------------------------------------------------

function EraseTreasureLvl(Item)

  EraseObjProperty(item,"level");

endfunction

//--------------------------------------------------------------------------------
