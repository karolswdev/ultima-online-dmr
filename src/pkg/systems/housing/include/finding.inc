//-----------------------------------------------------------------------------

include "include/finding";
include ":housing:common";

//-----------------------------------------------------------------------------

const GNHOF_OWNER            := 0x01;
const GNHOF_COOWNER          := 0x02;
const GNHOF_FRIEND           := 0x04;
const GNHOF_ANY              := 0x08;
const GNHOF_NOGM             := 0x10;

//-----------------------------------------------------------------------------

function FindHousesForPlayer(who, Flags := 0)

  var Houses := array;

  if(!Flags)
    return Houses;
  endif

  var Rlms := Realms();
  var x,y, tox, toy;

  foreach Realm in Rlms

    for(y:=0; y<Realm.height; y:=y+256)

      if(y+255 > Realm.height)
        toy := Realm.height;
      else
        toy := y + 255;
      endif

      for(x:=0;x<Realm.width;x:=x+256)
        if(x+255 > Realm.width)
          tox := Realm.width;
        else
          tox := x + 255;
        endif

        foreach Multi in ListMultisInBox(x, y, -127, tox, toy, 127, _Realm_iter)
          if(HouseMeetsConditions(Multi, who, Flags))
            Houses.append(Multi);
          endif
        endforeach

      endfor

    endfor

  endforeach

  return Houses;

endfunction

//-----------------------------------------------------------------------------

function HouseMeetsConditions(House, who, Flags)

  if(Flags & GNHOF_OWNER)
    if(IsHouseOwner(who, House))
      return 1;
    endif
  endif

  if(Flags & GNHOF_COOWNER)
    if(IsHouseCoOwner(who, House))
      return 1;
    endif
  endif

  if(Flags & GNHOF_FRIEND)
    if(IsHouseFriend(who, House))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetNearHouses(x, y, z, Realm, Range := 10)

  x     := CInt(x);
  y     := CInt(y);
  Range := CInt(Range);

  if(!x or !y or z == error or !Realm or Range <= 0)
    return array;
  endif

  z     := CInt(z);

  var Stand := GetStand(x, y, Realm, z);

  if(Stand.multi)
    return array( Stand.multi );
  else

    var Houses := dictionary;

    foreach Multi in ListMultisInBox(x-Range, y-Range, -127, x+Range, y+Range,127, Realm)
      Houses[ZeroFill(coordist(Multi.x, Multi.y, x, y), 3)+"_"+Multi.serial] := Multi;
    endforeach

    var aHouses := array;

    foreach House in Houses
      aHouses.append(House);
    endforeach

    return aHouses;

  endif

endfunction

//-----------------------------------------------------------------------------

function GetNearestHouseOf(x, y, z, Realm, Range, Flags, who := 0)

  Flags := CInt(Flags);

  if(!Flags)
    return 0;
  endif

  if(!IsPlayer(who))
    who := 0;
  endif

  if(!who and !(Flags & GNHOF_ANY))
    var Err := error;
    Err.errortext := "Nie widac by byl w poblizu jakis dom.";
    return Err;
  endif

  var Houses := GetNearHouses(x, y, z, Realm, Range);

  if(!Houses.size())
    var Err := error;
    Err.errortext := "Nie widac by byl w poblizu jakis dom.";
    return Err;
  endif

  foreach House in Houses

    if(Flags & GNHOF_ANY)
      return House;
    endif

    if(!(Flags & GNHOF_NOGM))
      if(IsGm(who))
        return House;
      endif
    endif

    if(Flags & GNHOF_OWNER)
      if(IsHouseOwner(who, House))
        return House;
      endif
    endif

    if(Flags & GNHOF_COOWNER)
      if(IsHouseCoOwner(who, House))
        return House;
      endif
    endif

    if(Flags & GNHOF_FRIEND)
      if(IsHouseFriend(who, House))
        return House;
      endif
    endif

  endforeach

  var Err  := error;
  var Msgs := "";

  if(Flags & GNHOF_OWNER)
    Msgs := "wlascicielem";
  endif

  if(Flags & GNHOF_COOWNER)
    if(Msgs)
      Msgs := Msgs + " ani co-wlascicielem";
    else
      Msgs := "co-wlascicielem";
    endif
  endif

  if(Flags & GNHOF_FRIEND)
    if(Msgs)
      Msgs := Msgs + " ani przyjacielem";
    else
      Msgs := "przyjacielem";
    endif
  endif

  Err.errortext := "Nie jestes "+Msgs;

  if(who.multi)
    Err.errortext := Err.errortext + " tego domu.";
  else
    Err.errortext := Err.errortext + " pobliskiego domu.";
  endif

  return Err;

endfunction

//-----------------------------------------------------------------------------

function FindHouseKeeper(house)

  var Owner := GetHouseOwner(house);

  if(Owner)
    return Owner;
  endif

  var CoOwners := GetHouseCoOwners(House);  
  var Friends  := GetHouseFriends(House);
  var Person;

  foreach cSerial in CoOwners
    Person := FindPlayerBySerial(cSerial);
    if(Person)
      return Person;
    endif
  endforeach

  foreach cSerial in Friends
    Person := FindPlayerBySerial(cSerial);
    if(Person)
      return Person;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
