//-----------------------------------------------------------------------------

const WMS_TYPE_ADD           := "A";
const WMS_TYPE_DEL           := "D";

const PROP_WMS_GROUP         := "WmsGroup";

//-----------------------------------------------------------------------------

function GetWmsGroups()

  var Data := GetWmsData();

  if(!Data)
    return Data;
  endif

  var Keys := Data.Keys();

  return Keys;

endfunction

//-----------------------------------------------------------------------------

function WmsGroupStruct(wmsName := 0)

  var Data := struct;

  Data.+Name := "";
  Data.+Realm := "";
  Data.+Descr := "";
  Data.+Type := 0;

  if(wmsName)

    var Group := GetWmsGroup(wmsName);

    if(!Group)
      return Group;
    endif

    Data.Name  := wmsName;
    Data.Type  := Group.GetProp("Type");
    Data.Realm := Group.GetProp("Realm");
    Data.Descr := Group.GetProp("Description");
    Data.+ItemCount := Group.GetProp("ItemCount");

  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function VerifyWmsGroupData(byref Data, ExData := 0)

  if(!Data.Name)
    return error{"errortext" := "Brak nazwy grupy"};
  elseif(Data.Name[" "])
    return error{"errortext" := "Nazwa nie moze zawierac spacji"};
  elseif(len(Data.Name) > 8)
    return error{"errortext" := "Po co taka dluga nazwa? Bedzie ciezko wpisywac! Max 8 znakow."};
  elseif(!Data.Realm)
    return error{"errortext" := "Brak realmu"};
  elseif(!Data.Descr)
    return error{"errortext" := "Brak opisu"};
  elseif(!(Data.Type in array(WMS_TYPE_ADD, WMS_TYPE_DEL)))
    return error{"errortext" := "Bledny typ grupy"};
  elseif(!ExData)
    if(GetWmsGroup(Data.Name))
      return error{"errortext" := "Grupa WMS o nazwie "+Data.Name+" juz istnieje"};
    endif
  endif

  if(ExData)
    if(ExData.ItemCount)
      if(Data.Realm != ExData.Realm)
        return error{"errortext" := "Nie mozna zmienic realmu WMS grupie, ktora posiada przedmioty."};
      elseif(Data.Type != ExData.Type)
        return error{"errortext" := "Nie mozna zmienic typu WMS grupie, ktora posiada przedmioty."};
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddWmsGroup(who, addData)

  var Res := VerifyWmsGroupData(addData);

  if(!Res)
    return Res;
  endif

  var wmsData := GetWmsData();

  if(!wmsData)
    return wmsData;
  endif

  var Elem := wmsData.FindElement(addData.Name);

  if(!Elem) 
    Elem := wmsData.CreateElement(addData.Name);
  endif

  if(!Elem)
    return Elem;
  endif

  Elem.SetProp("Type", addData.Type);
  Elem.SetProp("Description", addData.Descr);
  Elem.SetProp("Realm", addData.Realm);
  Elem.SetProp("Items", array);
  Elem.SetProp("Creator", who.acctname);
  Elem.SetProp("AddTime", PolCore().systime);
  Elem.SetProp("AddGameTime", ReadGameClock());
  Elem.SetProp("ItemCount", 0);

  return Elem;

endfunction

//-----------------------------------------------------------------------------

function ModWmsGroup(who, modData)

  var ExData := WmsGroupStruct(modData.Name);

  if(!ExData)
    return ExData;
  endif

  var Res := VerifyWmsGroupData(modData, ExData);

  if(!Res)
    return Res;
  endif

  var wmsGroup := GetWmsGroup(modData.Name);

  wmsGroup.SetProp("Type", modData.Type);
  wmsGroup.SetProp("Realm", modData.Realm);
  wmsGroup.SetProp("Description", modData.Descr);

  wmsGroup.SetProp("LastModifier", who.acctname);
  wmsGroup.SetProp("LastTime", PolCore().systime);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DelWmsGroup(wmsName)

  var wmsData := GetWmsData();
  
  if(!wmsData)
    return wmsData;
  endif

  var Group := GetWmsGroup(wmsName);

  if(!Group)
    return Group;
  endif

  if(Group.GetProp("ItemCount") > 0)
    return error{"errortext" := "WMS grupa: "+wmsName+" posiada przypisane przedmioty. Nie mozna jej usunac."};
    return;
  endif

  return wmsData.DeleteElement(wmsName);

endfunction

//-----------------------------------------------------------------------------

function GetWmsItemDefinitionFromStatic(stcItem)

  if(stcItem.x == error or stcItem.objtype == error)
    return error{"errortext" := "Bledna definicja statika: "+stcItem};
  endif

  stcItem.+name := GetStaticName(stcItem.objtype);
  stcItem.+color  := 0;
  stcItem.+serial := 0;

  return stcItem;

endfunction

//-----------------------------------------------------------------------------

function GetWmsItemDefinition(Item)

  var ItemDef := GetItemDefinition(Item, GIDF_BASIC | GIDF_STATIC);

  ItemDef.+serial := Item.serial;

  return ItemDef;

endfunction

