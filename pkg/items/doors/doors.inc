//-----------------------------------------------------------------------------

include "include/key";
include "include/dirs";
include "include/client/sounds";
include "include/move";

include ":doors:common";

//-----------------------------------------------------------------------------

const DOOR_OPEN_LOCKED       := 0;

const DOOR_OPEN_UNLOCKED     := 1;
const DOOR_OPEN_ADMIN        := 2;
const DOOR_OPEN_INSIDE       := 3;
const DOOR_OPEN_KEY          := 4;

//-----------------------------------------------------------------------------

function CanOpenDoor(who, Door)

  if(IsOpened(Door))
    return error;
  endif

  if(!IsLocked(Door))
    return DOOR_OPEN_UNLOCKED;
  endif

  // w przeciwnym wypadku sprawdzmy przywileje danej osoby
  
  if(!who.isa(POLCLASS_NPC))
    if(IsAdmin(who))
      return DOOR_OPEN_ADMIN;
    endif
  endif

  // moze drzwi otworzy od wewnatrz? sprawdzmy kierunek
  var LockId  := GetItemLockId(Door);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[door.objtype].Flow != error and !GetObjProperty(door,"NoFlow"))
    if(IsFacingObject(who, door, CInt(ItemCfg[door.objtype].Flow)))
      return DOOR_OPEN_INSIDE;
    endif
  endif
  
  // moze ma klucz ?
  var Keys;

  foreach Item in EnumerateItemsInContainer(who.backpack)
  
    if(Item.objtype == UOBJ_KEYRING)
    
      Keys := GetObjProperty(Item,"KeysInside");
      foreach KeyProp in Keys
        if(KeyProp[2] == LockId)
          return DOOR_OPEN_KEY;
        endif
      endforeach
      
    elseif(Item.objtype >= UOBJ_KEYSTART and Item.objtype <= UOBJ_KEYEND)
    
      if(GetItemLockId(Item) == LockId)
        return DOOR_OPEN_KEY;
      endif
      
    endif
    
  endforeach

  return DOOR_OPEN_LOCKED;
  
endfunction

//-----------------------------------------------------------------------------

