//-----------------------------------------------------------------------------

include "include/attributes/main";
include "include/client/misc";
include "include/start";
include "include/buffs";

//-----------------------------------------------------------------------------

const MOD_STR          := 1;
const MOD_DEX          := 2;
const MOD_INT          := 3;
const MOD_AR           := 4;
const MOD_PARA         := 5;
const MOD_INCO         := 6;
const MOD_GRAPHIC      := 7;
const MOD_COLOR        := 8;
const MOD_HGHOSTS      := 9;
const MOD_RA           := 10;
const MOD_TPREFIX      := 11;
const MOD_TSUFFIX      := 12;
const MOD_TGUILD       := 13;
const MOD_TRACE        := 14;
const MOD_LIGHT        := 15;
const MOD_POISON_PROT  := 16;
const MOD_LIFE         := 17;
const MOD_MANA         := 18;
const MOD_STAMINA      := 19;
const MOD_FIRERESIST   := 20;
const MOD_COLDRESIST   := 21;
const MOD_LIGHTRESIST  := 22;
const MOD_HAIRS        := 23; // array(HairsObjType, HairsColor)
const MOD_STATUS       := 24; // 1 - Blue, 2 - Crim, 3 - Murd
const MOD_HP_REGEN     := 25;
const MOD_MANA_REGEN   := 26;
const MOD_STAM_REGEN   := 27; // + ms regen rate
const MOD_CTH          := 28; // Zwieksza Cth moba, -1000 - 1000.
const MOD_LOWER_RC     := 29; // Lower Reagent Cost - odejmuje koszt regow, Min(RegAmt - 1, 1)
const MOD_FASTER_CAST  := 30;
const MOD_MANA_COST    := 31;
const MOD_SPELL_DMG    := 32;

const MOD_POISON_DMG   := 33;
const MOD_FIRE_DMG     := 34;
const MOD_COLD_DMG     := 35;
const MOD_LIGHT_DMG    := 36;

const MOD_MOUNT        := 37; // array(MountGraphic, MountColor)
const MOD_PENETRATE    := 38; // penetracja, -1000 - 1000, promil dodania do cth atakujacemu moba ktory ma tego tempmoda
const MOD_UNTOUCHABLE  := 39; // npece z maxhp/2 < Mod[1] nie beda atakowac, gdzie Mod[2] to array elementow (NPCELEM_UNDEAD/fire etc) niesprawdzanych 
const MOD_UNDEAD_SLAY  := 40;
const MOD_DAEMON_SLAY  := 41;
const MOD_ORDER_SLAY   := 42; // Slayery - -50 - 100 Mod, % dodania do dmg
const MOD_DEFENSE      := 43; // -50 - 50 mniejsza szansa na Dismount / range attack / Curse / Blind
const MOD_MONSTER_AR   := 44; // max ilosc przyjetych hitow od potworow
const MOD_SILVER_AR    := 45; // max ilosc przyjetych hitow ze srebra
const MOD_FIRE_BLOOD   := 46; // ilosc fireblood hitow
const MOD_ONHIT_AR     := 47; // max wartosc ar do jakiej moze urosnac, jesli urosnie w duration, to czas jest wydluzany
const MOD_LIFE_STOLEN  := 48; // 0-100% hp stolen from dmg

const MOD_RETORT       := 49; // Riposta, zadawany dmg zwieksza TempModAmount, co jest szansa na 2x mocniejszy cios
const MOD_CRIT_HIT     := 50; // zwiekszona szansa na krytyczne uderzenie (-1000 - 1000)
const MOD_STAM_DMG     := 51; // (60% szansy) zabiera stamine (ModAmount - ilosc) podczas ataku
const MOD_DISARM       := 52; // szansa na disarm podczas ataku (TempMod jako szansa 0-100)
const MOD_STUPOR       := 53; // dismount + krotki paraliz (TempMod jako ilosc hitow)
const MOD_2H_DMG       := 54; // TwoHanded Weapon Damage, TempMod - szansa 0-100 

