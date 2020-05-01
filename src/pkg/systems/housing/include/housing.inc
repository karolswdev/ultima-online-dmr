//-----------------------------------------------------------------------------

const HOUSE_MAX_DECAY_TIME        := 1814400;  // 21 days
const DEFAULT_HOUSE_NAME          := "dom";
const MAX_HOUSE_VENDORS           := 5;
const MAX_HOUSES_ON_PLAYER        := 4;
const PUBLIC_HOUSE_DELAY          := 0;

//-----------------------------------------------------------------------------

include "include/finding";
include "include/player/target";
include "include/findcity";

include ":housing:common";
include ":housing:finding";
//-----------------------------------------------------------------------------

function IsItemInsideHouse(item,house)

  if(item.multi.serial == house.serial)
    return 1;
  endif

  foreach itm in (house.items)
    if(itm == item)
      return 1;
    endif
  endforeach

  if(GetStand(Item.x, Item.y, Item.realm).multi.serial == house.serial)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckCharList(List)

  var i := 1;
  for(i:=1;i<=Cint(len(List));i:=i+1)
    if(!FindPlayerBySerial(Cint(List[i])))
      List.erase(i);
    endif
  endfor

  return List;

endfunction

//-----------------------------------------------------------------------------

function SetHouseCoOwners(house,CoOwners)

  SetObjProperty(house,"CoOwners",CoOwners);

endfunction

//-----------------------------------------------------------------------------

function SetHouseFriends(house,Friends)

  SetObjProperty(house,"Friends",Friends);

endfunction

//-----------------------------------------------------------------------------

function ClearHousePrivilegesForAcc(Acc)

  var i, Char;

  for(i:=1;i<=5;i:=i+1)

    Char := Acc.GetCharacter(i);

    if(Char)
      ClearHousePrivileges(Char);
    endif

  endfor

endfunction

//-----------------------------------------------------------------------------

function ClearHousePrivileges(who)

  var Houses := FindHousesForPlayer(who, GNHOF_COOWNER | GNHOF_FRIEND);

  foreach House in Houses

    if(IsHouseCoOwner(who, House))
      RemoveHouseCoOwner(House, who);
    endif
    
    if(IsHouseFriend(who, House))
      RemoveHouseFriend(House, who);
    endif

  endforeach

endfunction

//-----------------------------------------------------------------------------

function RemoveHouseCoOwner(House, who)

  var CoOwners := GetHouseCoOwners(House);
  
  var Idx := RetIndex(CoOwners, who.serial);

  if(Idx)
    CoOwners.erase(Idx);
    SetHouseCoOwners(House,CoOwners);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function RemoveHouseFriend(House, who)

  var Friends := GetHouseFriends(House);
  
  var Idx := RetIndex(Friends, who.serial);

  if(Idx)
    Friends.erase(Idx);
    SetHouseFriends(House, Friends);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseObjtype( deed )

  var DeedObjType := deed;
  if(deed.isa(POLCLASS_ITEM))
    DeedObjType := deed.objtype;
  endif

  var id   := ReadConfigFile( ":housing:itemdesc" );
  var elem := id[ DeedObjType ];
  PrintTextAbove(deed, "My HouseObjType is " + elem.HouseObjType);
  return GetObjtypeByName( elem.HouseObjType );

endfunction

//-----------------------------------------------------------------------------

function GetSecureNum(house)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  if(GetObjProperty(house,"SecNum") == error)
    var c := 0;
    foreach Item in ListItemsNearLocation(house.x,house.y,house.z,50,house.realm)
      if(IsSecured(Item) == house.serial)
        c := c + 1;
      endif
    endforeach
    SetObjProperty(house,"SecNum",c);
  endif

  return CInt(GetObjProperty(house,"SecNum"));

endfunction

//-----------------------------------------------------------------------------

function GetSecureMaxNum(house)

  var ItemCfg := ReadConfigFile(":housing:itemdesc");
  var DeedObj := GetDeedObjTypeByHouseType(house.objtype);

  return CInt(ItemCfg[DeedObj].NumSecure);

endfunction

//-----------------------------------------------------------------------------

function GetLockDownsNum(house)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  if(GetObjProperty(house,"LockNum") == error)
    var c := 0;
    foreach Item in ListItemsNearLocation(house.x,house.y,house.z,50,house.realm)
      if(IsLockedDown(Item) == house.serial)
        c := c + 1;
      endif
    endforeach
    SetObjProperty(house,"LockNum",c);
  endif

  return CInt(GetObjProperty(house,"LockNum"));

endfunction

//-----------------------------------------------------------------------------

