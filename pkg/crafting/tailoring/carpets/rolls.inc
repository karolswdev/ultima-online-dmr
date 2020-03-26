//--------------------------------------------------------------------------------

const PROP_ROLLED_TAPESTRY             := "RolledObjType";
const PROP_ROLLED_CARPET               := "RolledCarpet";
const PROP_CREATED_ARRAY               := "CreatedArray";

//--------------------------------------------------------------------------------

function IsCarpetRoll(Item)

  var ItemCfg := ReadConfigFile(":carpets:itemdesc");

  if(ItemCfg[Item.objtype])
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsCarpetRollMain(Item)

  if(IsCarpetRoll(Item))
    if(GetObjProperty(Item, PROP_ROLLED_TAPESTRY) != error or GetObjProperty(Item, PROP_ROLLED_CARPET) != error)
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function DestroyCreatedItems(Created)

  var Item;

  foreach Serial in Created

    if(Serial)
      Item := FindItemBySerial(Serial);
      if(Item)
        ReleaseItem(Item);
        if(!DestroyItem(Item))
          LogError("[CarpetRollBack] Unable to destroy Carpet roll created item! Have to continue anyway, item data: "+Item.serial+"/"+Item.objtype+"/"+GetName(Item));
        endif
      endif
    endif
  endforeach

  return 1;

endfunction

//--------------------------------------------------------------------------------

function CarpetRollBack(Item, Cont)

  if(GetObjProperty(Item, PROP_ROLLED_TAPESTRY) != error)

    var RolledObj := CInt(GetObjProperty(Item, PROP_ROLLED_TAPESTRY));
    var Created   := GetObjProperty(Item, PROP_CREATED_ARRAY);

    if(!RolledObj or !Created)
      return 0;
    endif
    
    DestroyCreatedItems(Created);
    CreateItemInCont(Cont, RolledObj);
    return 1;

  elseif(GetObjProperty(Item, PROP_ROLLED_CARPET) != error)

    var Created   := GetObjProperty(Item, PROP_CREATED_ARRAY);
    var RolledObj := CInt(GetObjProperty(Item, PROP_ROLLED_CARPET));
    var Width     := ToMin(CInt(GetObjProperty(Item, "RolledCarpetWidth")), 1);
    var Height    := ToMin(CInt(GetObjProperty(Item, "RolledCarpetHeight")), 1);

    if(!RolledObj or !Created)
      return 0;
    endif

    DestroyCreatedItems(Created);
    var RolledCarp := CreateItemInCont(Cont, RolledObj, 1);

    SetObjProperty(RolledCarp, "Width", Width);
    SetObjProperty(RolledCarp, "Height", Height);

    var Name := GetDescForItem(RolledCarp.objtype);
    Name := Name + " ["+CInt(Width)+" Szerokosci, "+CInt(Height)+" Dlugosci]";
    SetName(RolledCarp, Name);
    return 1;

  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------
