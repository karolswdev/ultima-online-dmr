//-----------------------------------------------------------------------------

include "include/player/friends";
include "include/const/attributes";
include "include/attributes/vitals";

//-----------------------------------------------------------------------------

const PROP_NO_FIGHT     := "#AN";
const PROP_FIGHT_DELAY  := "#Ah";
const PHYSIC_DMG        := 0;
const MAGIC_DMG         := 1;

//=============================================================================
//-----------------------------------------------------------------------------
// Damage
//-----------------------------------------------------------------------------
//=============================================================================

function DealDamage(defender, Dmg, attacker := 0, Type := PHYSIC_DMG)

  return run_script(":combat:dodamage", array(defender, VITALID_LIFE, Dmg, attacker, Type));

endfunction

//--------------------------------------------------------------------------------

function DealStaminaDamage(defender, Dmg, attacker := 0, Type := PHYSIC_DMG)

  return run_script(":combat:dodamage", array(defender, VITALID_STAMINA, Dmg, attacker, Type));

endfunction

//--------------------------------------------------------------------------------

function DealManaDamage(defender, Dmg, attacker := 0, Type := PHYSIC_DMG)

  return run_script(":combat:dodamage", array(defender, VITALID_MANA, Dmg, attacker, Type));

endfunction

//--------------------------------------------------------------------------------

function HealMobile(Mob, Amt, Healer, BlockCAG := 0)

  return run_script(":combat:doheal", array(Mob, VITALID_LIFE, Amt, Healer, BlockCAG));

endfunction

//--------------------------------------------------------------------------------

function HealStamina(Mob, Amt, Healer, BlockCAG := 0)

  return run_script(":combat:doheal", array(Mob, VITALID_STAMINA, Amt, Healer, BlockCAG));

endfunction

//--------------------------------------------------------------------------------

function HealMana(Mob, Amt, Healer, BlockCAG := 0)

  return run_script(":combat:doheal", array(Mob, VITALID_MANA, Amt, Healer, BlockCAG));

endfunction

//--------------------------------------------------------------------------------

function SaveLastHit(hitted,hitter)

  if(hitted and hitter)
    SetObjProperty(hitted, "#LastHitTime", ReadGameClock() );
    SetObjProperty(hitted, "LastHit", CInt(hitter.serial) );
  elseif(hitted)
    EraseObjProperty(hitted,"LastHit");
  endif

endfunction

//--------------------------------------------------------------------------------

function EraseLastHit(hitted)

  return EraseObjProperty(hitted, "LastHit");

endfunction

//--------------------------------------------------------------------------------

function GetLastHitter(victim)

  return FindPlayerBySerial(GetObjProperty(victim,"LastHit"));

endfunction

//--------------------------------------------------------------------------------

function CanDamage(who,Vic,Flags := 0, MainVic := 0)

  if(!Vic)
    return 0;
  endif

  if(who == Vic or Vic.cmdlevel or Vic.dead or !who or Vic.concealed)
    return 0;
  elseif(MainVic == Vic and !MainVic.hidden)
    return CanVsAttack(who, MainVic);
  endif

  if(!(Flags & CAN_DAMAGE_HIDDEN))
    if(Vic.hidden)
      return 0;
    endif
  endif
  
  if(!(Flags & NO_DIST_CHECK))
    if(!CheckSpecialLos(who,Vic))
      return 0;
    endif
  endif
  
  if(IsTamed(who))
    var Master := GetMaster(who);
    if(Master)
      if(Master == MainVic and MainVic)
        return 1;
      elseif(Master == Vic)
        return 0;
      elseif(Master.isa(POLCLASS_NPC))
        if(Vic.isa(POLCLASS_NPC))
          return 0;
        endif
      elseif(IsFriend(Master, Vic))
        return 0;
      elseif(RestrictedStatusAttack(Master, Vic))
        return 0;
      endif
    endif
  endif

  if(who.isa(POLCLASS_NPC))
    if(Vic.isa(POLCLASS_NPC))
      if(!IsTamed(Vic))
        return 0;
      else
        var Master := GetMaster(Vic);
        if(Master)
          if(Master == who)
            return 0;
          elseif(Master == MainVic and MainVic)
            return 1;
          elseif(Master.isa(POLCLASS_NPC))
            return 0;
          endif
        endif
      endif
    endif
  else
    if(IsFriend(who, Vic))
      return 0;
    elseif(RestrictedStatusAttack(who, Vic))
      return 0;
    endif
  endif
  
  return CanVsAttack(who, Vic);
  
endfunction

//-----------------------------------------------------------------------------

function CanDamageWall(Caster, CastOn)

  if(Caster.isa(POLCLASS_NPC))
    if(CastOn.isa(POLCLASS_NPC))
      if(!IsTamed(CastOn))
        return 0;
      elseif(GetMaster(CastOn).isa(POLCLASS_NPC))
        return 0;
      endif
    endif
  endif
  
  return CanVsAttack(Caster, CastOn);

endfunction

//-----------------------------------------------------------------------------