function GetLockDownsMaxNum(house)

  var ItemCfg := ReadConfigFile(":housing:itemdesc");
  var DeedObj := GetDeedObjTypeByHouseType(house.objtype);

  return CInt(ItemCfg[DeedObj].NumLockDowns);

endfunction

//-----------------------------------------------------------------------------

function SetItemSecured(item,house)

  if(GetSecureNum(house) >= GetSecureMaxNum(house) or IsSecured(item))
    return 0;
  endif

  SetHouseItem(item,house);
  SetObjProperty(item,"secure",house.serial);
  if(item.script)
    SetObjProperty(item,"oldscript",item.script);
  endif

  item.usescript := ":housing:secureCont";
  SetObjProperty(house,"SecNum",GetSecureNum(house) + 1);
  item.movable := 0;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function EraseSecured(item,house)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  if(!IsSecured(item))
    return 0;
  endif

  EraseHouseItem(item);
  EraseObjProperty(item,"secure");
  if(GetObjProperty(item,"oldscript"))
    item.usescript := GetObjProperty(item,"oldscript");
  else
    item.usescript := "";
  endif

  EraseObjProperty(item,"oldscript");
  SetObjProperty(house,"SecNum",GetSecureNum(house) - 1);

  item.decayat := ReadGameClock() + ITEM_DECAY_TIME;
  item.movable := 1;

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetItemLockDown(item,house)

  if(GetLockDownsNum(house) >= GetLockDownsMaxNum(house) or IsLockedDown(item))
    return 0;
  endif

  SetHouseItem(item,house);
  SetObjProperty(item,"lockeddown",house.serial);
  item.movable := 0;
  SetObjProperty(house,"LockNum",GetLockDownsNum(house) + 1);
  
  if(item.isa(POLCLASS_CONTAINER))
    var ContNum := CInt(GetObjProperty(house, "LockedConts"));
    SetObjProperty(house, "LockedConts", ContNum+1);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function EraseItemLockDown(item,house)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  if(!IsLockedDown(item))
    return 0;
  endif

  EraseHouseItem(item);
  EraseObjProperty(item,"lockeddown");
  item.decayat := ReadGameClock() + ITEM_DECAY_TIME;
  item.movable := 1;
  SetObjProperty(house,"LockNum",GetLockDownsNum(house) - 1);

  if(item.isa(POLCLASS_CONTAINER))
    var ContNum := CInt(GetObjProperty(house, "LockedConts"));
    SetObjProperty(house, "LockedConts", ContNum-1);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetHouseOwner(house,NewOwner)

  var Owner := GetHouseOwner(House);
  var List  := GetHouseOwnerList(Owner);
  var Idx   := RetIndex(List, House.serial);
  if(Idx)
    List.erase(Idx);
    SetObjProperty(Owner, "HouseSers", List);
  endif

  SetObjProperty(house,"ownerserial",NewOwner.serial);
  SetObjProperty(house, "owneracc", NewOwner.acctname);
  AddHouseToOwnerList(House);

endfunction

//-----------------------------------------------------------------------------

function GetHouseComponents(house)

  return GetObjProperty(house,"Components");

endfunction

//-----------------------------------------------------------------------------

function GetHouseName(house)

  var HouseName := GetObjProperty(house,"HouseName");
  if(!HouseName)
    HouseName := "";
    SetObjProperty(house,"HouseName",HouseName);
  endif

  return HouseName;

endfunction

//-----------------------------------------------------------------------------

function SetHouseName(house,NewName)

  if(NewName)
    SetObjProperty(house,"HouseName",NewName);
  endif

endfunction

//-----------------------------------------------------------------------------

function FindHouseDoors(house)

  var Doors := {};
  foreach item in (house.components)
    if(item.isa(POLCLASS_DOOR))
      Doors.append(item);
    endif
  endforeach

  foreach item in GetHouseComponents(house)
    item := FindItemBySerial(Item);
    if(item.isa(POLCLASS_DOOR))
      Doors.append(item);
    endif
  endforeach

  return Doors;

endfunction

//-----------------------------------------------------------------------------

