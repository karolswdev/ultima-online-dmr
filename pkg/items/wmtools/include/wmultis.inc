//-----------------------------------------------------------------------------

const WML_TYPE_ADD           := "A";
const WML_TYPE_DEL           := "D";
const WML_DYNAMIC_COLOR      := 999;

const PROP_WML_MID_LOC       := "WmlMidLoc";

//-----------------------------------------------------------------------------

function GetWmlGroups()

  var Data := GetWmlData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.Keys();
  
  MakeIntArray(Keys);
  Keys.sort();

  return Keys;

endfunction

//-----------------------------------------------------------------------------

function WmlGroupStruct(wmlId := 0)

  var Data := struct;

  Data.+Id := -1;

  if(wmlId)

    var Group := GetWmlGroup(wmlId);

    if(!Group)
      return Group;
    endif

    Data.Id    := wmlId;
    Data.+ItemCount := Group.GetProp("ItemCount");

  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function VerifyWmlGroupData(byref Data, ExData := 0)

  if(Data.Id < 0)
    return error{"errortext" := "Brak ID grupy"};
  elseif(!ExData)
    if(GetWmlGroup(Data.Id))
      return error{"errortext" := "Grupa WML o ID: "+Data.Id+" juz istnieje"};
    endif
  endif

  if(ExData)
    if(ExData.ItemCount)
      if(Data.Realm != ExData.Realm)
        return error{"errortext" := "Nie mozna zmienic realmu WML grupie, ktora posiada przedmioty."};
      elseif(Data.Type != ExData.Type)
        return error{"errortext" := "Nie mozna zmienic typu WML grupie, ktora posiada przedmioty."};
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddWmlGroup(who, addData)

  var Res := VerifyWmlGroupData(addData);

  if(!Res)
    return Res;
  endif

  var wmlData := GetWmlData();

  if(!wmlData)
    return wmlData;
  endif

  var Elem := wmlData.FindElement(CStr(CInt(addData.Id)));

  if(!Elem) 
    Elem := wmlData.CreateElement(CStr(CInt(addData.Id)));
  endif

  if(!Elem)
    return Elem;
  endif

  Elem.SetProp("Items", array);
  Elem.SetProp("Creator", who.acctname);
  Elem.SetProp("AddTime", PolCore().systime);
  Elem.SetProp("AddGameTime", ReadGameClock());
  Elem.SetProp("ItemCount", 0);
  Elem.SetProp("Status", "New");

  return Elem;

endfunction

//-----------------------------------------------------------------------------

function DelWmlGroup(wmlId)

  var wmlData := GetWmlData();
  
  if(!wmlData)
    return wmlData;
  endif

  var Group := GetWmlGroup(wmlId);

  if(!Group)
    return Group;
  endif

  if(Group.GetProp("ItemCount") > 0)
    return error{"errortext" := "WML grupa: "+wmlId+" posiada przypisane przedmioty. Nie mozna jej usunac."};
    return;
  endif

  return wmlData.DeleteElement(CStr(CInt(wmlId)));

endfunction

//-----------------------------------------------------------------------------

function ValidateAddToWmlGroup(Item)

  if(!Item.isa(POLCLASS_ITEM))
    return error{"errortext" := "To nie przedmiot: "+Item};
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function WmlDefinitionExistsForItem(Item, byref Items)

  var i := 1;

  foreach ItemData in Items

    if(ItemData.x == Item.x and ItemData.y == Item.y and ItemData.z == Item.z and Item.objtype == ItemData.objtype)
      return i;
    endif

    i += 1;

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function WmlGroupBuildExItemsDict(byref Items)

  var exItems := dictionary;
  var i := 1;

  foreach ItemData in Items

    if(ItemData.serial)
      exItems[ItemData.serial] := i;
    endif

    i += 1;

  endforeach

  return exItems;

endfunction

//-----------------------------------------------------------------------------

function DisplayWmlResult(who, Results)

  foreach Res in Results
    
    if(_Res_iter[1] == "#")
      SendSysMessage(who, "["+Res+"] "+SubStr(_Res_iter, 2, len(_Res_iter)-1), FONT_NORMAL, COLOR_WHITE);
    else
      SendSysMessage(who, "["+Res+"] Blad: "+_Res_iter, FONT_NORMAL, COLOR_ORANGE);
    endif

  endforeach

endfunction

//-----------------------------------------------------------------------------

function WmlItemDefinition(Item, midLoc)

  var ItemDef := GetItemDefinition(Item, GIDF_BASIC | GIDF_STATIC, midLoc);

  if(Item.color == WML_DYNAMIC_COLOR)
    ItemDef.+visible := 0;
    ItemDef.color := 0;
  else
    ItemDef.+visible := 1;
  endif

  return ItemDef;

endfunction

//-----------------------------------------------------------------------------