function OpenDoor(Door1)

  if(IsOpened(Door1))
    return;
  endif

  var DoorDesc := ReadConfigFile(":doors:itemdesc");
  
  var D1x  := Door1.x;
  var D1y  := Door1.y;
  var N1x  := D1x + CInt(DoorDesc[Door1.objtype].xmod);
  var N1y  := D1y + CInt(DoorDesc[Door1.objtype].ymod);

  var Door2,D2x,D2y,N2x,N2y;
  
  foreach Item in ListItemsNearLocation(Door1.x,Door1.y,Door1.z,1,Door1.realm)
    if(Item.isa(POLCLASS_DOOR))
      if(Item != Door1)
        if(!IsOpened(Item))
          Door2 := Item;
          break;
        endif
      endif
    endif
  endforeach

  if(Door2)
    D2x := Door2.x;
    D2y := Door2.y;
    N2x := D2x + CInt(DoorDesc[Door2.objtype].xmod);
    N2y := D2y + CInt(DoorDesc[Door2.objtype].ymod);
    SetObjProperty(Door1,"LinkOpenDoor",Door2.serial);
  endif

  var closesound, opensound;

  case (DoorDesc[Door1.objtype].doortype)
    "wood":  opensound   := SFX_OPEN_WOODEN_DOOR;
             closesound  := SFX_CLOSE_WOODEN_DOOR;
    "stone": 
    
    "metal": opensound   := SFX_OPEN_METAL_DOOR;
             closesound  := SFX_CLOSE_METAL_DOOR;

    "ice":   opensound   := SFX_OPEN_ICE_DOOR;
             closesound  := SFX_CLOSE_ICE_DOOR;
  endcase

  if(!OpenThisDoor(Door1,N1x,N1y,opensound))
    return;
  endif
  
  if(Door2)
    if(!OpenThisDoor(Door2,N2x,N2y,opensound))
      Door2 := 0;
    endif
  endif

  var Door1F := 0;
  var Door2F := 0;
  
  if(!Door2)
    Door2F := 1;
  endif

  SetObjProperty(Door1, "#Opened", GetPid());
  SetObjProperty(Door2, "#Opened", GetPid());
  sleepms(500+RandomInt(500));
  Detach();

  if(Door1.multi or Door2.multi)
    sleep(4);
  else
    sleep(10);
  endif

  while(!Door1F or !Door2F)
    if(!Door1F)
      if(Door1 and IsOpened(Door1) and GetObjProperty(Door1, "#Opened") == GetPid())
        if(!len(ListMobilesNearLocationEx(D1x, D1y, Door1.z, 0, LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL, Door1.realm)))
          CloseThisDoor(Door1,D1x,D1y,closesound);
          Door1F := 1;
        endif
      else
        Door1F := 1;
      endif
    endif
    
    if(!Door2F)
      if(Door2 and IsOpened(Door2) and GetObjProperty(Door2, "#Opened") == GetPid())
        if(!len(ListMobilesNearLocationEx(D2x, D2y, Door2.z, 0, LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL, Door2.realm)))
          CloseThisDoor(Door2,D2x,D2y,closesound);
          Door2F := 1;
        endif
      else
        Door2F := 1;
      endif
    endif

    sleep(2);
  endwhile

  if(GetObjProperty(Door1, "#Opened") == GetPid())
    EraseObjProperty(Door1, "#Opened");
  endif

  if(Door2)
    if(GetObjProperty(Door2, "#Opened") == GetPid())
      EraseObjProperty(Door2, "#Opened");
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CloseDoor(Door1,Door2 := 0)

  if(!IsOpened(Door1))
    return;
  endif

  var DoorDesc := ReadConfigFile(":doors:itemdesc");

  var N1x  := Door1.x - CInt(DoorDesc[Door1.objtype].xmod);
  var N1y  := Door1.y - CInt(DoorDesc[Door1.objtype].ymod);

  var N2x,N2y;

  var closesound;

  case (DoorDesc[Door1.objtype].doortype)
    "wood":  closesound  := SFX_CLOSE_WOODEN_DOOR;
    "stone": 
    "metal": closesound  := SFX_CLOSE_METAL_DOOR;
    "ice":   closesound  := SFX_CLOSE_ICE_DOOR;
  endcase

  if(!Door2)
    if(GetObjProperty(Door1,"LinkOpenDoor"))
      foreach Item in ListItemsNearLocation(N1x,N1y,Door1.z,2,Door1.realm)
        if(Item.isa(POLCLASS_DOOR))
          if(Item.serial == GetObjProperty(Door1,"LinkOpenDoor"))
            if(IsOpened(Item))
              Door2 := Item;
              break;
            endif
          endif
        endif
      endforeach
    endif
  else
    Door2 := 0;
  endif

  if(Door2)
    N2x := Door2.x - CInt(DoorDesc[Door2.objtype].xmod);
    N2y := Door2.y - CInt(DoorDesc[Door2.objtype].ymod);
  endif

  var Door1F := 0;
  var Door2F := 0;

  if(!Door2)
    Door2F := 1;
  endif

  while(!Door1F or !Door2F)
    if(!Door1F)
      if(Door1 and IsOpened(Door1))
        if(!len(ListMobilesNearLocationEx(N1x, N1y, Door1.z, 0, LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL,Door1.realm)))
          if(CloseThisDoor(Door1,N1x,N1y,closesound))
            Door1F := 1;
          endif
        endif
      else
        Door1F := 1;
      endif
    endif

    if(!Door2F)
      if(Door2 and IsOpened(Door2))
        if(!len(ListMobilesNearLocationEx(N2x, N2y, Door2.z, 0, LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL,Door2.realm)))
          if(CloseThisDoor(Door2,N2x,N2y,closesound))
            Door2F := 1;
          endif
        endif
      else
        Door2F := 1;
      endif
    endif
    
    sleep( 2 );

  endwhile

endfunction

//-----------------------------------------------------------------------------

function OpenThisDoor(door,Nx,Ny,opensound)

  if(!IsOpened(door))
    if(MoveDoorToOpenOrClose(door,Nx,Ny))
      if(opensound)
        PlaySoundEffect( door, opensound );
      endif

      door.graphic := door.objtype + 1;
      SetObjProperty( door, "IsOpened", GetPid() );

      return 1;
    endif
  endif
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CloseThisDoor(door,Nx,Ny,closesound)

  if(IsOpened(door))
    if(MoveDoorToOpenOrClose(door,Nx,Ny))
      if(closesound)
        PlaySoundEffect( door, closesound );
      endif

      door.graphic := door.objtype;
      EraseObjProperty( door, "IsOpened" );
      return 1;
    endif
  endif
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function MoveDoorToOpenOrClose(door,Nx,Ny)

  if(!ReserveItem(Door))
    return 0;
  endif

  door.movable := 0;

  if(Nx != door.x or Ny != door.y)
    if(MoveObject(door, Nx, Ny, door.z, door.realm, MOVEITEM_FORCELOCATION | MOVEITEM_IGNOREMOVABLE))
      ReleaseItem(Door);
      return 1;
    endif
  else
    ReleaseItem(Door);
    return 1;
  endif

  ReleaseItem(Door);

  return 0;

endfunction

//-----------------------------------------------------------------------------
