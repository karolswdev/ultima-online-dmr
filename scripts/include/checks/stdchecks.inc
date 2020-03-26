//-----------------------------------------------------------------------------

use cfgfile;

//-----------------------------------------------------------------------------

include "include/client/cids";
include "include/const/cfgs";
include "include/const/props";

include ":character:checks";

//=============================================================================
//-----------------------------------------------------------------------------
// Summoned Check
//-----------------------------------------------------------------------------
//=============================================================================

function IsSummoned(critter)

  return GetObjProperty(critter,"summoned");

endfunction

//-----------------------------------------------------------------------------

function IsNpcSummoned(critter)

  return GetObjProperty(critter,"NpcSummoned");

endfunction

//--------------------------------------------------------------------------------

function IsPlayerVendor(Vendor)

  return (Vendor.script == "playermerchant");

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Tamed Check
//-----------------------------------------------------------------------------
//=============================================================================

function IsTamed(Npc)

  if(Npc.script == "tamed")
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsReallyTamed(Npc)
  
  if(Npc.script == "tamed")
    if(!GetObjProperty(Npc,"NecroControlled") and !IsSummoned(Npc))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function WasTamed(Npc)

  return GetObjProperty(Npc, PROP_UNTAMED);

endfunction

//-----------------------------------------------------------------------------

function IsTamedTMF(critter)

  if((critter.script == "tamed" or IsSummoned(critter)) and !GetObjProperty(critter,"NpcSummoned"))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsPlayerTamed(critter)

  if(critter.script == "tamed")
    return IsPlayer(GetMaster(critter));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function WasPrevTamedBy(critter, who)

  var List := GetTamersList(critter);

  if(List)
    if(List.exists(who.serial))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetPrevTamed(critter)

  return Cint(GetObjProperty(critter,"PrevTamed"));

endfunction

//--------------------------------------------------------------------------------

function IsNpcElemental(Npc, Elem)

  if(!Npc.isa(POLCLASS_NPC) and Npc.isa(POLCLASS_UOBJECT))
    return 0;
  endif

  var cfgElem := GetNpcConfig(Npc);

  if(!cfgElem)
    return cfgElem;
  endif
  
  var Elems := GetConfigStringArray(cfgElem, "elemental");
  if(!Elems or !Elems.size())
    return 0;
  endif

  if(IsVarType(Elem, VAR_ARRAY))
    foreach subElem in Elem
      if(subElem in Elems)
        return 1;
      endif
    endforeach
  else
    return (Elem in Elems);
  endif

endfunction

//-------------------------------------------------------------------------------

function IsGoodNpc(Npc)

  if(Npc.isa(POLCLASS_NPC))
    return !Npc.murderer;
  endif

  return 0;

endfunction

//-------------------------------------------------------------------------------

function IsOwner(who,item)

  if(who.cmdlevel >= 4)
    return 1;
  endif

  if(GetObjProperty(item, PROP_OWNER))
    if(CInt(GetObjProperty(item, PROP_OWNER)) != who.serial)
      return 0;
    else
      return 1;
    endif
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function IsKRPacketType(who)

  return who.compareversion("6.0.1.7");

endfunction

//-----------------------------------------------------------------------------


function IsOnDMRTest()

  return GetPackageByName("testing");

endfunction

//-----------------------------------------------------------------------------

