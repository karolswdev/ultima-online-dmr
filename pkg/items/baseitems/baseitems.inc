//-----------------------------------------------------------------------------

use datafile;

//-----------------------------------------------------------------------------

include "include/logerror";
include "include/key";

include "common";
include ":doors:doors";

//-----------------------------------------------------------------------------

const BASE_ITEM_DATA         := ":baseitems:baseitems";
const SET_ITEMS_CFG          := ":baseitems:setitems";
const CPX_ITEMS_CFG          := ":baseitems:cpxitems";

//-----------------------------------------------------------------------------

const BIPROP_LOC             := "Loc";
const BIPROP_OBJTYPE         := "ObjType";
const BIPROP_COLOR           := "Color";
const BIPROP_NAME            := "Name";
const BIPROP_FLAGS           := "Flags";
const BIPROP_SERIAL          := "ActualSer";

const GBIELEM_EMPTY          := "Empty";
const GBIPROP_EMPTYIDS       := "EmptyNumbers";
const GBIPROP_NEXTID         := "NextNumber";

//-----------------------------------------------------------------------------

const BIF_LOCKED        := 0x00000001;
const BIF_INVIS         := 0x00000002;

const BIUF_HARD         := 0x00000001;

//-----------------------------------------------------------------------------

function AddBaseItem(ObjType, Ix, Iy, Iz, Realm, Color := 0, Name := 0, Flags := 0)

  var Err := error;

  if(!ObjType)
    Err.errortext := "Bledny ObjType ["+ObjType+"]";
    return Err;
  endif

  if(!CInt(ObjType))
    ObjType := GetObjTypeByName(ObjType);
  else
    ObjType := CInt(ObjType);
  endif
  
  var Data     := GetBaseItemData();
  var Nr       := GetNextElemNr(Data);

  if(Nr)
    var NewElem := Data.CreateElement(CStr(Nr));
    if(NewElem)
      NewElem.SetProp(BIPROP_LOC,     array(Ix,Iy,Iz,Realm));
      NewElem.SetProp(BIPROP_OBJTYPE, ObjType);
      NewElem.SetProp(BIPROP_COLOR,   Color);
      NewElem.SetProp(BIPROP_NAME,    Name);
      NewElem.SetProp(BIPROP_FLAGS,   Flags);
      return Nr;
    else
      Err.errortext := "Nie udalo sie stworzyc nowego elementu ["+NewElem+"].";
    endif
  else
    Err.errortext := "Nie udalo sie pobrac nastepnego Nr ["+Nr+"]";
  endif

  return Err;

endfunction

//-----------------------------------------------------------------------------

function GetNextElemNr(Data)

  var Elem  := Data.FindElement(GBIELEM_EMPTY);
  if(!Elem)
    Elem := Data.CreateElement(GBIELEM_EMPTY);
  endif

  var Nr    := 0;
  var Empty := Elem.GetProp(GBIPROP_EMPTYIDS);

  if(!Empty.size() or !Empty)
    Elem.SetProp(GBIPROP_EMPTYIDS, array);
  else
    if(Empty[1])
      Nr := CInt(Empty[1]);
      Empty.erase(1);
      Elem.SetProp(GBIPROP_EMPTYIDS,Empty);
    endif
  endif

  if(!Nr)
    var NextNr := Elem.GetProp(GBIPROP_NEXTID);
    if(!NextNr)
      NextNr := Data.Keys().size()-1;
    endif
    Nr := NextNr+1;
    Elem.SetProP(GBIPROP_NEXTID, Nr);
  endif

  return Nr;

endfunction

//-----------------------------------------------------------------------------

function DeleteBaseItem(Nr)

  var Data   := GetBaseItemData();
  var Elem   := Data.FindElement(CStr(Nr));

  if(!Elem)
    return RetError("Nie znaleziono Bi, Nr ["+Nr+"]");
  endif

  var EmptyE := Data.FindElement(GBIELEM_EMPTY);
  var Empty  := EmptyE.GetProp(GBIPROP_EMPTYIDS);
  var Item   := FindItemBySerial(Elem.GetProp(BIPROP_SERIAL));

  if(!Empty.size() or !Empty)
    Empty := array;
    EmptyE.SetProp(GBIPROP_EMPTYIDS,Empty);
  endif

  if(Elem)
    Empty.append(Nr);
    EmptyE.SetProp(GBIPROP_EMPTYIDS,Empty);
    var dRes := Data.DeleteElement(CStr(Nr));
    if(dRes)
      if(Item)
        EraseObjProperty(Item, BIPROP_ITEMNR);
      endif
      return 1;
    else
      return dRes;
    endif
  endif

  return RetError("Nie znaleziono Nr ["+Nr+"]");