function MoveItemToEscrow(Item, byref EscrData, bForce)

  if(!Item) // moze item juz nie istniec, jesli jakas rozbiorka deeda zniszczyla wszystkie czesci
    return -1;
  endif

  // tutaj ladujemy do specjalnych plecakow itemy - jak nie da rady to tworzymy nowy i tak dalej
  var curBackpack := GetEscrowBackpack(EscrData);

  if(!curBackpack)
    if(bForce)
      DestroyItem(Item);
      return 1;
    endif

    return error;
  endif

  var oldMovable := Item.movable;
    
  if(!Item.movable)
    Item.movable := 1;
  endif

  if(IsFenceSign(Item))

    var Deed := DestroyFence(curBackpack, Item, 1);

    if(!Deed)
      LogError("[MoveItemToEscrow] Cos sie stalo ze sie skopalo! Nie udalo sie rozebrac deeda plota Escrow: '"+GetName(EscrData.curBox)+"' przy jego niszczeniu. Item: "+Item.serial+"/"+Item.objtype+"/"+GetName(Item));
      Item.movable := oldMovable;
      return 0;
    endif

    return 1;

  elseif(IsItemFromDeed(Item))

    var Deed := DestroyDeedItem(Item, 0, curBackpack, 1);

    if(!Deed)
      LogError("[MoveItemToEscrow] Cos sie stalo ze sie skopalo! Nie udalo sie rozebrac deeda Escrow: '"+GetName(EscrData.curBox)+"' przy jego niszczeniu. Item: "+Item.serial+"/"+Item.objtype+"/"+GetName(Item));
      Item.movable := oldMovable;
      return 0;
    endif
    
    return 1;

  elseif(IsCarpetRollMain(Item))

    CarpetRollBack(Item, curBackpack);
    return 1;

  endif

  // tutaj funkcja nam zwroci aktualny backpack - sprawdzi czy nie jest pelny i ew zrobi nowy.
  var Count := 0;
  var MoveItemRes := 0;

  while(!MoveItemRes and Count < 5)
    
    MoveItemRes := MoveItemToContainer(Item, curBackpack);

    if(!MoveItemRes and IsStackableObj(Item.objtype) and Item.amount > 1)
      var ItemWeight := GetItemWeight(Item);

      if(EscrData.curBox.weight + ItemWeight < 65535)
        var LeftWeight := 65535 - EscrData.curBox.weight;
        var Amount := ToMax(CInt(LeftWeight / ItemWeight), Item.amount);

        if(Amount > 0)
          var DupItem := CreateItemCopyAtLoc(CR_TEMP_ITEM_X, CR_TEMP_ITEM_Y, CR_TEMP_ITEM_Z, Item, CR_TEMP_ITEM_REALM);
          var PrevAmount := Item.amount;

          if(DupItem)
            if(SubtractAmount(DupItem, PrevAmount - Amount))
              if(SubtractAmount(Item, Amount))
                if(!MoveItemToContainer(DupItem, curBackpack))
                  // kurwa no, to juz przesada! nadal zle? czy to juz zahacza o zalamanie czaso przestrzeni ?
                  CheckEscrowData(EscrData, DupItem.weight);
                  DestroyItem(DupItem);
                  AddAmount(Item, Amount);
                endif
                
                Count += 1;
                continue;
              else
                DestroyItem(DupItem);
              endif
            else
              DestroyItem(DupItem);
            endif
          endif
        endif
      endif
    endif


    CheckEscrowData(EscrData, Item.weight);
    curBackpack := GetEscrowBackpack(EscrData);

    if(!Item)
      break;
    endif
    
    if(!MoveItemRes)
      Count += 1;
    endif

  endwhile

  if(Count == 5)
    if(bForce)
      DestroyItem(Item);
    else
      Item.movable := oldMovable;
      LogError("[HouseDestroy] Nie udalo sie przeniesc przedmiotu do Escrow '"+GetName(EscrData.curBox)+"' item: "+hex(Item.serial)+"/"+hex(Item.objtype)+"/"+GetName(Item) + " do " + hex(curBackpack.serial)+ " MoveItem: " + MoveItemRes.errortext );
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DestroyAllHouseItems(House, Components)

  var Items   := House.items;

  foreach item in Items
    if(!(Item in Components))
      DestroyItem(item);
    endif
  endforeach

  var Dims := GetMultiDimensions(house.graphic);

  // solidny check wszystkiego wokol dla pewnosci

  foreach Item in ListObjectsInBox(house.x + Dims.xmin - 20, house.y + Dims.ymin - 20, -127,house.x + Dims.xmax + 20, house.y + Dims.ymax + 20, 127,house.realm)
    if(Item.isa(POLCLASS_ITEM) and !(Item in Components))
      if(GetHouseSerial(Item) == House.serial)
        DestroyItem(Item);
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function IsRealHouseItem(Item, House)

  if(Item.isa(POLCLASS_ITEM) and item.objtype != UOBJ_SRUNE and item.objtype != UOBJ_SCHEST and !item.movable)
    
    var isHouse := GetHouseSerial(Item);

    if(isHouse)
      return (isHouse == House.serial);
    else
      return (Item.multi.serial == House.serial);
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function MoveItemsToEscrow(byref EscrData, House, Components, bForce)

  return 0;

  var Dims    := GetMultiDimensions(house.graphic);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Items   := array;

  foreach Item in (House.items)
    if(IsRealHouseItem(Item, House))
      if(!(Item in Components))
        Items.append(Item);
      endif
    endif
  endforeach

  // dodatkowy list po wszystkim wokol - poniewaz house.items zwraca tylko itemy na multi;
  // przedmioty, ktore pozwalamy stawiac wokol domu tutaj nie beda sie znajdowac - dlatego dodatkowy selekcior

  foreach Item in ListObjectsInBox(house.x + Dims.xmin - 20, house.y + Dims.ymin - 20, -127,house.x + Dims.xmax + 20, house.y + Dims.ymax + 20, 127,house.realm)
    if(IsRealHouseItem(Item, House))
      if(!(Item in Components) and !(Item in Items))
        Items.append(Item);
      endif
    endif
  endforeach

  var leftItems := array;
  foreach Item in Items

    if(IsLockedDown(Item))
      EraseItemLockDown(item, house);

      if(!MoveItemToEscrow(Item, EscrData, bForce))
        SetItemLockDown(item, House);
        return 0;
      endif

    elseif(IsSecured(item))

      EraseSecured(item, house);

      if(IsHouseSecuredCont(item))

        if(EscrData)
          foreach subItem in ListRootItemsInContainer(item)
            if(!MoveItemToEscrow(subItem, EscrData, bForce))
              SetItemSecured(item, house);
              // ops, critcal failure, return 0!
              return 0;
            endif
          endforeach
        elseif(!bForce)
          return 0;
        endif

        // niszczymy secure skrzynke
        DestroyItem(item);

      else

        if(!MoveItemToEscrow(Item, EscrData, bForce))
          SetItemSecured(item, house);
          return 0;
        endif

      endif

    elseif(IsFenceSign(Item) or IsItemFromDeed(Item) or IsCarpetRollMain(Item))

      if(!MoveItemToEscrow(Item, EscrData, bForce))
        return 0;
      endif
  
    else

      leftItems.append(Item);

    endif

    EraseHouseItem(item);

  endforeach

  // zobaczmy, co nam zostalo - co sie nie przenioslo

  Items := array;
  
  foreach Item in leftItems
    if(Item)
      Items.append(Item);
    endif
  endforeach

  if(Items.size() > 0)
    LogNotice("[HouseDestroy]: Zostalo "+leftItems.size()+" przedmiotow po filtrowaniu! Ida do kasacji...", LOGF_SCRIPTS);
  endif

  foreach Item in Items

    if(Item)
      LogNotice("[HouseDestroy] Unidentified object: "+Item.serial+"/"+Item.objtype+"/"+GetName(Item)+" destroyed! DesOnMultiDes: "+ItemCfg[Item.objtype].DesOnMultiDes, LOGF_SCRIPTS);
      if (Item.objtype != UOBJ_CORPSE)
        DestroyItem(Item);
      endif
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetAllHouseComponents(House)

  var Components := GetHouseComponents(house);

  foreach Item in Components
    Item := FindItemBySerial(Item);
  endforeach

  Components += house.components;

  foreach item in (house.items)

    if(Item.isa(POLCLASS_DOOR) or Item.objtype == UOBJ_GUILD_STONE or (Item.objtype >= UOBJ_HSIGN_START and Item.objtype <= UOBJ_HSIGN_END))
      if(!(Item in Components))
        Components.append(Item);
      endif
    endif

  endforeach

  return Components;