//-----------------------------------------------------------------------------

function ValidateAddToWmsGroup(Name, Group, Item)

  if(Group.GetProp("Type") == WMS_TYPE_ADD)

    if(!Item.isa(POLCLASS_ITEM))
      return error{"errortext" := "To nie przedmiot: "+Item};
    endif

    var exName := GetObjProperty(Item, PROP_WMS_NAME);

    if(exName == Name)
      return error{"errortext" := "Przedmiot jest juz przypisany do tej grupy."};
    elseif(exName)
      return error{"errortext" := "Przedmiot jest przypisany do grupy: '"+exName+"'"};
    elseif(Group.GetProp("Realm") != Item.realm)
      return error{"errortext" := "Grupa do jakiej chcesz przypisac jest na innym realmie niz przedmot!'"};
    endif

    return CanBeStaticItem(Item);

  else

    if(Item.x == error or Item.y == error or Item.z == error or !Item.objtype)
      return error{"errortext" := "Bledna definicja statika."};
    endif

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function WmsDefinitionExistsForItem(Item, byref Items)

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

function WmsGroupBuildExItemsDict(byref Items)

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

function DisplayWmsResult(who, Results)

  foreach Res in Results
    
    if(_Res_iter[1] == "#")
      SendSysMessage(who, "["+Res+"] "+SubStr(_Res_iter, 2, len(_Res_iter)-1), FONT_NORMAL, COLOR_WHITE);
    else
      SendSysMessage(who, "["+Res+"] Blad: "+_Res_iter, FONT_NORMAL, COLOR_ORANGE);
    endif

  endforeach

endfunction

//-----------------------------------------------------------------------------

function DelStaticItemsFromWmsGroup(wmsName, Items)

  var Group := GetWmsGroup(wmsName);
  var Index;
  var Res := dictionary;

  if(!Group)
    return Group;
  endif

  var grItems := Group.GetProp("Items");
  var sMsg;

  foreach Item in Items
    Index := WmsDefinitionExistsForItem(Item, grItems);

    if(Index)
      sMsg := "#Usunieto przedmiot z WMS.";
      grItems.erase(Index);
    else
      sMsg := "Przedmiot nie istnieje w grupie.";
    endif

    Res[sMsg] := CInt(Res[sMsg]) + 1;
  endforeach

  Group.SetProp("Items", grItems);
  Group.SetProp("ItemCount", grItems.size());

  return Res;

endfunction

//-----------------------------------------------------------------------------

function DelItemFromWmsGroup(Item, byref GroupData)

  var wmsName := GetObjProperty(Item, PROP_WMS_NAME);

  if(wmsName == error)
    return error{"errortext" := "Przedmiot nie jest przypisany do grupy WMS."};
  else
    
    var Group := GetWmsGroup(wmsName);

    if(!Group)
      error{"errortext" := "Przedmiot "+hex(Item.serial)+" ma przypisana nieistniejaca GRUPE WMS. Zglos to do SMa!"};
    endif

    if(!GroupsData.exists(wmsName))
      GroupsData[wmsName] := struct;
      GroupsData[wmsName].+Items   := Group.GetProp("Items");
      GroupsData[wmsName].+exItems := WmsGroupBuildExItemsDict(GroupData[wmsName].Items);
    endif

    if(GroupsData[wmsName].exItems.exists(Item.serial))
      GroupsData[wmsName].Items.erase(GroupsData[wmsName].exItems[Item.serial]);
      EraseObjProperty(Item, PROP_WMS_NAME);
    else
      error{"errortext" := "Przedmiot "+hex(Item.serial)+" ma przypisana grupe WMS, ale nie istnial w definicji przedmiotow! Zglos to do SMa!"};
    endif

  endif

  return wmsGroup;

endfunction

//-----------------------------------------------------------------------------

function DelItemsFromWmsGroup(Items)

  var Res        := dictionary;
  var GroupsData := dictionary;

  foreach Item in Items

    var iRes := DelItemFromWmsGroup(Item, GroupsData);

    if(iRes == error)

      Res[iRes.errortext] := CInt(Res[iRes.errortext]) + 1;

    else
  
      var sMsg := "#Usunieto przedmiot z WMS.";
      Res[sMsg] := CInt(Res[sMsg]) + 1;

    endif

  endforeach

  // na koniec zapisujemy grupy, z jakich itemy zostaly usuniete.
  foreach grData in GroupsData
    
    var Group := GetWmsGroup(_grData_iter);

    Group.SetProp("Items", grData.Items);
    Group.SetProp("ItemCount", grData.Items.size());

  endforeach

  return Res;

endfunction

//-----------------------------------------------------------------------------

