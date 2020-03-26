//-----------------------------------------------------------------------------

include ":itemsets:shared";
include ":itemsets:definition";

//-----------------------------------------------------------------------------

const PROP_ITEM_SETS         := "ItemSets";

//-----------------------------------------------------------------------------

const IST_CONTAINER          := "cont";
const IST_ITEM               := "item";
const IST_LOCATION           := "loc";

//-----------------------------------------------------------------------------

const CISF_QUEST_ITEMS       := 0x01;
const CISF_PLAYER_ITEMS      := 0x02;
const CISF_LOC               := 0x04;
const CISF_CONTAINER         := 0x08;

//-----------------------------------------------------------------------------

function GetPersonSets(who)

  var Sets := GetObjProperty(who, PROP_ITEM_SETS);

  if(!Sets)
    Sets := dictionary;
  endif

  return Sets;

endfunction

//-----------------------------------------------------------------------------

function AddPersonSet(who, Name, byref Data)

  var Sets := GetPersonSets(who);

  Sets[Name] := Data;

  SetObjProperty(who, PROP_ITEM_SETS, Sets);

endfunction

//-----------------------------------------------------------------------------

function RemoveItemSet(who, Name)

  var Sets := GetPersonSets(who);

  if(Sets.exists(Name))
    Sets.erase(Name);
    SetObjProperty(who, PROP_ITEM_SETS, Sets);
    return 1;
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Funkcje generujace definicje seta
//-----------------------------------------------------------------------------
//=============================================================================

function ItemSetStruct()

  var Data := struct;

  Data.+Name    := "";
  Data.+Count   := 0;
  Data.+addTime := 0;
  Data.+Creator := 0;
  Data.+Items   := array;
  Data.+type    := 0;

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetItemSetDefinitionFromItem(who, Name, Item)

  var Data := ItemSetStruct();

  Data.Name    := Name;
  Data.addTime := PolCore().systime;
  Data.Creator := who.acctname;

  Data.Items.append(GetItemDefinition(Item, GIDF_FULL | GIDF_NO_LOCATION));
  Data.Count   := 0;

  if(Item.isa(POLCLASS_CONTAINER))
    Data.type := IST_CONTAINER;
  else
    Data.type := IST_ITEM;
  endif

  IncreaseItemSetCount(Data.Count, Data.Items[1]);

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetItemSetDefinitionFromLoc(who, Name, nwLoc, seLoc, midLoc)

  var Data := ItemSetStruct();

  Data.Name    := Name;
  Data.addTime := PolCore().systime;
  Data.Creator := who.acctname;

  Data.Count   := 0;
  Data.type := IST_LOCATION;

  if(coordist(nwLoc.x, nwLoc.y, seLoc.x, seLoc.y) > 50)
    var Res := error;
    Res.errortext := "Zasieg pomiedzy wskazanymi lokacjami przekracza 50! To stanowczo za duzo.";
    return Res;
  elseif(nwLoc.realm != seLoc.realm)
    var Res := error;
    Res.errortext := "Wth?! Rozne realmy?";
    return Res;
  elseif(nwLoc.x > midLoc.x or seLoc.x < midLoc.x or nwLoc.y > midLoc.y or seLoc.y < midLoc.y)
    var Res := error;
    Res.errortext := "Wskazany srodek nie jest wewnatrz wybranego wczesniej zasiegu...";
    return Res;
  endif

  foreach Item in ListObjectsInBox(nwLoc.x, nwLoc.y, -127, seLoc.x, seLoc.y, 127, nwLoc.realm)
    if(Item.isa(POLCLASS_ITEM))
      Data.Items.append(GetItemDefinition(Item, GIDF_FULL, midLoc));
      IncreaseItemSetCount(Data.Count, Data.Items[Data.Items.size()]);
    endif
  endforeach

  if(Data.Count == 0)
    var Res := error;
    Res.errortext := "Nie znaleziono przedmiotow w wybranym zasiegu.";
    return Res;
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function IncreaseItemSetCount(byref Count, byref Item)

  Count += 1;

  foreach subItem in (Item.Items)
    IncreaseItemSetCount(Count, subItem);
  endforeach

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Tworzenie setow
//-----------------------------------------------------------------------------
//=============================================================================

function CreateItemFromSet(who, Set, Dest := 0, Flags := 0)

  if(!Set or !Set.type)
    var Res := error;
    Res.errortext := "Bledne dane seta!";
    return Res;
  endif

  if(Set.type == IST_ITEM or Set.type == IST_CONTAINER)
    
    if(!Dest)
      if(Flags & CISF_LOC)
        Dest := LocTarget(who);

        if(!Dest)
          SendSysMessage(who, "Anulowano.", FONT_NORMAL, COLOR_RED);
          return 0;
        endif
      elseif(Flags & CISF_CONTAINER)
        Dest := ObjTarget(who);

        if(!Dest.isa(POLCLASS_CONTAINER))
          SendSysMessage(who, "To nie pojemnik.", FONT_NORMAL, COLOR_RED);
          return 0;
        endif
      else
        SendSysMessage(who, "Nieznany typ celu.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

  elseif(Set.type == IST_LOCATION)

    if(Flags & (CISF_QUEST_ITEMS | CISF_PLAYER_ITEMS))
    
      SendSysMessage(who, "To set lokacyjny. Nie jest mozliwe stworzenie przedmiotow questowych/dla gracza!", FONT_NORMAL, COLOR_RED);
      return 0;

    endif

    if(Flags & CISF_LOC)

      Dest := LocTarget(who);

      if(!Dest)
        SendSysMessage(who, "Anulowano.", FONT_NORMAL, COLOR_RED);
        return 0;
      endif

    else

      SendSysMessage(who, "To set lokacyjny. Nie jest mozliwe stworzenie w pojemniku!", FONT_NORMAL, COLOR_RED);
      return 0;

    endif

  else

    SendSysMessage(who, "Ten typ setow nie jest obslugiwany!", FONT_NORMAL, COLOR_RED);
    return 0;

  endif

  var Res := CreateItemsFromDefinition(Dest, Set.Items);

  if(!Res)
    SendSysMessage(who, CStr(Res.errortext), FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  SendSysMessage(who, "Stworzono set ["+Set.Name+"]. Ilosc przedmiotow: "+Res.Items.size(), FONT_NORMAL, COLOR_WHITE);

  if(Flags & (CISF_QUEST_ITEMS | CISF_PLAYER_ITEMS))

    foreach Item in (Res.Items)

      RemoveObjCreator(Item);

      if(Flags & CISF_QUEST_ITEMS)
        SetPropQuestItem(who, Item);
      endif

      LogMsg(QSITEMS_LOG, who, item);

    endforeach

    if(Flags & CISF_QUEST_ITEMS)
      var ctrlRes := AddItemSetControl(who, Set, Res.Items);
      if(ctrlRes)
        SendSysMessage(who, "Dodano przedmioty do globalnej kontroli.", FONT_NORMAL, COLOR_BLUE);
      else
        SendSysMessage(who, "Wystapil blad podczas dodawania przedmiotow do globalnej kontroli! Blad: "+ctrlRes.errortext, FONT_NORMAL, COLOR_RED);
      endif
    endif

    SendSysMessage(who, "Zalogowano utworzenie "+Res.Items.size()+" przedmiotow.", FONT_NORMAL, COLOR_ORANGE);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