endfunction

//-----------------------------------------------------------------------------

function DestroyHouse(house, bForce := 0)

  if(!DestroyMulti(House))
    LogError("[HouseDestroy] Nie udalo sie skasowac multi! House: "+house.serial);
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function KickFromHouse(who, sign)

  SendSysMessage(who,"Kogo chcesz wyrzucic ?",FONT_NORMAL,COLOR_GREEN);

  var Victim := ObjTarget(who);
  var House  := GetHouse(sign);

  if(!StandardMobCheck(who,Victim))
    return;
  endif

  NoLongerBusy(Victim);

  if(Victim == who)
    return;
  endif

  if(Victim.multi.serial != House.serial)
    SendSysMessage(who,"Ta osoba nie znajduje sie w twoim domu.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(GetStrength(Victim) > GetStrength(who) and RandomInt(10) > 5)
    SendSysMessage(who,"Jestes za slaby by moc wykopac ta osobe.",FONT_NORMAL,COLOR_RED);
  else
    if(MoveObject(Victim, sign.x, sign.y, GetStandZ(sign.x, sign.y, sign.realm, sign.z), sign.realm))
      SendSysmessage(who,"Wykopal"+ggm(who,3)+" osobe "+victim.name+".",FONT_NORMAL,COLOR_GREEN);
      SendSysMessage(Victim,"Zostal"+ggm(Victim,3)+" wyrzucon"+ggm(Victim,6)+" przez "+who.name+".",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who,"Nie udalo Ci sie wykopac tej osoby.",FONT_NORMAL,COLOR_RED);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function HaveBarrel(house)

  return FindItemBySerial(GetObjProperty(house,"barrelserial"));

endfunction

//-----------------------------------------------------------------------------

function SetHouseBarrel(house,barrel)

  SetObjProperty(house,"barrelserial",barrel.serial);

endfunction

//-----------------------------------------------------------------------------

function EraseHouseBarrel(house)

  var Barrel := HaveBarrel(house);
  if(Barrel)
    DestroyItem(Barrel);
    EraseObjProperty(house,"barrelserial");
  endif

endfunction

//-----------------------------------------------------------------------------

function IsHouseSecuredCont(Cont)

  return CInt(GetObjProperty(Cont,"HouseSecure"));

endfunction

//-----------------------------------------------------------------------------

function SetHouseSecure(SecureCont,house)

  SetObjProperty(SecureCont,"HouseSecure",house.serial);

endfunction

//-----------------------------------------------------------------------------

function EraseHouseSecure(SecureCont,house)

  EraseObjProperty(SecureCont,"HouseSecure");

endfunction

//-----------------------------------------------------------------------------

function SetPublicHouse(House)

  if(IsPublicHouse(House))
    return 0;
  endif

  SetObjProperty(House,"Public",1);
  var Doors := FindHouseDoors(House);
  foreach Door in Doors
    SetObjProperty(Door,"Public",1);
    Door.locked := 0;
  endforeach
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetPrivateHouse(House)

  if(!IsPublicHouse(House))
    return 0;
  endif
  EraseObjProperty(House,"Public");
  var Doors := FindHouseDoors(House);
  foreach Door in Doors
    EraseObjProperty(Door,"Public");
    Door.locked := 1;
  endforeach
  return 1;

endfunction

//-----------------------------------------------------------------------------

function FindHouseSign(House)

  return FindItemBySerial(GetObjProperty(House,"signserial"));

endfunction

//-----------------------------------------------------------------------------

function GetHouseVendors(House)

  var Vendors := GetObjProperty(house,"PlayerVendors");
  if(!Vendors or !Vendors.size())
    Vendors := {};
  endif

  var c := 1;
  var Npc;

  foreach Vendor in GetObjProperty(House,"PlayerVendors")
    Npc := FindNpcBySerial(Vendor);
    if(!Npc or CInt(GetObjProperty(Npc, "MultiOwnerSerial")) != House.serial)
      Vendors.erase(c);
    else
      c := c + 1;
    endif
  endforeach

  SetObjProperty(House,"PlayerVendors",Vendors);
  return Vendors;

endfunction

//-----------------------------------------------------------------------------

function AddHouseVendor(Vendor, House)

  var Vendors := GetHouseVendors(House);
  if(Vendor.serial in Vendors)
    return 1;
  endif
  if(len(Vendors) >= MAX_HOUSE_VENDORS)
    return 0;
  endif
  if(Vendor.multi.serial != House.serial)
    return 0;
  endif
  Vendors.append(Vendor.serial);
  SetObjProperty(House,"PlayerVendors",Vendors);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckHouseVendor(Vendor, House)

  if(!Vendor or !House)
    return;
  endif

  var Vendors := GetHouseVendors(House);

  if(!(Vendor.serial in Vendors))
    Vendors.append(Vendor.serial);
    SetObjProperty(House,"PlayerVendors",Vendors);
  endif

endfunction

//-----------------------------------------------------------------------------

function EraseHouseVendors(House,VendorSerial)

  if(!House.isa(POLCLASS_MULTI) or !VendorSerial)
    return 0;
  endif

  var Vendors := GetObjProperty(house,"PlayerVendors");
  var c := 1;

  foreach Vendor in Vendors
    if(Vendor == VendorSerial)
      Vendors.erase(c);
      break;
    endif
    c := c + 1;
  endforeach

  SetObjProperty(House,"PlayerVendors",Vendors);
  return Vendors;

endfunction

//-----------------------------------------------------------------------------

function CanPlaceInHouseAtLoc(House,Loc)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  foreach Item in ListItemsNearLocation(Loc.x,Loc.y,Loc.z,1,House.realm)
    if(item.isa(POLCLASS_DOOR) and Abs(Item.z - Loc.z) < 10)
      if(Loc.x == Item.x and Loc.y == Item.y)
        return 0;
      endif

      if(ItemCfg[Item.objtype].Flow == error)
        return 0;
      endif

      case(CInt(ItemCfg[Item.objtype].Flow))
        DIR_W:
        DIR_E:   if((Item.x + 1 == Loc.x or Item.x - 1 == Loc.x or Item.x == Loc.x) and (Item.y == Loc.y))
                   return 0;
                 endif
        DIR_S:
        DIR_N:   if((Item.y + 1 == Loc.y or Item.y - 1 == Loc.y or Item.y == Loc.y) and (Item.x == Loc.x))
                   return 0;
                 endif

        default: return 0;
      endcase
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsWall(x,y,z,Rlm)

  if(!GetStandingHeight(x,y,z,Rlm))
    if(ListItemsAtLocation(x,y,z,Rlm).size() == 0)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseOwnerList(who)

  var List := GetObjProperty(who, "HouseSers");
  var House,i;

  if(!List)
    List := array;
  endif

  for(i:=1;i<=List.size();i:=i+1)
    House := FindItemBySerial(List[i]);
    if(!House)
      List.erase(i);
      i := i - 1;
    elseif(GetHouseOwnerSerial(House) != who.serial)
      List.erase(i);
      i := i - 1;
    endif
  endfor

  SetObjProperty(who, "HouseSers", List);
  return List;

endfunction

//-----------------------------------------------------------------------------

function AddHouseToOwnerList(House)

  var Owner := GetHouseOwner(house);

  if(Owner)
    var List := GetHouseOwnerList(Owner);
    if(List.size() > MAX_HOUSES_ON_PLAYER and !Owner.cmdlevel)
      LogNotice("AddHouseToOwnerList: Osoba "+GetRealName(Owner)+" posiada "+List.size()+" H");
    endif
    SetObjProperty(House, "owneracc", Owner.acctname);
    if(!(House.serial in List))
      List.append(House.serial);
      SetObjProperty(Owner, "HouseSers", List);
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseSignObjType(sGrap)

  case(sGrap)
    0x0BD1: return 0x600A;
    0x0BD2: return 0x600B;
    0x0BCF: return 0x600C;
    0x0BD0: return 0x600D;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ReCreateHouseSign(Sign)

  if(!Sign)
    return 0;
  endif

  var ObjType := GetHouseSignObjType(Sign.objtype);
  if(!ObjType)
    LogError("ReCreateHouseSign: brak odpowiedniego ObjType dla znaku ["+Sign.objtype+"] ["+Sign.x+","+Sign.y+","+Sign.z+" "+Sign.realm+"] Tworzenie 0x600A.");
    ObjType := 0x600A;
  endif

  var nSign   := CreateItemAtLoc(Sign.x, Sign.y, Sign.z, ObjType, 1, Sign.realm);
  DestroyItem(Sign);

  return nSign;

endfunction

function CreateCustomHouseSign(x, y, z, realm)

  var ObjType := 0x600A;
  var nSign   := CreateItemAtLoc(x, y, z, ObjType, 1, realm);
  return nSign;

endfunction

//-----------------------------------------------------------------------------

function IsHouseAccepted(House)

  if(House)
	return 1;
    var Res := GetObjProperty(House, "TerrainAccepted");
    if(Res == 1)
      return 1;
    elseif(Res and Res > ReadGameClock())
      return -1;
    elseif(Res)
      EraseObjProperty(House, "TerrainAccepted");
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanBuildHouseAt(DeedObjType, Loc, Realm, House := 0)

  return 1;

  if(IsHouseAccepted(House))
    return 1;
  endif

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var housetype := GetHouseObjtype( DeedObjType );
  var Graphic   := ItemCfg[housetype].Graphic;
  var Dims      := GetMultiDimensions(Graphic);
  var nLoc      := struct;
  var Res;

  nLoc.+x := Loc.x;
  nLoc.+y := Loc.y;
  nLoc.+z := Loc.z;

  Res := CanBuildHouseAtLoc(DeedObjType, nLoc, Realm, House);
  if(!Res)
    return Res;
  endif

  nLoc.x := Loc.x + Dims.xmin;
  nLoc.y := Loc.y + Dims.ymin;
  Res := CanBuildHouseAtLoc(DeedObjType, nLoc, Realm, House);
  if(!Res)
    return Res;
  endif

  nLoc.x := Loc.x + Dims.xmax;
  nLoc.y := Loc.y + Dims.ymax;
  Res := CanBuildHouseAtLoc(DeedObjType, nLoc, Realm, House);
  if(!Res)
    return Res;
  endif

  nLoc.x := Loc.x + Dims.xmin;
  nLoc.y := Loc.y + Dims.ymax;
  Res := CanBuildHouseAtLoc(DeedObjType, nLoc, Realm, House);
  if(!Res)
    return Res;
  endif

  nLoc.x := Loc.x + Dims.xmax;
  nLoc.y := Loc.y + Dims.ymin;
  Res := CanBuildHouseAtLoc(DeedObjType, nLoc, Realm, House);
  if(!Res)
    return Res;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanBuildHouseAtTerrain(DeedObjType, byref Loc, Realm)

  var DeedCfg  := ReadConfigFile(":housing:itemdesc");
  var Terrain  := DeedCfg[DeedObjType].Terrain;
  var landtype := GetMapInfo(Loc.x, Loc.y, Realm).landtile;
  var Err      := error;

  if(!Terrain)
    LogError("CanBuildHouseAtTerrain: Dom ObjType ["+DeedObjType+"] nie posiada terenu.");
    return 1;
  endif

  Terrain := SplitWords(Terrain);

  if(IsSnowLandTile(LandType))
    if(!("snow" in Terrain))
      Err.errortext := "Nie uda Ci sie wybudowac tego domu na sniegu.";
      return Err;
    else
      return 1;
    endif
  elseif(IsSandLandTile(LandType))
    if(!("sand" in Terrain))
      Err.errortext := "Nie uda Ci sie wybudowac tego domu na piachu.";
      return Err;
    else
      return 1;
    endif
  elseif(!("green" in Terrain))
    Err.errortext := "Nie uda Ci sie wybudowac tego domu na zieleni.";
    return Err;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanBuildHouseAtLoc(DeedObjType, byref Loc, Realm, House := 0)

  if(IsHouseAccepted(House))
    return 1;
  endif

  var landtype := GetMapInfo(Loc.x, Loc.y, Realm).landtile;
  var Err      := error;

  if((landtype >= 0x9 and landtype <= 0x15) or (landtype >= 0x150 and landtype <= 0x15c))
    Err.errortext := "Nie mozesz wybudowac domu na farmie.";
    return Err;
  endif

  if((landtype >= 0x71 and landtype <= 0x78) or (landtype >= 0x27e and landtype <= 0x281) or (landtype>=0x377 and landtype<=0x37a) or (landtype>=0x553 and landtype <= 0x556))
    Err.errortext := "Nie mozesz wybudowac domu na drodze.";
    return Err;
  endif

  if(FindCityCoords(Loc.x, Loc.y, Realm))
    Err.errortext := "Nie mozesz wybudowac domu na terenie miasta!";
    return Err;
  endif

  if(IsInNoHouseArea(Loc.x,Loc.y,realm))
    Err.errortext := "Nie mozesz wybudowac domu w tej okolicy.";
    return Err;
  endif

  foreach Multi In ListMultisInBox(Loc.x-5, Loc.y-5, -127, Loc.x+5, Loc.y+5, 127, Realm)
    if(Multi.isa(POLCLASS_HOUSE) and Multi != House)
      Err.errortext := "Nie mozesz wybudowac domu tak blisko innego domu.";
      return Err;
    endif
  endforeach

  /*
  foreach Item In ListObjectsInBox(Loc.x-20, Loc.y-20, -127, Loc.x+20, Loc.y+20, 127, Realm)
    if(Item.isa(POLCLASS_ITEM))
      if(Item.objtype >= UOBJ_HRSIGN_START and Item.objtype <= UOBJ_HRSIGN_END)
        Err.errortext := "Nie mozesz wybudowac domu tak blisko budynku do wynajmu.";
        return Err;
      elseif(Item.isa(POLCLASS_DOOR) and !GetHouseSerial(Item))
        Err.errortext := "Nie mozesz wybudowac domu tak blisko budynkow publicznych.";
        return Err;
      endif
    endif
    sleepms(2);
  endforeach
  */

  return CanBuildHouseAtTerrain(DeedObjType, Loc, Realm);

endfunction

//-----------------------------------------------------------------------------

function IsSandLandTile(Tile)

  if(Tile >= 0x16 and Tile <= 0x4B)
    return 1;
  elseif(Tile >= 0x12A and Tile <= 0x12D)
    return 1;
  elseif(Tile >= 0x1B9 and Tile <= 0x1D1)
    return 1;
  elseif(Tile >= 0x28A and Tile <= 0x291)
    return 1;
  elseif(Tile >= 0x34D and Tile <= 0x254)
    return 1;
  elseif(Tile >= 0x359 and Tile <= 0x35C)
    return 1;
  elseif(Tile >= 0x3B7 and Tile <= 0x3CA)
    return 1;
  elseif(Tile >= 0x442 and Tile <= 0x455)
    return 1;
  elseif(Tile >= 0x501 and Tile <= 0x510)
    return 1;
  elseif(Tile >= 0x66F and Tile <= 0x672)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsSnowLandTile(Tile)

  if(Tile >= 0x11A and Tile <= 0x11D)
    return 1;
  elseif(Tile >= 0x3A9 and Tile <= 0x3AC)
    return 1;
  elseif(Tile >= 0x5DF and Tile <= 0x5E2)
    return 1;
  elseif(Tile >= 0x75D and Tile <= 0x773)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouses(byref Houses)

  var sHouses := dictionary;
  var DeedCfg := ReadConfigFile(":housing:itemdesc");

  foreach DeedObjType in GetConfigIntKeys(DeedCfg)
    if(DeedCfg[DeedObjType].HouseObjType != error)
      var House           := struct;
      House.+Name         := DeedCfg[DeedObjType].name;
      House.+Desc         := DeedCfg[DeedObjType].desc;
      House.+DeedObjType  := DeedObjType;
      House.+Terrain      := DeedCfg[DeedObjType].Terrain;
      House.+NumLockDowns := DeedCfg[DeedObjType].NumLockDowns;
      House.+NumSecure    := DeedCfg[DeedObjType].NumSecure;
      sHouses[ZeroFill(DeedCfg[DeedObjType].VendorSellsFor, 8)+"_"+House.DeedObjType] := House;
    endif
  endforeach

  sleepms(2);
  Houses := array;

  foreach House in sHouses
    Houses.append(House);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function FindItemsInHouseArea(House, ObjType)

  if(!House.isa(POLCLASS_MULTI))
    return 0;
  endif

  var Dims  := GetMultiDimensions(house.graphic);
  var Items := array;

  foreach Item in ListObjectsInBox(house.x + Dims.xmin - 20, house.y + Dims.ymin - 20, -127,house.x + Dims.xmax + 20, house.y + Dims.ymax + 20, 127,house.realm)
    if(Item.isa(POLCLASS_ITEM) and Item.objtype == ObjType)
      if(GetHouseSerial(Item) == House.serial)
        Items.append(Item);
      elseif(Item.multi.serial == House.serial)
        Items.append(Item);
      endif
    endif
    Sleepms(1);
  endforeach

  return Items;

endfunction

//-----------------------------------------------------------------------------

function SetNewOwnerOfHouse(House)

  var nOwner   := 0;
  var CoOwners := GetHouseCoOwners(House);

  var accname  := GetObjProperty(House, "owneracc");
  if(accname)
    var acc := FindAccount(accname);
    if(acc)    
      for i:= 1 to 5
        nOwner := acc.getcharacter(i);
        if(nOwner)
          if(GetHouseOwnerList(nOwner).size() < MAX_HOUSES_ON_PLAYER)
            SetHouseOwner(House,nOwner);
            CoOwners.erase(RetIndex(CoOwners, nOwner.serial));
            SetHouseCoOwners(House,CoOwners);
            return nOwner;
          endif
        endif
      endfor
    endif
  endif

  foreach cSerial in CoOwners
    nOwner := FindPlayerBySerial(cSerial);
    if(nOwner)
      if(GetHouseOwnerList(nOwner).size() < MAX_HOUSES_ON_PLAYER)
        SetHouseOwner(House,nOwner);
        CoOwners.erase(RetIndex(CoOwners, cSerial));
        SetHouseCoOwners(House,CoOwners);
        return nOwner;
      endif
    endif
  endforeach

  return nOwner;

endfunction

//-----------------------------------------------------------------------------

function SetPublic( house,who:= 0)

  if(SetPublicHouse(house))
    SendSysMessage(who,"Zadeklarowal"+ggm(who,3)+" ten dom publicznym, kupcy moga teraz sprzedawac.",FONT_NORMAL,COLOR_GREEN);
    SetObjProperty(house, "PublicDelay", ReadGameClock() + PUBLIC_HOUSE_DELAY);
  else
    SendSysMessage(who,"Ten dom jest zadeklarowany publicznym.",FONT_NORMAL,COLOR_RED);
  endif

endfunction