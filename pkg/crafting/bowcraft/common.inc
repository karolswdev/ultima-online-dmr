//-----------------------------------------------------------------------------

function RepairBowCraftItem(who,Item)

  if(!StandardItemCheck(who, Item))
    return 0;
  endif

  var LogType  := GetItemMakeMod(Item);
  if(!LogType)
    LogType := 1;
  endif
  
  var LogObj := GetLogObjTypes()[LogType];
  if(IsArtifact(Item) or GetBulkLevel(Item))
    LogObj := UOBJ_MYSTICLOG;
  endif

  RepairItem( who, Item, SKILLID_BOWCRAFT, LogObj, 0x21);

endfunction

//-----------------------------------------------------------------------------