endfunction

//-----------------------------------------------------------------------------

function GetBaseItemData()

  var Data := OpenDataFile(BASE_ITEM_DATA);
  if(!Data)
    Data := CreateDataFile(BASE_ITEM_DATA);
    Data.CreateElement(GBIELEM_EMPTY);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function FindBaseItemLoc(Nr)

  var Data := GetBaseItemData();
  var Elem := Data.FindElement(CStr(Nr));

  if(!Elem)
    var Err := error;
    Err.errortext := "Nie znaleziono Nr ["+Nr+"]";
    return Err;
  endif

  var Loc  := Elem.GetProp(BIPROP_LOC);
  
  if(!Loc[4])
    Loc[4] := _DEFAULT_REALM;
  endif
  
  return Loc;

endfunction

//-----------------------------------------------------------------------------

function DestroyBaseCreatedItem(Nr,Elem := 0)

  if(!Elem)
    Elem := FindBaseItemElem(Nr);
  endif
  
  if(Elem)
    var Loc := Elem.GetProp(BIPROP_LOC);
    
    if(!Loc[4])
      Loc[4] := _DEFAULT_REALM;
    endif
    
    var BItem := 0;
    
    foreach item in ListItemsNearLocationOfType(CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),0,CInt(Elem.GetProp("ObjType")),Loc[4])
      if(GetItemBINr(Item) == CInt(Nr))
        BItem := Item;
        break;
      endif
    endforeach
    
    if(!BItem)
      BItem := FindItemBySerial(Elem.GetProp(BIPROP_SERIAL));
    endif
    
    if(BItem)
      if(DestroyItem(BItem))
        Elem.SetProp(BIPROP_SERIAL, 0);
        return 1;
      else
        return RetError("Nie udalo sie zniszczyc BI, Nr ["+Nr+"]: ["+BItem.serial+"]");
      endif
    else
      return -1;
    endif
  endif

  return RetError("Nie znaleziono BI, Nr ["+Nr+"]");
  
endfunction

//-----------------------------------------------------------------------------

function DestroyBaseItem(Nr)

  var Elem := FindBaseItemElem(Nr);
  
  if(Elem)
    var dRes := DestroyBaseCreatedItem(Nr,Elem);
    if(dRes)
      return DeleteBaseItem(Nr);
    else
      return dRes;
    endif
  else
    return RetError("Nie znaleziono BI, Nr ["+Nr+"]");
  endif

endfunction

//-----------------------------------------------------------------------------

function FindBaseCreatedItem(Nr)

  var Elem    := FindBaseItemElem(Nr);

  if(!Elem)
    return RetError("Nie znaleziono BI, Nr ["+Nr+"]");
  endif

  var Loc     := Elem.GetProp(BIPROP_LOC);
  var ObjType := Elem.GetProp(BIPROP_OBJTYPE);
  
  if(!Loc[4])
    Loc[4] := _DEFAULT_REALM;
  endif
  
  foreach item in ListItemsNearLocationOfType(CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),0,ObjType,Loc[4])
    if(GetItemBINr(item) == CInt(Nr))
      return item;
    endif
  endforeach

  var Ser := Elem.GetProp(BIPROP_SERIAL);

  if(Ser)
    var Item := FindItemBySerial(Ser);
    if(Item)
      if(GetItemBINr(item) == CInt(Nr))
        return Item;
      endif
    endif
  endif

  return RetError("Przedmiot BI, Nr ["+Nr+"] Nie istnieje.");

endfunction

//-----------------------------------------------------------------------------

