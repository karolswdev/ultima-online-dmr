//-----------------------------------------------------------------------------

function IsItemGroupName(GroupName)

  var Cfg := ReadConfigFile(ITEMGROUPS_CFG);

  if(Cfg[GroupName])
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetItemGroupObjTypes(GroupName)

  var Cfg := ReadConfigFile(ITEMGROUPS_CFG);

  if(!Cfg[GroupName])
    return RetError("Grupa itemow "+GroupName+" nie istnieje!");
  endif

  var cItems := GetConfigStringArray(Cfg[GroupName], "Item");

  if(!cItems.size() or !cItems)
    return RetError("Grupa itemow jest pusta! ["+GroupName+"]");
  endif

  foreach cItem in cItems
    cItem := SplitWords(cItem);
    var ObjType := GetItemObjType(cItem[2]);

    if(!ObjType)
      return RetError("Grupa itemow ["+GroupName+"]: Zdefiniowany item ["+cItem[2]+"] nie istnieje.");
    else
      cItem := ObjType;
    endif
  endforeach

  return cItems;

endfunction

//-----------------------------------------------------------------------------

function GetRandomItemFromGroup(GroupName)

  var ObjTypes := GetItemGroupObjTypes(GroupName);

  if(!ObjTypes)
    return ObjTypes;
  endif

  var ObjType := ObjTypes[RandomInt(ObjTypes.size())+1];

  if(!ObjType)
    return RetError("Wystapil blad podczas losowania itema z grupy: "+GroupName);
  endif

  return ObjType;

endfunction

//-----------------------------------------------------------------------------