function AddItemsToWmsGroup(wmsName, Items)

  var Group := GetWmsGroup(wmsName);
  var Type  := Group.GetProp("Type");

  if(!Group)
    return error{"errortext" := "Brak grupy o nazwie: "+wmsName};
  endif

  var Res   := dictionary;
  var grItems := Group.GetProp("Items");

  foreach Item in Items

    var iRes := ValidateAddToWmsGroup(wmsName, Group, Item);

    if(iRes == error)

      Res[iRes.errortext] := CInt(Res[iRes.errortext]) + 1;

    elseif(WmsDefinitionExistsForItem(Item, grItems))

      if(Type == WMS_TYPE_ADD)

        var errorText := "Definicja dla przedmiotu juz istniala. Przedmiot zostal usuniety.";
        Res[errorText] := CInt(Res[errorText]) + 1;

        DestroyItem(Item);

      elseif(Type == WMS_TYPE_DEL)

        var errorText := "Definicja dla statika juz istniala.";
        Res[errorText] := CInt(Res[errorText]) + 1;

      endif

    else

      var sMsg := "#Dodano przedmiot do grupy "+wmsName;
      Res[sMsg] := CInt(Res[sMsg]) + 1;

      var ItemDef;

      if(Type == WMS_TYPE_ADD)
        ItemDef := GetWmsItemDefinition(Item);
      elseif(Type == WMS_TYPE_DEL)
        ItemDef := GetWmsItemDefinitionFromStatic(Item);
      else
        return error{"errortext" := "What tha fuck!?"};
      endif

      grItems.append(ItemDef);
      SetObjProperty(Item, PROP_WMS_NAME, wmsName);

    endif

  endforeach

  Group.SetProp("Items", grItems);
  Group.SetProp("ItemCount", grItems.size());

  return Res;

endfunction

//-----------------------------------------------------------------------------

function CleanItemsInWmsGroup(Name)

  var Group := GetWmsGroup(Name);
  var Res := struct;
  Res.+desCount := 0;
  Res.+allCount := 0;

  if(!Group)
    return RetError("Grupa '"+Name+"' nie istnieje");
  endif

  var Items := Group.GetProp("Items");
  var dRes, Item;

  foreach ItemData in Items
    
    Item := FindItemBySerial(ItemData.serial);

    if(Item)

      dRes := DestroyItem(Item);

      if(!dRes)
        return RetError("Nie udalo sie zniszczyc przedmiotu: "+Item.name+"/"+hex(Item.serial)+": "+dRes.errortext);
      endif

      Res.desCount += 1;

    endif

    Res.allCount += 1;

  endforeach

  Group.SetProp("Items", array);
  Group.SetProp("ItemCount", 0);

  return Res;

endfunction

//-----------------------------------------------------------------------------

function GetWmsGroupTypes()

  var Types := dictionary;

  Types[WMS_TYPE_ADD] := "Dodanie";
  Types[WMS_TYPE_DEL] := "Skasowanie";

  return Types;

endfunction

//-----------------------------------------------------------------------------

function GetWmsGroupTypeName(TypeId)

  var Types := GetWmsGroupTypes();

  if(Types.exists(TypeId))
    return Types[Typeid];
  endif

  return "<Brak>";

endfunction

//-----------------------------------------------------------------------------

function GetWmsGroupItem(wmsName, Index)

  var wmsGroup := GetWmsGroup(wmsName);

  if(!wmsGroup)
    return wmsGroup;
  endif

  var Items := wmsGroup.GetProp("Items");

  if(!Items.exists(Index))
    return error{"errortext" := "Przedmiot o indeksie: "+Index+" nie istnieje w WMS grupie "+wmsName};
  endif

  return Items[Index];

endfunction

//-----------------------------------------------------------------------------

function DeleteItemFromWmsGroup(wmsName, Index)

  var wmsGroup := GetWmsGroup(wmsName);

  if(!wmsGroup)
    return wmsGroup;
  endif

  var Items := wmsGroup.GetProp("Items");

  if(!Items.exists(Index))
    return error{"errortext" := "Przedmiot o indeksie: "+Index+" nie istnieje w WMS grupie "+wmsName};
  endif

  var Item := FindItemBySerial(Items[Index].serial);

  if(Item)
    var Res := DestroyItem(Item);
    if(!Res)
      return Res;
    endif
  endif

  Items.erase(Index);
  wmsGroup.SetProp("Items", Items);
  wmsGroup.SetProp("ItemCount", Items.size());
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetWmsGroupForPerson(who, byref wmsName)

  if(!wmsName)
    wmsName := GetCurrentWmsGroup(who);
  endif

  return GetWmsGroup(wmsName);

endfunction

//-----------------------------------------------------------------------------

function GetCurrentWmsGroup(who)

  return GetObjProperty(who, PROP_WMS_GROUP);

endfunction

//-----------------------------------------------------------------------------

function EraCurrentWmsGroup(who)

  if(GetCurrentWmsGroup(who))
    EraseObjProperty(who, PROP_WMS_GROUP);
  else
    return RetError("Osoba "+GetRealName(who)+" nie posiada przypisanej grupy");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetCurrentWmsGroup(who, wmsName)

  var curName := GetCurrentWmsGroup(who);

  if(curName == wmsName)
    return RetError("Masz juz przypisana grupe WMS: "+wmsName);
  elseif(!GetWmsGroup(wmsName))
    return RetError("Grupa WMS: "+wmsName+" nie istnieje.");
  endif

  SetObjProperty(who, PROP_WMS_GROUP, wmsName);
  return 1;

endfunction

//-----------------------------------------------------------------------------