function CreateBaseItem(Nr)

  var NewItem := 0;
  var Data    := GetBaseItemData();
  var Elem    := Data.FindElement(CStr(Nr));

  if(!Elem)
    return RetError("Nie znaleziono BI, Nr ["+Nr+"]");
  endif

  var Loc     := Elem.GetProp(BIPROP_LOC);
  var ObjType := Elem.GetProp(BIPROP_OBJTYPE);
  
  if(!Loc[4])
    Loc[4] := _DEFAULT_REALM;
  endif

  foreach item in ListItemsNearLocationOfType(CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),0,ObjType,Loc[4])
    if(GetItemBINr(item) == CInt(Nr))
      NewItem := Item;
    elseif(Item.z == CInt(Loc[3]))
      if(GetItemBINr(Item))
        DestroyBaseItem(GetItemBINr(Item));
      else
        DestroyItem(Item);
      endif
    endif
  endforeach

  if(!NewItem)
    NewItem := FindItemBySerial(Elem.GetProp(BIPROP_SERIAL));
  endif
  
  if(!NewItem)
    NewItem := CreateItemAtLoc(CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),ObjType,1,Loc[4]);
      
    if(!NewItem)
      LogError("CreateBaseItem: Nr ["+Nr+"] O: ["+ObjType+"] R: ["+NewItem.errortext+"]", LOGF_WORLD);
      return RetError("Nr ["+Nr+"] O: ["+ObjType+"] R: ["+NewItem.errortext+"]");
    endif

    var Color := Elem.GetProp(BIPROP_COLOR);
    var Name  := Elem.GetProp(BIPROP_NAME);
    var Flags := Elem.GetProp(BIPROP_FLAGS);

    if(Color)
      NewItem.color := Color;
    endif

    if(Name)
      SetName(NewItem, Name);
    endif

    if(Flags & BIF_INVIS)
      NewItem.invisible := 1;
    endif

    if(Flags & BIF_LOCKED)
      NewItem.locked := 1;
    endif

    NewItem.movable := 0;
    SetItemBINr(NewItem, Nr);
    Elem.SetProp(BIPROP_SERIAL, NewItem.serial);
    return NewItem;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetItemBiNr(Item,Nr)

  SetObjProperty(Item, BIPROP_ITEMNR,Nr);

endfunction

//-----------------------------------------------------------------------------

function CreateAllBaseItems(who := 0,Cmd := 0)

  var Data := GetBaseItemData();
  var Num  := 0;
  var Err  := 0;
  var Ex   := 0;
  var Item;
  var AllItems := Data.Keys();
  var List := array;
  var Proc, lProc, n := 0;
  
  if(Who)
    SendSysMessage(who,"Tworzenie "+(AllItems.size()-1)+" BaseItemow.",FONT_NORMAL,COLOR_GREEN);
  endif
  
  foreach Nr in AllItems
    if(CInt(Nr))
      Item := CreateBaseItem(Nr);
      if(Item == 1)
        Ex := Ex + 1;
      elseif(!Item)
        Err := Err + 1;
        List.append(array(Nr, Item.errortext, 1204));
      else
        if(who and Cmd)
          List.append(array(Nr, Item.serial+" ["+Item.x+","+Item.y+","+Item.z+" "+Item.realm+"]", 1419));
        endif
        Num := Num + 1;
      endif
    endif
    Proc := (n * 100) / AllItems.size();
    if(Proc != lProc and !(Proc % 10))
      SendSysMessage(who, "["+Proc+"%] Cr: "+Num+" Err: "+Err+" cNr: "+Nr, FONT_NORMAL, COLOR_GREEN);
    endif
    lProc := Proc;
    n := n + 1;
  endforeach
  
  if(who)
    SendSysMessage(who,"Stworzono "+Num+" z "+(AllItems.size()-1)+" BI, "+Ex+" istniejacych, "+Err+" blednych.",FONT_NORMAL,COLOR_GREEN);
    if(List.size())
      DisplayBIResult(who, List);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function DisplayBIResult(who, byref List)

  GInit(30, 30);
  GMultiHeaderInit();
  GResPic(0, 0, 3600, 750, 460);
  GAddPageData(718,16,2650,2651,718,428,2648,2647);
  GTextLine(100, 15, 999, "Wynik");
  GMultiPage();

  var n := 1;

  foreach Entry in List
    GCheckNextPage(20);
    GTextLine(40,  GNPCount()*20+40, 999, n);
    GTextLine(80,  GNPCount()*20+40, 670, Entry[1]);
    GTextLine(140, GNPCount()*20+40, Entry[3], Entry[2]);
    n := n + 1;
  endforeach

  GSend(who);

