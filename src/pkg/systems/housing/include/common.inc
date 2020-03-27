//-----------------------------------------------------------------------------

include "include/finding";

//-----------------------------------------------------------------------------

function CheckIsHouseItem(Item)

  var iHouse := Item.multi;

  if(!iHouse)
    iHouse := FindItemBySerial(GetHouseSerial(item));
  endif

  return iHouse;

endfunction

//-----------------------------------------------------------------------------

function GetHouseSerial(item)

  if(GetObjProperty(item,"houseserial"))
    SetObjProperty(item,"house_serial",Cint(GetObjProperty(item,"houseserial")));
    EraseObjProperty(item,"houseserial");
  endif

  return Cint(GetObjProperty(item,"house_serial"));

endfunction

//-----------------------------------------------------------------------------

function SetHouseItem(item,house)

  SetObjProperty(item,"house_serial",Cint(house.serial));

endfunction

//-----------------------------------------------------------------------------


function EraseHouseItem(item)

  EraseObjProperty(item,"house_serial");

endfunction

//-----------------------------------------------------------------------------

function IsSecured(item)

  return CInt(GetObjProperty(item,"secure"));

endfunction

//-----------------------------------------------------------------------------

function IsLockedDown(item)

  return Cint(GetObjProperty(item,"lockeddown"));

endfunction

//-----------------------------------------------------------------------------

function GetHouse(item)

  return FindItemBySerial(GetHouseSerial(item));

endfunction

//-----------------------------------------------------------------------------

function GetHouseOwner(house)

  return FindPlayerBySerial(GetObjProperty(house,"ownerserial"));

endfunction

//-----------------------------------------------------------------------------

function GetHouseOwnerSerial(house)

  return Cint(GetObjProperty(house,"ownerserial"));

endfunction

//-----------------------------------------------------------------------------

function IsHouseOwner(who, house)
  
 if(GetHouseOwner(house).acctname == who.acctname)
   return 1;
 endif

 return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseOwnerBank(house)

  var Owner := FindHouseKeeper(house);

  if(Owner)
    return FindBankBox(Owner);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseOwnerEscrow(house)

  var Owner := FindHouseKeeper(house);

  if(Owner)
    return GetEscrowData(Owner);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsHouseCoOwner(who, house)

  if(who.serial in GetHouseCoOwners(house))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsHouseFriend(who, house)

  if(who.serial in GetHouseFriends(house))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHouseCoOwners(house)

  var CoOwners := GetObjProperty(house,"CoOwners");
  if(!CoOwners)
    CoOwners := {};
  endif

  return CoOwners;


endfunction

//-----------------------------------------------------------------------------

function GetHouseFriends(house)

  var Friends := GetObjProperty(house,"Friends");
  if(!Friends)
    Friends := {};
  endif

  return Friends;

endfunction

//-----------------------------------------------------------------------------

function GetDeedObjTypeByHouseType(housetype)

  var id   := ReadConfigFile( ":housing:itemdesc" );
  var elem := id[ CInt(housetype) ];

  if(elem.Name)
    return GetObjtypeByName( elem.Name + "deed" );
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckPlaceForMulti(Place,Rlm)

  if(!Place)
    return 0;
  endif

  return (GetStandingHeight(Place.x,Place.y,Place.z,Rlm).multi);

endfunction

//-----------------------------------------------------------------------------

function IsPublicHouse(House)

  return CInt(GetObjProperty(House,"Public"));

endfunction

//-----------------------------------------------------------------------------

function IsOutsideHouse(House, x, y, z)

  if(House.isa(POLCLASS_MULTI))
    var m := GetStand(x, y, House.realm, z+20);

    if(m.multi.serial != House.serial)
      return 1;
    elseif(m.z == error)
      return 0;
    elseif(abs(m.z - z) >= 20)
      return 0;
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsInVampireHouse(who)

  if(who.multi)
    if(!who.multi.isa(POLCLASS_BOAT))
      var ItemCfg := ReadConfigFile(":housing:itemdesc");
      return CInt(ItemCfg[who.multi.objtype].VampireHouse);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ResetHousePayment(House)

  EraseObjProperty(House, "PaymentAmount");

endfunction

//-----------------------------------------------------------------------------

function HaveHouseGuild(House)

  foreach Item in (house.items)

    if(Item.objtype == UOBJ_GUILD_STONE)
      return 1;
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanControlHouse(Item, who, bFriendCheck := 0)

  var House;

  if(Item.isa(POLCLASS_MULTI))
    House := Item;
  else
    House := GetHouse(Item);
  endif

  if(!House)
    return error;
  endif

  if(IsHouseOwner(who, House) or IsHouseCoOwner(who, House))
    return 1;
  elseif(bFriendCheck and IsHouseFriend(who, House))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanAccessSecureCont(who, Item)

  var house := GetHouse( item );
  var AccessFriend := GetObjProperty(item, "AccessFriendSecureCont");

  if(House and IsHousePaymentLocked(House))
    SendSysMessage(who,"Skrzynia zostala zamknieta do czasu oplacenia podatku!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(IsHouseOwner(who,house) or IsHouseCoOwner(who,house) or IsAdmin(who) or (IsHouseFriend(who,house) and AccessFriend)) 
    return 1;
  endif
  
  return 0;
  
endfunction 
  
//-----------------------------------------------------------------------------