/* 
Dostêpne predkosci dla gracza:
RUNSPEED_NORMAL: 0=normal (normalnie biega, normalnie chodzi, na koniu/z buta)
RUNSPEED_FAST:   1=szybka (biega z buta jak na koniu, ale na koniu biega normalnie) - to co ma wilk
RUNSPEED_SLOW:   2=wolna - nie da sie biegac, po prostu, ani z buta ani na koniu, samo chodzenie
RUNSPEED_HYBRID: 3=hybrydowa - ma predkosc biegania, ale animacje chodzenia ;)
*/

const MOD_RUN_SPEED    := 55; 

const MOD_RED_SPELL    := 56; // reduction spell damage, redukuje obrazenia czarow 0-100%
const MOD_CHARM_ANIMAL := 57; // jesli jest on, to zwierzaki nie atakuja osoby
const MOD_DISMOUNT_RES := 58; // odpornosc na dismount 0-100%
const MOD_MAGIC_SHIELD := 59; // odbicie magicznymi obrazeniami przy sparowaniu tarcza, wartosc = DMGTYPE_FIRE itp

const MOD_SLOWDOWN     := 60; // amount to ilosc o ile zostanie zmniejszony run_speed npeca (max 255). Mod nie dziala na graczy (z wiadomych przyczyn)
//nowe mody na potrzebe burzy druida
const MOD_CASTSIGHT    := 61;
const MOD_DMG_REDUCE   := 62;
const MOD_TREE_SUPPORT := 63; // max ilosc przyjetych hitow od potworow

const MOD_BLOCK_SELFHEAL := 64;
const MOD_BLOCK_CAST     := 65;
const MOD_BLOCK_FIGHT    := 66;

const MOD_MANA_STOLEN    := 67; // 0-100% mana stolen from dmg
const MOD_MANA_DMG       := 68; // 0-100% mana damage from physical damage
const MOD_MULTI_SHOT     := 69;
const MOD_EXPLO_DMG      := 70;

//--------------------------------------------------------------------------------
// Do Mod Flags
//--------------------------------------------------------------------------------

const MODF_FORCE_MODIFY      := 0x01;
const MODF_CHECK_VAMPIRE     := 0x02;

const MODDUR_INFINITE        := -1;

//-----------------------------------------------------------------------------
// Undo Mod Flags
//-----------------------------------------------------------------------------

const UMDF_WAIT              := 0x01;
const UMDF_NOCHECK           := 0x02;
const UMDF_OLD_WAIT          := 0x04;
const UMDF_PREVIOUS          := 0x08;

//-----------------------------------------------------------------------------
// Props
//-----------------------------------------------------------------------------

const PROP_TEMP_PREVMOD      := "#ModPrev_";
const PROP_TEMP_MOD          := "#Mod_";
const PROP_MOD_DELAY         := "#ModDelay_";
const PROP_MAGIC_MOD         := "#MagMod_";
const PROP_TEMP_SKMOD        := "#SkMod_";
const PROP_MAGIC_SKMOD       := "#MagSkMod_";
const PROP_STATIC_MOD        := "Mod_";

const PROP_TEMP_MOD_AMT      := "#ModAmt_";
const PROP_TMOD_PREV_AMT     := "ModPrevAmt_";

//================================================================================
//--------------------------------------------------------------------------------
// Temp Mod
//--------------------------------------------------------------------------------
//================================================================================