function DelItemsFromWmlGroup(wmlId, Items, midLoc)

  var Group := GetWmlGroup(wmlId);
  var Index;
  var Res := dictionary;

  if(!Group)
    return Group;
  endif

  var grItems := Group.GetProp("Items");
  var sMsg;

  foreach Item in Items

    var ItemDef := WmlItemDefinition(Item, midLoc);

    Index := WmlDefinitionExistsForItem(ItemDef, grItems);

    if(Index)
      sMsg := "#Usunieto przedmiot z WML.";
      grItems.erase(Index);
    else
      sMsg := "Przedmiot nie istnieje w grupie.";
    endif

    Res[sMsg] := CInt(Res[sMsg]) + 1;

  endforeach

  Group.SetProp("Items", grItems);
  Group.SetProp("ItemCount", grItems.size());
  if(Group.GetProp("Status") != "New")
    Group.SetProp("Status", "Changed");
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AddItemsToWmlGroup(wmlId, Items, midLoc)

  var Group := GetWmlGroup(wmlId);

  if(!Group)
    return error{"errortext" := "Brak elementu WML o ID: "+wmlId};
  endif

  var Res   := dictionary;
  var grItems := Group.GetProp("Items");

  foreach Item in Items

    var iRes := ValidateAddToWmlGroup(Item);
    var ItemDef := WmlItemDefinition(Item, midLoc);

    if(iRes == error)

      Res[iRes.errortext] := CInt(Res[iRes.errortext]) + 1;

    elseif(WmlDefinitionExistsForItem(ItemDef, grItems))

      var errorText := "Definicja dla przedmiotu juz istniala.";
      Res[errorText] := CInt(Res[errorText]) + 1;

    else

      var sMsg := "#Dodano przedmiot do elementu WML: "+wmlId;
      Res[sMsg] := CInt(Res[sMsg]) + 1;

      grItems.append(ItemDef);

    endif

  endforeach

  Group.SetProp("Items", grItems);
  Group.SetProp("ItemCount", grItems.size());
  if(Group.GetProp("Status") != "New")
    Group.SetProp("Status", "Changed");
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function CreateWmlGroup(wmlId, midLoc)

  var Group := GetWmlGroup(wmlId);

  if(!Group)
    return error{"errortext" := "Grupa '"+wmlId+"' nie istnieje"};
  endif

  var Items := Group.GetProp("Items");

  foreach Item in Items

    if(Item.objtype == 1)
      continue;
    endif

    var crItem := CreateItemAtLoc(midLoc.x + Item.x, midLoc.y + Item.y, midLoc.z + Item.z, Item.objtype, 1, midLoc.realm);

    if(!crItem)
      return crItem;
    endif

    if(!Item.visible)
      crItem.color := WML_DYNAMIC_COLOR;
    endif

    crItem.movable := 0;

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DeleteWmlGroup(wmlId)

  var Group := GetWmlGroup(wmlId);

  if(!Group)
    EraseWmlGroup(wmlId);
    return error{"errortext" := "Grupa '"+wmlId+"' nie istnieje"};
  endif

  var Status := Group.GetProp("Status");
  if(Status == "Exist" or Status == "Changed")
    Group.SetProp("Status", "Removed");
    Group.SetProp("Items", array);
    Group.SetProp("ItemCount", 0);
  elseif (Status == "New")
    EraseWmlGroup(wmlId);
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CleanItemsInWmlGroup(wmlId)

  var Group := GetWmlGroup(wmlId);

  if(!Group)
    return error{"errortext" := "Grupa '"+wmlId+"' nie istnieje"};
  endif

  Group.SetProp("Items", array);
  Group.SetProp("ItemCount", 0);
  if(Group.GetProp("Status") != "New")
    Group.SetProp("Status", "Changed");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetWmlGroupItem(wmlId, Index)

  var wmlGroup := GetWmlGroup(wmlId);

  if(!wmlGroup)
    return wmlGroup;
  endif

  var Items := wmlGroup.GetProp("Items");

  if(!Items.exists(Index))
    return error{"errortext" := "Przedmiot o indeksie: "+Index+" nie istnieje w WML ID: "+wmlId};
  endif

  return Items[Index];

endfunction

//-----------------------------------------------------------------------------

function DeleteItemFromWmlGroup(wmlId, Index)

  var wmlGroup := GetWmlGroup(wmlId);

  if(!wmlGroup)
    return wmlGroup;
  endif

  var Items := wmlGroup.GetProp("Items");

  if(!Items.exists(Index))
    return error{"errortext" := "Przedmiot o indeksie: "+Index+" nie istnieje w WML ID: "+wmlId};
  endif

  Items.erase(Index);
  wmlGroup.SetProp("Items", Items);
  wmlGroup.SetProp("ItemCount", Items.size());
  if(wmlGroup.GetProp("Status") != "New")
    wmlGroup.SetProp("Status", "Changed");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetWmlMidLoc(who, wmlId)

  var midLocs := GetObjProperty(who, PROP_WML_MID_LOC);

  if(!midLocs)
    return RetError("Nie istnieje zadne zapisane centrium.");
  endif

  if(!midLocs.exists(CInt(wmlId)))
    return RetError("Centrum dla: "+wmlId+" nie istnieje.");
  endif

  return midLocs[CInt(wmlId)];

endfunction

//-----------------------------------------------------------------------------

function SetWmlMidLoc(who, wmlId, midLoc)

  var midLocs := GetObjProperty(who, PROP_WML_MID_LOC);

  if(!midLocs)
    midLocs := dictionary;
  endif

  midLocs[CInt(wmlId)] := midLoc;

  SetObjProperty(who, PROP_WML_MID_LOC, midLocs);

endfunction

//-----------------------------------------------------------------------------

function BuildMultiTable()

  var ObjType;
  var Table := dictionary;

  for(ObjType := UOBJ_HOUSE_START; ObjType <= UOBJ_HOUSE_END; ObjType += 1)

    var ItemCfg := GetItemConfig(ObjType);

    if(ItemCfg and ItemCfg.MultiId)
      Table[CInt(ItemCfg.MultiId)] := ItemCfg;
    endif

  endfor

  return Table;

endfunction

//-----------------------------------------------------------------------------
