//-----------------------------------------------------------------------------

const WMSTATICS_DATA         := ":wmtools:statics";
const PROP_WMS_NAME          := "WmsName";

//-----------------------------------------------------------------------------

function GetWmsGroup(wmsName)

  if(wmsName == error)
    return error{"errortext" := "Nie przekazano nazwy grupy."};
  endif

  var Data := GetWmsData();

  return Data.FindElement(wmsName);

endfunction

//-----------------------------------------------------------------------------

function GetWmsData()

  var Data := OpenDataFile(WMSTATICS_DATA);

  if(!Data)
    Data := CreateDataFile(WMSTATICS_DATA);
  endif

  if(!Data)
    LogError("GetWmsData: Nie udalo sie stworzyc WMStaticsData ["+WMSTATICS_DATA+"]: "+Data.errortext);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function IsWms(Item)

  return GetObjProperty(Item, PROP_WMS_NAME) != error;

endfunction

//-----------------------------------------------------------------------------

function GetItemWmsName(Item)

  return GetObjProperty(Item, PROP_WMS_NAME);

endfunction

//-----------------------------------------------------------------------------

function OnWmsItemDestroy(Item)

  var Group := GetWmsGroup(GetItemWmsName(Item));

  if(Group)

    var Items := Group.GetProp("Items");
    var i := 1;
    var Idx;

    foreach ItemData in Items

      if(ItemData.serial == Item.serial)
        
        Idx := i;
        break;

      endif

      i += 1;

    endforeach

    if(Idx)
      Items.erase(Idx);
    endif

    Group.SetProp("Items", Items);
    Group.SetProp("ItemCount", Items.size());

  endif

endfunction

//-----------------------------------------------------------------------------