function CanMod(who, GetType)
  
  var Types := array{GetType};
  Types := Types + GetDependantMod(GetType);

  foreach Type in Types
    if(GetObjProperty(who, PROP_TEMP_MOD + Type) != error)
      
      return 0;

    elseif(who.dead)

      return error;

    elseif(type == MOD_GRAPHIC or type == MOD_MOUNT)

      if(!who.isA(POLCLASS_NPC))
        if(GetEquipmentByLayer(who,LAYER_MOUNT))
          return error;
        endif
      endif

    elseif(CInt(GetObjProperty(who, PROP_MOD_DELAY + Type)) > ReadGameClock())

      return error;

    elseif(type == MOD_SLOWDOWN)

      if(!who.isA(POLCLASS_NPC))
        return error;
      endif

    endif

    if(GetModOldAmount(who, Type) != error)
      UndoMod(who, Type, UMDF_NOCHECK | UMDF_OLD_WAIT);
    endif
  endforeach

  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function CanModAny(who, arrMods)

  foreach Mod in arrMods
    if(CanMod(who, Mod))
      return 1;
    endif
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CanModifyMod(who, Type)

  if(!CanMagicMod(who, Type))
    return 0;
  endif

  case(CanMod(who, Type))
    0:           case(Type)
                   MOD_HAIRS:           
                   MOD_MOUNT:   
                   MOD_HGHOSTS: 
                   MOD_LIGHT:
                   MOD_PARA:      return 0;
                 endcase

                 var CurMod := GetObjProperty(who, PROP_TEMP_MOD + Type);

                 if(CurMod == error)
                   return 1;
                 else
                   
                   if(!IsDependantMod(who, Type)) // jesli jest blokniety przez dependa i ma juz moda, to nie mozna podwoic.
                     if(!GetObjProperty(who,PROP_TEMP_PREVMOD + Type))
                       return 1;
                     endif
                   endif
                 endif

    1:           return 1;
    default:     return error;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function DoMod(who, type, amt, duration, StickItem := 0, Flags := 0)

  if(who.isa(POLCLASS_MOBILE))
    var ModCore     := FindModCore(who);
  
    if(ModCore)
      var Parms       := struct;

      Parms.+type     := EVID_MOD;
      Parms.+modtype  := type;
      Parms.+amt      := amt;
      Parms.+duration := duration;
      Parms.+stkItem  := StickItem;
      Parms.+source   := who;
      Parms.+Flags    := Flags;

      ModCore.sendevent(Parms);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function ModifyMod(who, type, amt, duration)

  if(who.isa(POLCLASS_MOBILE))
    var ModCore     := FindModCore(who);
  
    if(ModCore)
      var Parms       := {};

      Parms.+type     := EVID_MOD;
      Parms.+modtype  := type;
      Parms.+amt      := amt;
      Parms.+duration := duration;
      Parms.+source   := who;
      Parms.+Flags    := MODF_FORCE_MODIFY;

      ModCore.sendevent(Parms);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetModDelay(who, Type, Duration)

  var curTime := GetModDelay(who, MOD_PARA);

  // sprawdzmy, czy czas jaki chcemy ustawic nie jest mniejszy od biezacego
  if(curTime != error)
    if(curTime > Duration)
      return; // ups, jest mniejszy, wiec nie chcemy go ustawic
    endif
  endif

  SetObjProperty(who, PROP_MOD_DELAY + Type, ReadGameClock() + Duration);

endfunction

//-----------------------------------------------------------------------------

function GetModDelay(who, Type)

  var Prop := GetObjProperty(who, PROP_MOD_DELAY + Type);
  
  if(Prop != error)
    return ToMin(CInt(Prop) - ReadGameClock(), 0);
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function GetModAmount(who, Type)

  var ModProps := GetObjProperty(who, PROP_TEMP_MOD + Type);

  if(ModProps != error)
    return ModProps[1];
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetModDuration(who, Type)

  return CInt(GetObjProperty(who, PROP_TEMP_MOD + Type)[2]);

endfunction

//-----------------------------------------------------------------------------

function GetModStartTime(who, Type)

  return CInt(GetObjProperty(who, PROP_TEMP_MOD + Type)[3]);

endfunction

//-----------------------------------------------------------------------------