endfunction

//-----------------------------------------------------------------------------

function DeleteAllBaseItems(who)

  SendSysMessage(who,"Napewno chcesz skasowac wszystkie BI ?",FONT_NORMAL,COLOR_GREEN);
  if(!YesNo(who,"Tak?"))
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Data := GetBaseItemData();
  var Num  := 0;
  var Err  := 0;
  var Item;
  var AllItems := Data.Keys();

  if(Who)
    SendSysMessage(who,"Calkowite kasowanie "+(AllItems.size()-1)+" BaseItemow.",FONT_NORMAL,COLOR_GREEN);
  endif
  foreach Nr in AllItems
    if(CInt(Nr))
      if(!DestroyBaseItem(Nr))
        Err := Err + 1;
      else
        Num := Num + 1;
      endif
    endif
  endforeach

  Item := Data.FindElement(GBIELEM_EMPTY);
  Item.SetProp(GBIPROP_EMPTYIDS,{});

  if(who)
    SendSysMessage(who,"Skasowano "+Num+" z "+(AllItems.size()-1)+" BI, "+Err+" blednych.",FONT_NORMAL,COLOR_GREEN);
  endif

endfunction

//-----------------------------------------------------------------------------

function DestroyAllBaseItems(who := 0)

  var Data := GetBaseItemData();
  var Num  := 0;
  var Err  := 0;
  var AllItems := Data.Keys();

  if(who)
    SendSysMessage(who,"Kasowanie "+(len(AllItems)-1)+" BaseItemow.",FONT_NORMAL,COLOR_GREEN);
  endif

  foreach Nr in AllItems
    if(CInt(Nr))
      if(DestroyBaseCreatedItem(Nr))
        Num := Num + 1;
      else
        Err := Err + 1;
      endif
    endif
  endforeach
  
  if(who)
    SendSysMessage(who,"Skasowano "+Num+" z "+(AllItems.size()-1)+" BI, "+Err+" nie istniejacych.",FONT_NORMAL,COLOR_GREEN);
  endif

endfunction

//-----------------------------------------------------------------------------

