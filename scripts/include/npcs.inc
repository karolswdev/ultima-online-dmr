//-----------------------------------------------------------------------------

use uo;
use os;
use cfgfile;
use util;

//-----------------------------------------------------------------------------

include "include/bits";
include "include/client/cids";
include "include/logerror";
include "include/attack";
include "include/move";

include "include/mobile/npcs/utils";
include "include/mobile/npcs/attack";
include "include/mobile/npcs/name";
include "include/mobile/npcs/creation";
include "include/mobile/npcs/properties";

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

const BIRD_IDX               := 1;
const DAEMON_IDX             := 2;
const DRAGON_IDX             := 3;
const EVILMAGE_IDX           := 4;
const MALE_IDX               := 5;
const FEMALE_IDX             := 6;
const PIRATE_MALE_IDX        := 7;
const PIRATE_FEMALE_IDX      := 8;
const LIZARD_IDX             := 9;
const ORC_IDX                := 10;
const OGRE_IDX               := 11;
const RATMAN_IDX             := 12;

//--------------------------------------------------------------------------------

const NPCF_NO_UPKEEP         := 0x00000001;
const NPCF_NO_LOOT           := 0x00000002;
const NPCF_GUARD_KILL        := 0x00000004;
const NPCF_NO_CORPSE         := 0x00000008;
const NPCF_NO_LOOTING        := 0x00000010;
const NPCF_NO_WANDER         := 0x00000020;
const NPCF_FROZEN            := 0x00000040;
const NPCF_NO_ANCHOR         := 0x00000080;
const NPCF_GUARDIGNORE       := 0x00000100;
const NPCF_MIN_LOOT          := 0x00000200;
const NPCF_ANCHOR_ENABLED    := 0x00000400;
const NPCF_DOUBLE_LOOT       := 0x00000800;
const CRNPCF_NOMULTI         := 0x00000001;
const CRNPCF_CHECKLOS        := 0x00000002;

//--------------------------------------------------------------------------------

const NPCELEM_ELEMENTAL      := "elemental";
const NPCELEM_UNDEAD         := "undead";
const NPCELEM_DAEMON         := "daemon";
const NPCELEM_WOLF           := "wolf";
const NPCELEM_DRAGON         := "dragon";
const NPCELEM_POISON         := "poison";
const NPCELEM_COLD           := "cold";
const NPCELEM_FIRE           := "fire";
const NPCELEM_LIGHT          := "light";
const NPCELEM_VAMPIRE        := "vampire";

//=============================================================================
//-----------------------------------------------------------------------------
// Kill NPC
//-----------------------------------------------------------------------------
//=============================================================================

function KillNPC(byref Npc, param := 0, attacker := 0)

  if(Npc and Npc.isa(POLCLASS_NPC))
    
    if(param)
      Npc.hidden := 1;
      MoveToKillingPlace(Npc);
    endif

    if(GetEquipmentByLayer(Npc,LAYER_MOUNT))
      DestroyItem(GetEquipmentByLayer(Npc,LAYER_MOUNT));
    endif

    RevokePrivilege(Npc, "invul" );
    EraseInvulToAttack(Npc);
    SetFlag(Npc, NPCF_GUARD_KILL);
    
    if(!DealDamage(Npc, GetMaxHp(Npc), attacker))
      return 0;
    endif

    if(!Npc)
      return 1;
    endif

    if(GetHp(Npc) > 0)
      var n := 1;
      while(CInt(GetHp(Npc)) > 0 and n < 20)
        if(!DealDamage(Npc, 6000, attacker))
          return 0;
        endif
        n += 1;
      endwhile

      if(n == 20)
        LogError("KillNPC: Nie udalo sie zabic Npeca ["+Npc.name+"/"+Npc.serial+"] Hp: ["+GetHp(Npc)+"/"+GetMaxHp(Npc)+"]");
      endif
    endif

    if(!FindNpcBySerial(Npc.serial))
      return 1;
    endif
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function MoveToKillingPlace(who)

  MoveObject(who,6104,1184,0,BRITB_REALM,MOVECHAR_FORCELOCATION);
  
endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Backpacks
//-----------------------------------------------------------------------------
//=============================================================================

function GetNpcBackpack(me)

  var Backpack := me.backpack;

  if(!Backpack)
    Backpack := CreateItemAtTempLoc(UOBJ_NPCPACK);
    if(Backpack)
      if(!EquipItem(me, Backpack))
        DestroyItem(Backpack);
      endif
    endif
  endif

  return Backpack;

endfunction

//-----------------------------------------------------------------------------

function GetMerchantStorage()

  var Storage := FindStorageArea( "Merchant Storage" );

  if(!Storage)
    Storage := CreateStorageArea( "Merchant Storage" );
    if(!Storage)
      LogError("GetMerchantStorage: Blad podczas tworzenia Storage Area kupcow! "+Storage);
    else
      LogError("GetMerchantStorage: Tworzenie Storage Area kupcow.");
    endif
  endif

  return Storage;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Others
//-----------------------------------------------------------------------------
//=============================================================================

function CreateNpcBlood(Npc, Chance, Flags)

  if(Npc.isa(POLCLASS_NPC))
    if(IsNpcElemental(Npc, NPCELEM_UNDEAD))
      return 0;
    elseif(IsNpcElemental(Npc, NPCELEM_DAEMON))
      return 0;
    endif
  endif

  if(RandomInt(100) < Chance)
    return CreateBlood(Npc.x, Npc.y, Npc.z, Npc.realm, Flags);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetMercSkillID(Npc)

  var mercElem    := GetNpcMercConfig(Npc);
  var AttributeID := mercElem.Attribute;
  
  return GetSkillIDByAttributeID(AttributeID);

endfunction

//-----------------------------------------------------------------------------

function GetNecroControlledTime(me)

  return Cint(GetObjProperty(me,"NecroControlled"));

endfunction

//-----------------------------------------------------------------------------

function IsNecroControl(me)

  if(GetNecroControlledTime(me))
    if(GetNecroControlledTime(me) > ReadGameClock())
      return 1;
    endif
  endif
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------