function GetModOldAmount(who, Type)

  return GetObjProperty(who, PROP_TMOD_PREV_AMT + Type);

endfunction

//-----------------------------------------------------------------------------

function SetModOldAmount(who, Type, byref nValue)

  SetObjProperty(who, PROP_TMOD_PREV_AMT + Type, nValue);

endfunction

//-----------------------------------------------------------------------------

function GetModLeftTime(who, Type)

  if(CanMod(who, Type) == 0)
    return (GetModStartTime(who, Type) + GetModDuration(who, Type)) - ReadGameClock();
  endif

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Temp Skill Mod
//--------------------------------------------------------------------------------
//================================================================================

function CanSkillMod(who, SkillID)

  if(GetObjProperty(who, PROP_TEMP_SKMOD + SkillID))
    return 0;
  elseif(who.dead)
    return error;
  else
    return 1;
  endif
  
endfunction

//-----------------------------------------------------------------------------

function CanSkillModAny(who, arrSkillIds)

  if(who.dead or !arrSkillIds)
    return error;
  endif

  foreach SkillID in arrSkillIds
    if(!GetObjProperty(who, PROP_TEMP_SKMOD + SkillID))
      return 1;
    endif
  endforeach
  
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function DoSkillMod(who,SkillID,amt, duration, sound := 0)

  if(who.isa(POLCLASS_MOBILE))
    var ModCore     := FindModCore(who);
    if(ModCore)
      var Parms       := {};
    
      Parms.+type     := EVID_SKMOD;
      Parms.+skid     := SkillID;
      Parms.+amt      := amt;
      Parms.+duration := duration;
      Parms.+sound    := sound;
      Parms.+source   := who;
      Parms.+Flags    := 0;
      ModCore.sendevent(Parms);
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function GetSkillModAmount(who, SkillID)

  return CInt(GetObjProperty(who, PROP_TEMP_SKMOD + SkillID)[1]);

endfunction

//-----------------------------------------------------------------------------

function GetSkillModDuration(who, SkillID)

  return CInt(GetObjProperty(who, PROP_TEMP_SKMOD + SkillID)[2]);

endfunction

//-----------------------------------------------------------------------------

function GetSkillModTime(who, SkillID)

  return CInt(GetObjProperty(who, PROP_TEMP_SKMOD + SkillID)[3]);

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Undo Mods
//--------------------------------------------------------------------------------
//================================================================================

function UndoMod(who, type, Flags := 0)

  if(who.isa(POLCLASS_MOBILE))
    if(!(Flags & UMDF_NOCHECK))
      if(CanMod(who,type) == 1)
        return 0;
      endif
    endif

    var ModCore     := FindModCore(who);

    var Parms       := struct;
    Parms.+type     := EVID_UNDOMOD;
    Parms.+modtype  := type;
    Parms.+source   := who;
    Parms.+flags    := Flags;
    Parms.+pid      := GetPid();

    if(ModCore and (IsLoggedIn(who) or who.isa(POLCLASS_NPC)))
      ModCore.sendevent(Parms);
    else
      start_script(":magiceffects:undomod",Parms);
    endif

    if(Flags & UMDF_WAIT)
      
      var Count := 0;
      var retEv;

      while(Count <= 6)
        Count += 1;
        retEv := wait_for_event(1);

        if(retEv.undoDone)  
          return 1;
        endif
      endwhile

      return 0;

    elseif(Flags & UMDF_OLD_WAIT)

      Parms := 0;
      sleepms(20);
      while(Parms < 60 and GetModOldAmount(who, Type) != error)
        Parms := Parms + 1;
        sleepms(100);
      endwhile

      return (Parms != 60);

    endif

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function UndoSkillMod(who, SkillID)

  if(who.isa(POLCLASS_MOBILE))
    if(CanSkillMod(who,SkillID) == 1)
      return 0;
    endif

    var ModCore     := FindModCore(who);
    var Parms       := {};

    Parms.+type     := EVID_UNDOSKMOD;
    Parms.+skid     := SkillID;
    Parms.+source   := who;

    if(ModCore and (IsLoggedIn(who) or who.isa(POLCLASS_NPC)))
      ModCore.sendevent(Parms);
    else
      start_script(":magiceffects:undomod",Parms);
    endif

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckForLongModDur(who, Type)

  if(CanMod(who, Type) == 0)
    return 0;
  endif

  if(GetModOldAmount(who, Type) != error)
    UndoMod(who, Type, UMDF_NOCHECK);
  endif

  return 1;
  
endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Magic Mods
//--------------------------------------------------------------------------------
//================================================================================

function CanMagicMod(who,ModType)

  if(GetMagicModItemSerial(who, ModType) or who.dead)
    return 0;
  endif
  return 1;

endfunction

//-----------------------------------------------------------------------------

function DoMagicMod(who,item,ModType,MagicLevel)

  var ModCore := FindModCore(who);
  if(ModCore)
    var Parms := {};

    Parms.+type    := EVID_MAGICMOD;
    Parms.+item    := item;
    Parms.+modtype := ModType;
    Parms.+mlvl    := MagicLevel;
    Parms.+source  := who;

    ModCore.sendevent(Parms);

  endif

endfunction

//-----------------------------------------------------------------------------

function GetTempModAmount(who, Type)

  return CInt(GetObjProperty(who, PROP_TEMP_MOD_AMT + Type));

endfunction

//-----------------------------------------------------------------------------

function DecreaseTempModAmount(who, Type, Amt)

  var curAmt := GetTempModAmount(who, Type);
  var prevAmt := Min(Amt, curAmt);

  curAmt := Max(curAmt - Amt, 0);

  if(curAmt > 0)
    SetTempModAmount(who, Type, curAmt);
  else
    EraseTempModAmount(who, Type);
  endif

  return prevAmt; // zwracamy wartosc o jaka zostal obnizony mod

endfunction

//-----------------------------------------------------------------------------

function SetTempModAmount(who, Type, Amount)

  return SetObjProperty(who, PROP_TEMP_MOD_AMT + Type, CInt(Amount));

endfunction

//-----------------------------------------------------------------------------

function EraseTempModAmount(who, Type)

  return EraseObjProperty(who, PROP_TEMP_MOD_AMT + Type);

endfunction

//-----------------------------------------------------------------------------

function SetMagicModItemSerial(who, item, ModType)

  SetObjProperty(who, PROP_MAGIC_MOD + ModType, item.serial);

endfunction

//-----------------------------------------------------------------------------

function GetMagicModItemSerial(who, ModType)

  return CInt(GetObjProperty(who, PROP_MAGIC_MOD + ModType));

endfunction

//-----------------------------------------------------------------------------

function GetMagicModItem(who, ModType)

  var ItemSer := GetMagicModItemSerial(who, ModType);

  if(ItemSer)
    foreach Item in ListEquippedItems(who)
      if(Item.serial == ItemSer)
        return Item;
      endif
    endforeach
  endif

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Magic Skill Mods
//--------------------------------------------------------------------------------
//================================================================================

function CanMagicSkillMod(who, SkillID)

  if(GetMagicSkModItemSerial(who, SkillID) or who.dead)
    return 0;
  else
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function DoMagicSkillMod(who, item, SkID, MagicLevel)

  var ModCore := FindModCore(who);
  if(ModCore)
    var Parms := {};

    Parms.+type    := EVID_MAGICSKMOD;
    Parms.+item    := item;
    Parms.+skid    := SkID;
    Parms.+mlvl    := MagicLevel;
    Parms.+source  := who;

    ModCore.sendevent(Parms);
  endif

endfunction

//-----------------------------------------------------------------------------

