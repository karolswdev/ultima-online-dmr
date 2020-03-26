//-----------------------------------------------------------------------------

include "include/findcity";
include "include/finding";

//-----------------------------------------------------------------------------

const RENTHOUSES_CFG                   := ":rhousecfg:";

//-----------------------------------------------------------------------------

function GetHr(City,Nr)

  var CityCfg := ReadConfigFile(RENTHOUSES_CFG+lower(City));
  if(CityCfg)
    return FindConfigElem(CityCfg, Nr);
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetHrCity(Sign)

  return GetObjProperty(Sign,"HrCity");
  
endfunction

//-----------------------------------------------------------------------------

function GetHrNr(Sign)

  return CInt(GetObjProperty(Sign,"HrNr"));

endfunction

//-----------------------------------------------------------------------------

function GetOwner(Sign)

  return FindPlayerBySerial(GetOwnerSerial(Sign));

endfunction

//-----------------------------------------------------------------------------

function GetOwnerSerial(Sign)

  return CInt(GetObjProperty(Sign,PROP_OWNER));

endfunction

//-----------------------------------------------------------------------------

function CheckNearHr(x,y,z,Rlm,Range := 15)

  var Hr;

  if(GetStand(x, y, Rlm, z).multi)
    return 0;
  endif

  foreach Item in ListItemsNearLocation(x,y,z,Range,Rlm)
    if(GetHrNr(Item))
      Hr := GetHr(GetHrCity(Item), GetHrNr(Item));
      if(IsInHr(x, y, z, Rlm, Hr))
        return Item;
      endif
    endif
  endforeach
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInHr(x,y,z,Rlm,House)

  __ParamUnused(z);

  if(Rlm != GetCityRealm(House.City))
    return 0;
  elseif(x > CInt(House.HouseXS) and y > CInt(House.HouseYS))
    if(x < CInt(House.HouseXE) and y < CInt(House.HouseYE))
      return 1;
    endif
  endif
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function CanControlHr(who,Sign)

  if(GetOwnerSerial(Sign) == who.serial)
    return 1;
  elseif(IsHrFriend(Sign,who))
    return 1;
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function AddItemToHr(Item,Sign)

  SetObjProperty(Item,"HrSign",Sign.serial);

  if(Item.isa(POLCLASS_CONTAINER))
    SetObjProperty(Sign, "HrConts", CInt(GetObjProperty(Sign, "HrConts"))+1);
  endif

endfunction

//-----------------------------------------------------------------------------

function IsHrItem(Item,Sign)

  if(GetHrSignFromItem(Item) == Sign)
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHrSignFromItem(Item)

  return FindItemBySerial(GetObjProperty(Item,"HrSign"));

endfunction

//-----------------------------------------------------------------------------
// Friends
//-----------------------------------------------------------------------------

function IsHrFriend(Sign,who)

  var Friends := GetHrFriends(Sign);
  
  if(who.serial in Friends)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHrFriends(Sign)

  var Friends := GetObjProperty(Sign,"HrFriends");
  var i       := 1;
  
  if(!Friends)
    Friends := {};
  endif

  foreach Friend in GetObjProperty(Sign,"HrFriends")
    Friend := FindPlayerBySerial(Friend);
    if(!Friend)
      Friends.erase(i);
    else
      i := i + 1;
    endif
  endforeach
  
  SetObjProperty(Sign, "HrFriends", Friends);
  
  return Friends;

endfunction

//-----------------------------------------------------------------------------

function HrHaveVendor(Sign)

  var Vendor := FindNpcBySerial(GetObjProperty(Sign,"HrVendor"));
  if(Vendor)
    if(GetObjProperty(Vendor,"MultiOwnerSerial") == Sign.serial)
      return Vendor;
    else
      EraseObjProperty(Sign,"HrVendor");
    endif
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanLockHrDoors(Door)

  var SignSer := GetObjProperty(Door, "HrPublic");

  if(SignSer != error)
    var Sign := FindItemBySerial(SignSer);
    if(Sign)
      if(HrHaveVendor(Sign))
        return 0;
      endif
    endif

    EraseObjProperty(Door,"HrPublic");
  endif

  return 1;
  
endfunction

//-----------------------------------------------------------------------------