function CanVsAttack(Attacker, Defender)
  
  __ParamUnused(Attacker);
  __ParamUnused(Defender);
  /*
  var BAttacker := Attacker;
  var BDefender := Defender;
  var AStatus, DStatus;
  
  if(Attacker.isa(POLCLASS_NPC))
    Attacker := GetMaster(Attacker);
  endif

  if(Defender.isa(POLCLASS_NPC))
    Defender := GetMaster(Defender);
  endif
  
  __NONPVP__ DELETED
  if(IsPlayer(Attacker) and IsPlayer(Defender)) // pvp
    if(BAttacker.isa(POLCLASS_NPC)) 
      if(IsYoung(Attacker))   // Mlody zawsze ma PVP_NO niezaleznie czy mob jest w strefie czy nie
        AStatus := PVP_NO;
      else
        AStatus := __ifelse(IsInNonPvPArea(BAttacker.x,BAttacker.y,BAttacker.realm),PVP_NO, PVP_YES);  // Jesli nie jest mlody, to mob ma status taki w jakiej strefie jest
      endif
    else
      AStatus := GetPvPStatus(Attacker);  // normalne odczytanei statusu atakujacego.
    endif

    if(BDefender.isa(POLCLASS_NPC))
      if(IsYoung(Defender))  // Mlody zawsze ma PVP_NO niezalaleznie czy mob jest w strefie czy nie
        DStatus := PVP_NO;
      elseif(GetPvPStatus(Defender) == PVP_FORCED_YES)  // Jezeli gracz ma status PVP_FORCED_YES to jego mob tez.
        DStatus := PVP_FORCED_YES;
      else
        DStatus := __ifelse(IsInNonPvPArea(BDefender.x,BDefender.y,BDefender.realm),PVP_NO,PVP_YES);  // Jezeli nie jest mlody i nie ma PVP_FORCED_YES, to mob ma status taki w jakiej jest strefie
      endif
    else
      DStatus := GetPvPStatus(Defender);  // normlne odczytanie statsu broniacego
    endif

    if (AStatus and DStatus)  // jesli obaj maja przynajmniej status PVP_YES
      return 1;
    elseif (DStatus == PVP_FORCED_YES) // Atakujacy moze zaatakowac, jezeli obronca ma PVP_FORCED_YES
      return 1;
    else
      return 0;
    endif
  endif*/

  return 1;

endfunction
/*
//-----------------------------------------------------------------------------

function GetPvPStatus(who)
  return GetObjProperty(who, PROP_PVP_STATUS);
endfunction

//-----------------------------------------------------------------------------

function CanChangePvPStatus(who)
  
  if(CInt(GetObjProperty(who, PROP_PVP_NO_CHANGE)) < ReadGameClock());
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetPvPStatus(who, value, time := 0) // PVP_YES | PVP_NO | PVP_FORCED_YES
  case(value)
    0: 
    1:
    2: SetObjProperty(who, PROP_PVP_STATUS,value);
  endcase
  time := CInt(time);
  if(time)
    SetObjProperty(who, PROP_PVP_NO_CHANGE, ReadGameClock() + time);
  endif
endfunction
*/
//-----------------------------------------------------------------------------

function RedPvPDamage(byref Attacker,byref Defender,Dmg, type := PHYSIC_DMG)
  
  if(!IsPlayer(Attacker) or !IsPlayer(Defender))
    return Dmg;
  endif

  case (type)
    PHYSIC_DMG: type := "P";
    MAGIC_DMG:  type := "M";
  default: return Dmg;
  endcase

  if(CInt(GetObjProperty(Defender,"#"+type+"DmgClock")) > ReadMillisecondClock())
    if(GetObjProperty(Defender,"#Last"+type+"DmgPlayer") != Attacker.serial)
      Dmg := CInt(Dmg * CDbl(0.550));
    endif
  endif
  

  SetObjProperty(Defender,"#"+type+"DmgClock",ReadMillisecondClock() + 6000 + RandomInt(1000));
  SetObjProperty(Defender,"#Last"+type+"DmgPlayer",Attacker.serial);
  
  return Dmg;

endfunction

//-----------------------------------------------------------------------------

function RestrictedStatusAttack(who, Vic)

  if(GetObjProperty(who, "RestrictedStAttack"))
    if(IsMurderer(who) and IsMurderer(Vic))
      return 1;
    elseif(!IsMurderer(who) and IsOrder(Vic))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ArchDelay(Who, Weapon, Mod := 0)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  var Delay := CInt(ItemCfg[Weapon.objtype].HitDelay);
  if(!Delay)
    Delay := 4000;
  endif
  // goqsane: the multiplier was 1.5, changed to 3.5 to expose more dexterity influence.
  Delay :=  Delay - (GetEffectiveDexterityBaseValue(who) * 2);
  Delay :=  Delay * (1 + Mod);
  SetObjProperty(who,PROP_FIGHT_DELAY,CStr(CInt(ReadMillisecondClock() + Delay))); 

endfunction

//-----------------------------------------------------------------------------

function GetAttackSkill(attacker)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(attacker.isa(POLCLASS_NPC))
    if(IsHuman(attacker))
      return FindSkillidByIdentifier(ItemCfg[attacker.weapon.objtype].Attribute);
    else
      return FindSkillidByIdentifier(GetNpcConfig(attacker).AttackAttribute);
    endif
  elseif(IsPlayer(attacker))
    return FindSkillidByIdentifier(ItemCfg[attacker.weapon.objtype].Attribute);
  endif

  return error;
  
endfunction

//-----------------------------------------------------------------------------