function UndoMagicSkillMod(who, ItemSer, SkID, ModAmt)

  if(!CanMagicSkillMod(who, SkID) and GetMagicSkModItemSerial(who, SkID) == ItemSer)
    EraseMagicSkModItemSerial(who, SkID);
    ModifySkillTemporaryMod(who, SkID,0-ModAmt);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMagicSkModItemSerial(who, SkillID)

  return CInt(GetObjProperty(who, PROP_MAGIC_SKMOD + SkillID));

endfunction

//-----------------------------------------------------------------------------

function SetMagicSkModItemSerial(who, item, SkillID)

  SetObjProperty(who, PROP_MAGIC_SKMOD + SkillID, item.serial);

endfunction

//-----------------------------------------------------------------------------

function EraseMagicSkModItemSerial(who, SkillID)

  EraseObjProperty(who, PROP_MAGIC_SKMOD + SkillID);

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Misc
//--------------------------------------------------------------------------------
//================================================================================

function FindModCore(who)

  var CorePid;
  if(who.isa(POLCLASS_NPC))
    CorePid := GetGlobCtrlProc(NPCMODCORE_SCNAME);
  else
    CorePid := CInt(GetObjProperty(who,PROP_MODCORE_PID));
    if(CorePid)
      CorePid := GetProcess(CorePid);
    endif
  endif

  return CorePid;

endfunction

//-----------------------------------------------------------------------------

function UndoAllMods(who, Flags := 0)

  foreach Mod in GetAllMods()
    if(CanMod(who,Mod) == 0)
      UndoMod(who, Mod, Flags);
    endif
  endforeach
  
  var i, maxSkId := GetMaxSkillId();
  for(i:=0;i<=maxSkId;i:=i+1)
    if(CanSkillMod(who,i) == 0)
      UndoSkillMod(who, i);
    endif
  endfor

endfunction

//-----------------------------------------------------------------------------

function GetAllMods()

  return {
    MOD_STR,
    MOD_DEX,
    MOD_INT,
    MOD_LIFE,
    MOD_STAMINA,
    MOD_MANA,
    MOD_INCO,
    MOD_TPREFIX,
    MOD_TSUFFIX,
    MOD_TGUILD,
    MOD_TRACE,
    MOD_AR,
    MOD_GRAPHIC,
    MOD_COLOR,
    MOD_PARA,
    MOD_HGHOSTS,
    MOD_RA,
    MOD_LIGHT,
    MOD_POISON_PROT,
    MOD_STATUS,
    MOD_FIRERESIST,
    MOD_COLDRESIST,
    MOD_LIGHTRESIST,
    MOD_HP_REGEN,
    MOD_MANA_REGEN,
    MOD_STAM_REGEN,
    MOD_CTH,
    MOD_LOWER_RC,
    MOD_FASTER_CAST,
    MOD_MANA_COST,
    MOD_SPELL_DMG,
    MOD_POISON_DMG,
    MOD_COLD_DMG,
    MOD_FIRE_DMG,
    MOD_LIGHT_DMG,
    MOD_MOUNT,
    MOD_HAIRS,
    MOD_PENETRATE,
    MOD_UNTOUCHABLE,
    MOD_UNDEAD_SLAY,
    MOD_DAEMON_SLAY,
    MOD_ORDER_SLAY,
    MOD_DEFENSE,
    MOD_TREE_SUPPORT,
    MOD_MONSTER_AR,
    MOD_SILVER_AR,
    MOD_FIRE_BLOOD,
    MOD_MANA_STOLEN,
    MOD_MULTI_SHOT,
    MOD_EXPLO_DMG,
    MOD_MANA_DMG,
    MOD_ONHIT_AR,
    MOD_LIFE_STOLEN,
    MOD_RETORT,
    MOD_CRIT_HIT,
    MOD_STAM_DMG,
    MOD_DISARM,
    MOD_STUPOR,
    MOD_2H_DMG,
    MOD_RUN_SPEED,
    MOD_SLOWDOWN,
    MOD_RED_SPELL,
    MOD_CHARM_ANIMAL,
    MOD_DISMOUNT_RES,
    MOD_MAGIC_SHIELD,
    MOD_CASTSIGHT,    
    MOD_DMG_REDUCE,
    MOD_BLOCK_CAST,
    MOD_BLOCK_FIGHT,
    MOD_BLOCK_SELFHEAL
  };