function UpdateBaseItem(Item, who := 0)

  var Data := GetBaseItemData();
  var Nr   := GetItemBINr(Item);

  if(!Nr)
    if(who)
      SendSysMessage(who, "To nie jest BI.",FONT_NORMAL,COLOR_RED);
    endif
    return RetError("To nie jest BI.");
  endif

  if(!Item.isa(POLCLASS_ITEM))
    if(who)
      SendSysMessage(who, "To nie przedmiot.",FONT_NORMAL,COLOR_RED);
    endif
    return RetError("To nie przedmiot.");
  endif

  var Elem := Data.FindElement(CStr(Nr));

  if(!Elem)
    EraseObjProperty(Item, BIPROP_ITEMNR);
    if(who)
      SendSysMessage(who, "Przedmiot posiada nie istniejacy BI Nr! Kasowanie numeru.",FONT_NORMAL,COLOR_RED);
    endif
    return RetError("Przedmiot posiada nie istniejacy BI Nr! Kasowanie numeru.");
  endif

  var ObjType := Elem.GetProp(BIPROP_OBJTYPE);

  if(ObjType != Item.objtype)
    if(who)
      SendSysMessage(who, "Przedmiot ma nieodpowiedni ObjType: ["+Item.objtype+" != "+ObjType+"]",FONT_NORMAL,COLOR_RED);
    endif
    var Err := error;
    Err.errortext := "Przedmiot ma nieodpowiedni ObjType: ["+Item.objtype+" != "+ObjType+"]";
    DestroyItem(Item);
    return Err;
  endif

  var Color   := Elem.GetProp(BIPROP_COLOR);
  var Loc     := Elem.GetProp(BIPROP_LOC);
  var Flags   := Elem.GetProp(BIPROP_FLAGS);
  var Serial  := Elem.GetProp(BIPROP_SERIAL);
  var Name    := Elem.GetProp(BIPROP_NAME);
  var iName   := CheckBaseItemName(Item);
  var Changed := 0;
  
  if(!Loc[4])
    Loc[4] := _DEFAULT_REALM;
  endif

  if(who)
    SendSysMessage(who,"Zmieniono dane BI, Nr ["+Nr+"]",FONT_NORMAL,COLOR_GREEN);
  endif

  if(Color != Item.color)
    Changed := 1;
    if(who)
      SendSysMessage(who,"Clr: ["+Color+"] > ["+Item.color+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  if(Item.isa(POLCLASS_LOCKABLE))
    if(Item.locked != IsBitOn(Flags, BIF_LOCKED))
      Changed := 1;
      if(who)
        SendSysMessage(who,"Lock: ["+IsBitOn(Flags, BIF_LOCKED)+"] > ["+Item.locked+"]",FONT_NORMAL,COLOR_GREEN);
      endif
    endif
  endif

  if(iName != Name)
    Changed := 1;
    if(who)
      SendSysMessage(who,"Name: ["+Name+"] > ["+iName+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  if(Item.invisible != IsBitOn(Flags, BIF_INVIS))
    Changed := 1;
    if(who)
      SendSysMessage(who,"Invis: ["+IsBitOn(Flags, BIF_INVIS)+"] > ["+Item.invisible+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  if(Serial != Item.serial)
    Changed := 1;
    if(who)
      SendSysMessage(who,"Ser: ["+Serial+"] > ["+Item.serial+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  if(Loc[1] != item.x or Loc[2] != item.y or Loc[3] != item.z or Loc[4] != item.realm)
    Changed := 1;
    if(who)
      SendSysMessage(who,"Loc: ["+Loc[1]+","+Loc[2]+","+Loc[3]+" "+Loc[4]+"] > ["+item.x+","+Item.y+","+Item.z+" "+Item.realm+"]",FONT_NORMAL,COLOR_GREEN);
    endif
  endif

  SetBitOff(Flags, BIF_LOCKED);
  SetBitOff(Flags, BIF_INVIS);
  
  if(Item.locked)
    SetBitOn(Flags, BIF_LOCKED);
  endif

  if(Item.invisible)
    SetBitOn(Flags, BIF_INVIS);
  endif
  
  Elem.SetProp(BIPROP_COLOR,   Item.color);
  Elem.SetProp(BIPROP_FLAGS,   Flags);
  Elem.SetProp(BIPROP_NAME,    iName);
  Elem.SetProp(BIPROP_LOC,     array(Item.x,Item.y,Item.z,Item.realm));

  return Changed;

endfunction

//-----------------------------------------------------------------------------

function UpdateBaseItemFromData(Nr,who := 0, uFlags := 0)

  if(!Nr)
    if(who)
      SendSysMessage(who,"Bledny Numer.",FONT_NORMAL,COLOR_RED);
    endif
    return RetError("Bledny Numer.");
  endif

  var Item := FindBaseCreatedItem(Nr);

  if(!Item)
    return 0;
  endif

  var Data := GetBaseItemData();
  var Elem := Data.FindElement(CStr(Nr));

  if(!Elem)
    if(who)
      SendSysMessage(who, "BI Nr ["+Nr+"] nie istnieje.", FONT_NORMAL, COLOR_RED);
    endif
    return RetError("BI Nr ["+Nr+"] nie istnieje.");
  endif

  var ObjType := Elem.GetProp(BIPROP_OBJTYPE);

  if(ObjType != Item.objtype)
    DestroyItem(Item);
    var Res := CreateBaseItem(Nr);
    if(Res)
      if(who)
        SendSysMessage(who, "Stworzono BI, Nr ["+Nr+"]", FONT_NORMAL, COLOR_GREEN);
      endif
      return 1;
    else
      if(who)
        SendSysMessage(who, "Blad podczas tworzenia BI, Nr ["+Nr+"]: "+Res.errortext, FONT_NORMAL, COLOR_RED);
      endif
      return RetError("Blad podczas tworzenia BI, Nr ["+Nr+"]: "+Res.errortext);
    endif
  endif
  
  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var Color     := Elem.GetProp(BIPROP_COLOR);
  var Flags     := Elem.GetProp(BIPROP_FLAGS);
  var Name      := Elem.GetProp(BIPROP_NAME);
  var Loc       := Elem.GetProp(BIPROP_LOC);
  var iName     := CheckBaseItemName(Item);
  var Graphic   := ItemCfg[ObjType].graphic;
  var Changed   := 0;

  if(!Graphic)
    Graphic := ObjType;
  endif

  if(!Name or Name == "0")
    Name := GetDescForItem(item.objtype);
  endif
  
  if(!Loc[4])
    Loc[4] := _DEFAULT_REALM;
  endif

  if(who)
    SendSysMessage(who,"Update BI, Nr ["+Nr+"]",FONT_NORMAL,COLOR_GREEN);
  endif

  if(Color)
    if(Color != item.color)
      if(who)
        SendSysMessage(who,"Clr: ["+item.color+"] > ["+Color+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      item.color := Color;
      Changed    := 1;
    endif
  elseif(ItemCfg[item.objtype].Color != item.color and ItemCfg[item.objtype].Color)
    if(who)
      SendSysMessage(who,"Clr: ["+item.color+"] > ["+ItemCfg[item.objtype].Color+"]",FONT_NORMAL,COLOR_GREEN);
    endif
    item.color := ItemCfg[item.objtype].Color;
    Changed    := 1;
  elseif(!ItemCfg[item.objtype].Color and Item.color)
    if(who)
      SendSysMessage(who,"Clr: ["+item.color+"] > [0]",FONT_NORMAL,COLOR_GREEN);
    endif
    item.color := 0;
    Changed    := 1;
  endif

  if(Graphic != Item.graphic)
    if((uFlags & BIUF_HARD) or !HaveBaseItemGrapChanged(Item, Graphic))
      if(who)
        SendSysMessage(who,"Grap: ["+item.graphic+"] > ["+Graphic+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      Item.graphic := Graphic;
      Changed      := 1;
    endif
  endif

  if(Item.isa(POLCLASS_LOCKABLE) and !GetItemLockID(Item))
    if(Item.locked != IsBitOn(Flags, BIF_LOCKED))
      if(who)
        SendSysMessage(who,"Lock: ["+Item.locked+"] > ["+IsBitOn(Flags, BIF_LOCKED)+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      item.locked := IsBitOn(Flags, BIF_LOCKED);
      Changed     := 1;
    endif
  endif

  if(iName != Name and Name and iName and ItemCfg[item.objtype].desc)
    if(who)
      SendSysMessage(who,"Name: ["+iName+"] > ["+Name+"]",FONT_NORMAL,COLOR_GREEN);
    endif
    SetName(item, Name);
    Changed := 1;
  endif

  if(item.invisible != IsBitOn(Flags, BIF_INVIS))
    if(who)
      SendSysMessage(who,"I: ["+Item.invisible+"] > ["+IsBitOn(Flags, BIF_INVIS)+"]",FONT_NORMAL,COLOR_GREEN);
    endif
    item.invisible := IsBitOn(Flags, BIF_INVIS);
    Changed := 1;
  endif
  
  if(Loc[1] != Item.x or Loc[2] != Item.y or Loc[3] != Item.z or Loc[4] != Item.realm)
    if((uFlags & BIUF_HARD) or !HaveBaseItemLocChanged(Item, Loc[1], Loc[2], Loc[3]) or Loc[4] != Item.realm)
      if(who)
        SendSysMessage(who,"Loc: ["+item.x+","+Item.y+","+Item.z+" "+Item.realm+"] > ["+Loc[1]+","+Loc[2]+","+Loc[3]+" "+Loc[4]+"]",FONT_NORMAL,COLOR_GREEN);
      endif
      MoveObject(Item,Loc[1],Loc[2],Loc[3],Loc[4],MOVEITEM_FORCELOCATION | MOVEITEM_IGNOREMOVABLE);
      item.movable := 0;
      Changed      := 1;
    endif
  endif
  
  return Changed;

endfunction

//-----------------------------------------------------------------------------

function HaveBaseItemGrapChanged(Item, Graphic)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(Item.isa(POLCLASS_DOOR))
    if(Graphic+1 == Item.graphic)
      return 1;
    endif
  elseif(ItemCfg[Item.objtype].change)
    if(ItemCfg[Item.objtype].change == Item.graphic)
      return 1;
    endif
  endif

  case(Graphic)
    0xa97: return Item.graphic == 0xa9d;
    0xa98: return Item.graphic == 0xa9d;
    0xa9b: return Item.graphic == 0xa9d;
    0xa99: return Item.graphic == 0xa9e;
    0xa9a: return Item.graphic == 0xa9e;
    0xa9c: return Item.graphic == 0xa9e;

    0xa9d: case(Item.graphic)
             0xa97:  return 1;
             0xa98:  return 1;
             0xa9b:  return 1;
           endcase

    0xa9e: case(Item.graphic)
             0xa99:  return 1;
             0xa9a:  return 1;
             0xa9c:  return 1;
           endcase
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveBaseItemLocChanged(Item, x, y, z)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(Item.isa(POLCLASS_DOOR) and IsOpened(Item))
    if(x + ItemCfg[Item.objtype].xmod == Item.x and y + ItemCfg[Item.objtype].ymod == Item.y and Item.z == z)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckBaseItemName(Item)

  var IName := Item.desc;

  if(IName[1,2] == "a ")
    IName := IName[3,len(IName)-3];
  elseif(IName[1,3] == "an ")
    IName := IName[4,len(IName)-4];
  endif

  if(GetDescForItem(item.objtype) != IName)
    return IName;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function UpdateItemFromData(who, uFlags)

  var Item := LocTarget(who).item;
  if(Item.isa(POLCLASS_ITEM))
    UpdateBaseItemFromData(GetItemBINr(Item), who, uFlags);
  else
    SendSysMessage(who, "Anulowano.", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function UpdateDataFromItem(who)

  var Item := LocTarget(who).item;
  if(Item.isa(POLCLASS_ITEM))
    UpdateBaseItem(Item,who);
  else
    SendSysMessage(who, "Anulowano.", FONT_NORMAL, COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

function UpdateAllItemsFromData(who, uFlags)

  SendSysMessage(who,"Chcesz uaktualnic wszystkie BI z biezacej daty?",FONT_NORMAL,COLOR_GREEN);
  if(!YesNo(who,"Na pewno?"))
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Data     := GetBaseItemData();
  var Num      := 0;
  var Err      := 0;
  var AllItems := Data.Keys();
  var List     := array;
  var Res;
  var Proc, lProc, n := 0;
  
  foreach Nr in (AllItems)
    if(CInt(Nr))
      Res := UpdateBaseItemFromData(Nr, 0, uFlags);
      if(Res)
        Num := Num + 1;
        List.append(array(Nr, "Item Updated", 1419));
      elseif(Res == error)
        List.append(array(Nr, Res.errortext, 1204));
        Err := Err + 1;
      endif
    endif
    Proc := (n * 100) / AllItems.size();
    if(Proc != lProc and !(Proc % 10))
      SendSysMessage(who, "["+Proc+"%] UP: "+Num+" cNr: "+Nr, FONT_NORMAL, COLOR_GREEN);
    endif
    lProc := Proc;
    n := n + 1;
  endforeach

  SendSysMessage(who,"Uaktualniono "+Num+" itemow z "+(AllItems.size()-1)+", "+Err+" bledow.",FONT_NORMAL,COLOR_GREEN);
  if(List.size())
    DisplayBIResult(who, List);
  endif

endfunction

//-----------------------------------------------------------------------------

function UpdateDataFromAllItems(who)

  SendSysMessage(who,"Chcesz uaktualnic date z biezacych BI?",FONT_NORMAL,COLOR_GREEN);
  if(!YesNo(who,"Na pewno?"))
    SendSysMessage(who,"Anulowano.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var Data     := GetBaseItemData();
  var Num      := 0;
  var Err      := 0;
  var AllItems := Data.Keys();
  var List     := array;
  var Res;
  var Proc, lProc, n := 0;

  foreach Nr in (AllItems)
    if(CInt(Nr))
      var Item := FindBaseCreatedItem(Nr);
      if(Item)
        Res := UpdateBaseItem(Item);
        if(Res)
          Num := Num + 1;
          List.append(array(Nr, "Data Updated", 1419));
        elseif(Res == error)
          List.append(array(Nr, Res.errortext, 1204));
          Err := Err + 1;
        endif
      endif
    endif
    Proc := (n * 100) / AllItems.size();
    if(Proc != lProc and !(Proc % 10))
      SendSysMessage(who, "["+Proc+"%] UP: "+Num+" cNr: "+Nr, FONT_NORMAL, COLOR_GREEN);
    endif
    lProc := Proc;
    n := n + 1;
  endforeach

  SendSysMessage(who,"Uaktualniono date z "+Num+" itemow z "+(AllItems.size()-1)+", "+Err+" bledow.",FONT_NORMAL,COLOR_GREEN);
  if(List.size())
    DisplayBIResult(who, List);
  endif

endfunction

//-----------------------------------------------------------------------------

function GoToBaseItem(who,Nr)

  if(CInt(Nr))
    var Loc := FindBaseItemLoc(Nr);
    if(Loc)
      MoveObject(who,CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),Loc[4],MOVECHAR_FORCELOCATION);
      SendSysMessage(who,"Przenoszenie do BI Nr ["+Nr+"] ["+Loc[1]+","+Loc[2]+","+Loc[3]+" "+Loc[4]+"].",FONT_NORMAL,COLOR_GREEN);
      return 1;
    else
      SendSysMessage(who,"BI Nr ["+Nr+"] nie istnieje.",FONT_NORMAL,COLOR_RED);
    endif
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function FindBaseItemElem(Nr)

  var Data := GetBaseItemData();
  return Data.FindElement(CStr(Nr));

endfunction
//-----------------------------------------------------------------------------

function AddExistingItemToBICmd(who)

  var Item := LocTarget(who).item;

  var Res := AddExistingItemToBI(Item, who);

  if(!Res)
    SendSysMessage(who, Res.errortext, FONT_NORMAL, COLOR_RED);
  endif

  return Res;
  
endfunction

//-----------------------------------------------------------------------------

function AddExistingItemToBI(Item, who := 0)

  var Res  := CanBeBaseItem(Item);
  if(!Res)
    return Res;
  endif

  var ObjType := Item.objtype;
  var Name    := CheckBaseItemName(item);
  var Color   := Item.color;
  var x       := item.x;
  var y       := item.y;
  var z       := item.z;
  var Rlm     := item.realm;
  var Flags   := 0;
  
  if(Item.locked)
    SetBitOn(Flags, BIF_LOCKED);
  endif
  
  if(Item.invisible)
    SetBitOn(Flags, BIF_INVIS);
  endif

  var Nr := AddBaseItem(ObjType, x, y, z, Rlm, Color, Name, Flags);
  
  if(Nr)
    if(who)
      SendSysMessage(who,"Dodano Bi, Nr ["+Nr+"]  O: ["+ObjType+"]  N: ["+Name+"] C: ["+Color+"] L: ["+IsBitOn(Flags, BIF_LOCKED)+"] I: ["+IsBitOn(Flags, BIF_INVIS)+"]. ["+x+","+y+","+z+" "+Rlm+"]",FONT_NORMAL,COLOR_GREEN);
    endif
    var Elem := FindBaseItemElem(Nr);
    Elem.SetProp(BIPROP_SERIAL, Item.serial);
    SetItemBINr(Item, Nr);
    Item.movable := 0;
    return 1;
  else
    return RetError("Nie udalo sie dodac BI, ["+Nr+"].");
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanBeBaseItem(Item)

  if(!Item.isa(POLCLASS_ITEM))
    return RetError("To nie przedmiot.");
  endif

  if(GetItemBINr(Item))
    return RetError("To jest juz BI.");
  endif
  
  if(IsWms(Item))
    return RetError("Przedmiot jest statyczny.");
  endif

  if(Item.container)
    return RetError("Przedmiot znajduje sie w pojemniku.");
  endif

  if(CheckIsHouseItem(Item))
    return RetError("Przedmiot znajduje sie w multi (domu).");
  endif
  
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[Item.objtype].CantBeBase)
    return RetError("Ten przedmiot nie moze byc BI.");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
