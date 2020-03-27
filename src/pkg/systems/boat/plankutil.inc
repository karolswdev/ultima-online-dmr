//-----------------------------------------------------------------------------

use uo;
use os;

//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/key";
include "include/logerror";

//-----------------------------------------------------------------------------

function IsExtended( plank )
  return plank.graphic in { GID_PLANK_EXTENDED_FACING_EAST, GID_PLANK_EXTENDED_FACING_WEST, GID_PLANK_EXTENDED_FACING_NORTH, GID_PLANK_EXTENDED_FACING_SOUTH };
endfunction                          

//-----------------------------------------------------------------------------

function IsPlank( object )
  return object.graphic in { GID_PLANK_EXTENDED_FACING_EAST, GID_PLANK_EXTENDED_FACING_WEST, GID_PLANK_EXTENDED_FACING_NORTH, GID_PLANK_EXTENDED_FACING_SOUTH, GID_PLANK_RETRACTED_FACING_EAST, GID_PLANK_RETRACTED_FACING_WEST, GID_PLANK_RETRACTED_FACING_NORTH, GID_PLANK_RETRACTED_FACING_SOUTH };
endfunction                          

//-----------------------------------------------------------------------------

function IsPlankOccupied( plank )

  if(plank)
    return ListMobilesNearLocation( plank.x, plank.y, plank.z, 0, plank.realm ).size();
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function Extend( plank )
  if (!IsExtended(plank))
    TogglePlank(plank);
  endif
endfunction

//-----------------------------------------------------------------------------

function Retract( plank )
  if (IsExtended(plank))
    TogglePlank(plank);
  endif
endfunction

//-----------------------------------------------------------------------------

function TogglePlank( plank )
  var partnertype := PlankPartner( plank.graphic );
  if (partnertype)
    plank.graphic := partnertype;
  endif
endfunction

//-----------------------------------------------------------------------------

function PlankPartner( planktype )
  case (planktype)
    GID_PLANK_EXTENDED_FACING_EAST:  return GID_PLANK_RETRACTED_FACING_EAST;   // 0x3ed5: return 0x3eb1;
    GID_PLANK_RETRACTED_FACING_EAST: return GID_PLANK_EXTENDED_FACING_EAST;    // 0x3eb1: return 0x3ed5;
    GID_PLANK_EXTENDED_FACING_WEST:  return GID_PLANK_RETRACTED_FACING_WEST;   // 0x3ed4: return 0x3eb2;
    GID_PLANK_RETRACTED_FACING_WEST: return GID_PLANK_EXTENDED_FACING_WEST;    // 0x3eb2: return 0x3ed4;
    GID_PLANK_EXTENDED_FACING_NORTH:  return GID_PLANK_RETRACTED_FACING_NORTH; // 0x3e84: return 0x3e85;
    GID_PLANK_RETRACTED_FACING_NORTH: return GID_PLANK_EXTENDED_FACING_NORTH;  // 0x3e85: return 0x3e84;
    GID_PLANK_EXTENDED_FACING_SOUTH:  return GID_PLANK_RETRACTED_FACING_SOUTH; // 0x3e89: return 0x3e8a;
    GID_PLANK_RETRACTED_FACING_SOUTH: return GID_PLANK_EXTENDED_FACING_SOUTH;  // 0x3e8a: return 0x3e89;
    default: LogError("PlankPartner: Bledny PlankType: "+ Hex(planktype) );
             return error;
  endcase
endfunction

//-----------------------------------------------------------------------------

function fk(me, plank)

  var DoorLockId := GetItemLockID(plank);
  var keylockid;
  var insidering;
  
  if(!DoorLockId)
    return 1;
  endif
  
  var Items := EnumerateItemsInContainer(me.backpack);
  
  foreach key in Items
    if(key.objtype >= UOBJ_KEYSTART and key.objtype <= UOBJ_KEYEND and key.objtype != UOBJ_KEYRING)
      if(GetItemLockID(key) == DoorLockId)
        return 1;
      endif
    elseif(key.objtype == UOBJ_KEYRING)
      if(IsKeyRingLockID(key,DoorLockId))
        return 1;
      endif
    endif
  endforeach
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------