endfunction

//-----------------------------------------------------------------------------
// Static Mods - On Npcs
//-----------------------------------------------------------------------------

function ApplyStaticMods(Npc)

  if(!Npc.isa(POLCLASS_NPC))
    return;
  endif

  if(GetObjProperty(Npc, PROP_STATIC_MOD + MOD_AR) != error)
    Npc.ar_mod := Npc.ar_mod + CInt(GetObjProperty(Npc, PROP_STATIC_MOD + MOD_AR));
  endif

endfunction

//-----------------------------------------------------------------------------

function GetStaticMod(Npc, ModType)

  if(Npc.isa(POLCLASS_NPC))
    return GetObjProperty(Npc, PROP_STATIC_MOD + ModType);
  else
    return error;
  endif

endfunction

//-----------------------------------------------------------------------------

function DelStaticMod(Npc, ModType)

  if(Npc.isa(POLCLASS_NPC))
    var cMod := GetStaticMod(Npc, ModType);

    if(cMod != error)

      case(ModType)
        MOD_AR:              Npc.ar_mod -= cMod;

      endcase

      EraseObjProperty(Npc, PROP_STATIC_MOD + ModType);
     
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetStaticMod(Npc, ModType, Amt)

  if(Npc.isa(POLCLASS_NPC))
    DelStaticMod(Npc, ModType);
    SetObjProperty(Npc, PROP_STATIC_MOD + ModType, Amt);
    ApplyStaticMods(Npc);
  endif

endfunction

//-----------------------------------------------------------------------------------//

function CheckLogonMods(who)

  if(CanMod(who, MOD_RUN_SPEED) == 0)
    SendMountSpeedPacket(who, 1);
  endif

endfunction

//-----------------------------------------------------------------------------

function CanGetMagicStatMod(who)

  return (GetBaseSkillBaseValue(who, SKILLID_KENSAI) == 0 and GetBaseSkillBaseValue(who, SKILLID_BARBARIAN) == 0);

endfunction

//-----------------------------------------------------------------------------------//

function CheckForNpcLongMods(Npc)

  CheckForLongModDur(Npc, MOD_SLOWDOWN);
  CheckForLongModDur(Npc, MOD_RUN_SPEED);

endfunction

//-----------------------------------------------------------------------------------//

function GetDependantMod(type)

  case (type)
    MOD_AR:         return array{MOD_ONHIT_AR};
    MOD_ONHIT_AR:   return array{MOD_AR};
    default:        return array{};
  endcase

endfunction

//-----------------------------------------------------------------------------------//

function IsDependantMod(who, type)

  var dTypes := GetDependantMod(type);

  foreach dType in dTypes
    if(GetObjProperty(who, PROP_TEMP_MOD + dType) != error)
      return 1;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------------//

function TemporarySlowdown(mobile, Time)

  if(CanMod(mobile, MOD_RUN_SPEED) == 0 and !mobile.isa(POLCLASS_NPC))
    var Amt := GetModAmount(mobile, MOD_RUN_SPEED);

    case(Amt)
      RUNSPEED_FAST:
        ModifyMod(mobile, MOD_RUN_SPEED, RUNSPEED_NORMAL, Time); 
      RUNSPEED_NORMAL:
        ModifyMod(mobile, MOD_RUN_SPEED, RUNSPEED_SLOW, Time); 
    endcase
  else
    DoMod(mobile, MOD_RUN_SPEED, RUNSPEED_SLOW, Time);
  endif

endfunction

//-----------------------------------------------------------------------------------//
