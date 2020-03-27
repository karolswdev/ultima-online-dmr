//--------------------------------------------------------------------------------

use uo;
use polsys;

//--------------------------------------------------------------------------------

include "include/const/props";
include "include/const/move";
include "include/const/misc";
include "include/objtypes/misc";
include "include/updweather";
include "include/checks/stdchecks";
include "include/npcs";
include "include/client/sounds";

include ":eventcore:movement";

//-----------------------------------------------------------------------------

function MoveObject(Obj, x, y, z, Realm := 0, Flags := 0, Type := MOVETYPE_NORMAL)

  if(!Obj.isa(POLCLASS_UOBJECT) or Obj.objtype == UOBJ_WRESTLE or z == error or !Obj)
    return RetError("Tego obiektu nie mozna przenosic");
  endif
  
  x := CInt(x);
  y := CInt(y);
  z := CInt(z);

  if(!Realm)
    Realm := Obj.realm;
  endif

  var Rlm := Realms()[Realm];

  if(!Rlm)
    return RetError("Nie przekazano realmu!");
  endif

  if(x < 0 || y < 0 || x >= Rlm.width || y >= Rlm.height)
    return RetError("Coordy wykraczaja poza granice realmu!");
  endif

  var lx     := Obj.x;
  var ly     := Obj.y;
  var lz     := Obj.z;
  var lRealm := Obj.realm;

  var oldProp;

  if(IsPlayer(Obj))
    oldProp := GetObjProperty(Obj, PROP_MOVELOC);
    SetObjProperty(Obj, PROP_MOVELOC, { Type, ReadGameClock(), { lx, ly, lz, lRealm }, { x, y, z, Realm } } );
  endif

  var Res := MoveObjectToLocation(Obj, x, y, z, Realm, flags);

  if(Res and IsPlayer(Obj))
    if(Obj.realm != lower(lRealm))
      SendLogonWPackets(Obj);
    endif
  elseif(IsPlayer(Obj) and oldProp)
    SetObjProperty(Obj, PROP_MOVELOC, oldProp );
  endif

  CheckAutoEventMovement(BuildLocStruct(lx, ly, lz, lRealm), Obj);

  return Res;
  
endfunction

//-----------------------------------------------------------------------------

function MoveToPack(item,who)

  var BackPack;

  if(who.isa(POLCLASS_NPC))
    BackPack := GetNpcBackPack(who);
  else
    BackPack := who.backpack;
  endif

  if(!item or !BackPack)
    return 0;
  endif
  var res := MoveItemToContainer(item,BackPack);
  if(!res)
    LogNotice("MoveToPack " + res.text + " | w: " + who.serial + " b: " + BackPack.serial + " i: " + item.serial + " " + GetName(item), LOGF_FORCE_SYSLOG | LOGF_SCRIPTS);
    return MoveObject(item, who.x, who.y, who.z, who.realm, MOVEITEM_FORCELOCATION);
  endif
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function MoveToCont(Item, Cont, Flags := 0)

  // a zeby przekazac error, res w zmienna!
  
  var Res := MoveItemToContainer(Item, Cont);

  if(Res)

    if(IsStackableObj(Item.objtype) and (Flags & MOVECONT_JOIN_STACKS))

      // sprawdzamy po move, bo onins/onrem mogly cos zmienic !

      foreach PackItem  in EnumerateItemsInContainer(Cont,ENUMERATE_ROOT_ONLY)
        
        if(IsStackable(PackItem, Item))
          if(PackItem == Item)
            continue;
          endif
          // no to jak moga, to trzeba incresnac amount i destroynac item!
          if(AddAmount(PackItem, Item.amount))
            if(!DestroyItem(Item))
              SubtractAmount(PackItem, Item.amount);
            else
              return res;
            endif
          endif
        endif
      endforeach
    endif

  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function WasJustMoving(who, Type := MOVETYPE_ALL, tDelay := MOVELOC_DELAY)

  if(IsPlayer(who))
    var MoveLoc := GetObjProperty(who, PROP_MOVELOC);
    if(MoveLoc[2] and MoveLoc[3] and MoveLoc[4])
      if(Type == -1 or Type == MoveLoc[1])
        if(MoveLoc[2] + tDelay > ReadGameClock())
          return 1;
        endif
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetLastMove(who)

  return GetObjProperty(who, PROP_MOVELOC);

endfunction

//-----------------------------------------------------------------------------

function GetLastMoveType(who)

  return GetObjProperty(who, PROP_MOVELOC)[1];

endfunction

//-----------------------------------------------------------------------------

function GetLastMoveTime(who)

  return GetObjProperty(who, PROP_MOVELOC)[2];

endfunction

//-----------------------------------------------------------------------------

function GetLastMoveLoc(who)

  return GetObjProperty(who, PROP_MOVELOC)[3];

endfunction

//-----------------------------------------------------------------------------

function GetNewMoveLoc(who)

  return GetObjProperty(who, PROP_MOVELOC)[4];

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Realms
//-----------------------------------------------------------------------------
//=============================================================================

function GetRealmById(Id)

  var NotId := 0;

  if(CInt(Id) == 0 and Id[1] != "0" and Id != 0)
    NotId := 1;
  endif
  
  foreach Realm in Realms()
    if((Realm.mapid == CInt(Id) and !NotId) or _Realm_iter == Id)
      return _Realm_iter;
    endif
  endforeach
  
  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetRealmsNames()

  var retData := dictionary;
  var Keys := Realms().keys();
  
  foreach Realm in Keys
    retData[Realm] := Realm;
  endforeach

  return retData;

endfunction

//--------------------------------------------------------------------------------

function GetRealmId(Realm)

  return Realms()[Realm].mapid;

endfunction

//-----------------------------------------------------------------------------
