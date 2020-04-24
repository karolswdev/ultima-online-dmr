//------------------------------------------------------------------------

include "include/clock";
include "include/tempmods";
include "include/checks/locchecks";
include "include/buildings";
include "include/regs";
include "include/npcs";
include "include/std";
include "include/gumpboxes";
include "include/move";
include "include/checks/objchecks";
include "include/objtypes/magic";

include ":magic:magicatt";
include ":magic:magicgain";
include ":vampire:common";
include ":glyphs:glyphs";

//------------------------------------------------------------------------

const SPELL_RES_GAIN         := 1;
const SPELL_RES_ERROR        := 0;
const SPELL_RES_NORMAL       := -1;

//------------------------------------------------------------------------

var SpellGlob;

//================================================================================
//--------------------------------------------------------------------------------
// Magic
//--------------------------------------------------------------------------------
//================================================================================

function InitSpellGlob(Caster, SpellID, Parms)

  SpellGlob := struct;
  
  SpellGlob.+CastType := Parms[2];
  SpellGlob.+Flags    := Parms[4];
  SpellGlob.+Parms    := Parms;
  SpellGlob.+SpellId  := SpellId;
  SpellGlob.+Type     := GetSpellValue(SpellID, "SpellType");
  SpellGlob.+Circle   := GetSpellValue(SpellID,"Circle");
  SpellGlob.+AdditionalMana := GetSpellValue(SpellId, "AdditionalMana");
  SpellGlob.+FastCast := GetSpellValue(SpellId, "Fast");

  SpellGlob.+CasterLoc := struct;
  SpellGlob.CasterLoc.+x := Caster.x;
  SpellGlob.CasterLoc.+y := Caster.y;
  SpellGlob.CasterLoc.+z := Caster.z;
  SpellGlob.CasterLoc.+realm := Caster.realm;

  if(Caster.multi.isa(POLCLASS_BOAT))
    SpellGlob.CasterLoc.x := Abs(SpellGlob.CasterLoc.x - Caster.multi.x);
    SpellGlob.CasterLoc.y := Abs(SpellGlob.CasterLoc.y - Caster.multi.y);
  endif

  SpellGlob.+CasterMount := GetEquipmentByLayer(Caster, LAYER_MOUNT);
  SpellGlob.+DmgType := GetSpellDamageType(SpellID);

endfunction

//--------------------------------------------------------------------------------

function GetSpellDamageType(SpellID)

  var DmgType  := GetSpellValue(SpellID, "Element", STRING_ID);

  if(!DmgType)
    DmgType := 0;
  else
    DmgType := SplitWords(DmgType);

    case(DmgType[1])
      SPELLELEM_FIRE:     DmgType := DMGTYPE_FIRE;
      SPELLELEM_COLD:     DmgType := DMGTYPE_COLD;
      SPELLELEM_LIGHT:    DmgType := DMGTYPE_ELECTRIC;
      SPELLELEM_MISC:     DmgType := DMGTYPE_MISC;
      SPELLELEM_POISON:   DmgType := DMGTYPE_POISON;
      default:            DmgType := 0;
    endcase

  endif

  return DmgType;

endfunction

//--------------------------------------------------------------------------------

function SpellCast(SpellParms, SpellID, Type := TRGT_NONE, Noto := TRGO_NONE)

  SetLogRunCycles(LST_SPELL);

  var WasOffen := IsCastingOffensiveNear(SpellParms[1]);
  var Ret := __SpellCast(SpellParms, SpellID, Type, Noto);

  if(!WasOffen and Ret and (Noto & TRGO_HARMFUL))
    EraseOffensiveCast(SpellParms[1]);
  endif
  
  return Ret;

endfunction

//--------------------------------------------------------------------------------

function __SpellCast(byref SpellParms, SpellID, Type, Noto)

  var Parms           := GetSpellParms(SpellParms);
  var Caster          := Parms[1];
  var CastObject      := Parms[3];

  if(CanMod(Caster, MOD_BLOCK_CAST) == 0)
    Fizzle(Caster,"Nie mozesz rzucic teraz czaru. Twa magia zostala zablokowana!");
    return 0;
  endif

  TriggerTipEvent(Caster, TEID_MAGIC_CAST);
  InitSpellGlob(Caster, SpellID, Parms);

  var Steps           := GetSpellValue(SpellID,"Steps");
  var CastOn;
  
  if(ChNocZone(Caster))
    Fizzle(Caster,"Magia w tym miejscu zanika...");
    return 0;
  endif

  if(Noto & TRGO_HARMFUL)
    if(IsPlayer(Caster))
      if(!GetObjProperty(Caster, "BlockAskCityCast") and FindCity(Caster))
        if(!YesNoEx(Caster, "Jeste� w mie�cie, gdzie czarowanie ofensywne jest zabronione.|Czy na pewno chcesz rzuci� czar ofensywny?|Ryzykujesz zaatakowanie przez stra�nika."))
          Fizzle(Caster,"Anulowano.");
          return 0;
        endif
      endif
    endif

    if(IsPeacefulZone(Caster.x, Caster.y, Caster.realm))
      Fizzle(Caster,"Magia w tym miejscu nie dziala...");
      return 0;
    endif

    SetOffensiveCast(Caster);
  endif

  if(!SpellGlob.Type or !SpellGlob.Circle)
    Fizzle(caster,"Ta magia jest dla Ciebie nie znana.");
    return 0;
  endif

  if(!Caster.isa(POLCLASS_NPC))
    if(IsBusy(Caster,NO_BUSY_DELAY_CHECK | NO_COMBAT_CHECK))
      return;
    endif

    if(SpellGlob.CastType == CAST_PLAYER and SpellGlob.Type == SPTYPE_MAGE)
      var SpellBook := GetEquipmentByLayer(Caster,LAYER_HAND1);

      if(SpellBook.objtype != UOBJ_SPELLBOOK)
        SpellBook := GetEquipmentByLayer(Caster,LAYER_HAND2);
      endif

      if(SpellBook.objtype != UOBJ_SPELLBOOK)
        SpellBook := FindObjTypeInContainer(Caster.backpack,UOBJ_SPELLBOOK);
      endif

      if(SpellBook.objtype == UOBJ_SPELLBOOK)
	  
		if(!SpellBook.HasSpell(SpellID))
          Fizzle(Caster,"Nie masz tego czaru w swej ksiedze.");
          return 0;
		endif
		       
      else
        Fizzle(Caster,"Nie masz magicznej ksiegi!");
        return 0;
      endif
    endif
  else
    SpellGlob.CastType := CAST_NPC;
  endif

  if(SpellGlob.Type == SPTYPE_VAMPIRE and !Caster.isa(POLCLASS_NPC))
    if(!IsVampire(Caster))
      Fizzle(Caster, "Nie wiesz jak to rzucic...");
      return 0;
    endif
    
    var hour := GetHour();

    if(hour < VAMPIRE_HOUR and hour >= VAMPIRE_END_HOUR and !IsDungeon(Caster.x, Caster.y, Caster.realm))
      var DayCast := SplitWords(GetSpellValue(SpellID,"DayCast"));

      if(DayCast[1] != "1")
        Fizzle(Caster, "Nie mozesz rzucic tego czaru w dzien na powierzchni...");
        return 0;
      else
        if(GetEffectiveSkill(Caster,SKILLID_WEARWOLF) < CInt(DayCast[2]))
          Fizzle(Caster, "Masz zbyt malo umiejetnosci by rzucic ten czar w dzien na powierzchni...");
          return 0;
        endif
      endif
    endif
  endif

  SetObjProperty(Caster, "LastCastingSpell", SpellID );

  if(Caster.hidden)
    Caster.hidden := 0;
  endif

  if(!(SpellGlob.Flags & SPF_NOWORDS))
    SpeakPowerWords(caster,SpellID);
  endif

  if(IsPlayer(Caster))
    Caster.squelch(2);
  endif

  var Delay := GetCircleValue(SpellGlob.Circle,"SpellDelay");
  
  if(SpellGlob.FastCast)
	if(Caster.cmdlevel)
		SendSysMessage(Caster, "Delay := 0");
	endif
	  Delay := 0;
  endif

  // FasterEbolt support
  if(SpellID == 42)
    var fasterMod := GetGlyphMod(Caster, "FasterEbolt");
    if(fasterMod > 0)
      Delay := Delay - CInt(fasterMod * 100);
    endif
  endif
  
  var SpellManaCost   := GetCircleValue(SpellGlob.Circle,"UMana") + SpellGlob.AdditionalMana;

  var ArtMod          := GetTempModAmount(Caster,MOD_MANA_COST);

  if(ArtMod)
    SpellManaCost := SpellManaCost - CInt(SpellManaCost * (ArtMod * 0.01));
  endif

  if(IsBitOn(SpellGlob.Flags, SPF_USEMOREMANA))
    SpellManaCost := CInt(SpellManaCost*1.5);
  endif

  if(SpellGlob.Flags & SPF_NODELAY)
    Delay := 0;
  endif

  var Diff := GetCircleValue(SpellGlob.Circle,"Diff");

  case(SpellGlob.CastType)
    CAST_SCROLL:     Diff := GetCircleValue(Max(SpellGlob.Circle-1, 1),"Diff");
                     SpellManaCost := Max(SpellManaCost - Max(CInt(SpellManaCost * 0.15), 1), 1);

                     if(!CastObject)
                       Fizzle(Caster,"Stracil"+ggm(caster,3)+" koncetracje.");
                       return 0;
                     endif
    CAST_NPC:        Diff -= 10;
                     var Points := GetCircleValue(SpellGlob.Circle,"Points") - 10;
                     GainMagicPoints(Caster, SpellGlob.Type, Points, SpellGlob.Circle);
                     Delay     := CInt(Delay / 2);
    CAST_WAND:
                     if(!ReserveItem(CastObject))
                       Fizzle(Caster,"Stracil"+ggm(caster,3)+" koncetracje.");
                       return 0;
                     endif
  endcase

  if(!(SpellGlob.Flags & SPF_NOMANA))
    if(GetMana(Caster) < SpellManaCost)
      Fizzle(caster,"Brakuje Ci many.");
      return 0;
    endif
  endif

  if(!MainCast(Caster, SpellID))
    return 0;
  endif

  var AnimId := GetMagicAnimId(Caster, SpellID);

  // lets play first animation & ensure memory is thread safe
  if(Caster.ip)
	  Malloc(Caster);
  endif

  if(AnimId)
    PlayAction(Caster, AnimId);
  endif

  if(!GetObjProperty(Caster, "#FastCast") || !SpellGlob.FastCast)

      sleepms(1500);

  endif

  if(SpellGlob.CastType != CAST_NPC)
    if(!DoAnimation(Caster, SpellID))
      if(!(SpellGlob.Flags & SPF_NOMANA))
        ConsumeManaAmount(Caster, SpellManaCost / 2);
      endif
      return 0;
    endif
  endif

  Delay := Delay + (1400 - (GetCasterSkillValue(Caster) * 2)) - (GetTempModAmount(Caster,MOD_FASTER_CAST) * 210);
  
  if(GetObjProperty(Caster, "#FastCast"))
    //PrintTextAbove(Caster, "*wpada w szal*");
    Delay := 10;
  endif
  
  // goqsane: 02-24-2018: Added new reagent... grzyb_magic: should put down the delay per each consumed magic mushroom
  var specialReagent := FindItemInContainer(Caster.backpack, "grzyb_magic");
  
  if(specialReagent)
	if(SubtractAmount(specialReagent, 1))
		Delay := CInt(Delay * 0.82);
	endif
  endif

  if(Delay > 0)
    sleepms(Delay);
  endif

  if(SpellGlob.CasterMount != GetEquipmentByLayer(Caster, LAYER_MOUNT))
    Fizzle(Caster,"Stracil"+ggm(Caster,3)+" koncentracje.");
    return 0;
  endif

  if(Caster.dead or Caster.frozen or Caster.paralyzed)
    Fizzle(Caster, "Czar prysl...");
    return 0;
  elseif(!Caster)
    return 0;
  endif

  if(!CheckSpellSkill(Caster,Diff))
    if(!(SpellGlob.Flags & SPF_NOMANA))
      ConsumeManaAmount(Caster, SpellManaCost / 2);
    endif
    return 0;
  endif

  if(!(SpellGlob.Flags & SPF_NOMANA))
    if(!ConsumeManaAmount(caster,SpellManaCost))
      Fizzle(caster,"Brakuje Ci many.");
      return 0;
    endif
  endif
  
  var mod := GetGlyphMod(caster, "MreturnMana");
  if(mod)
	if(RandomInt(100) <= mod)
		PrintTextAbove(caster, "Mana powraca");
		SetMana(caster, GetMana(caster)+SpellManaCost);
	endif
  endif

  if(SpellGlob.CastType == CAST_NPC)
    CastOn := CastObject;

    if(CastOn != Caster and Caster.isa(POLCLASS_NPC) and CastOn.isa(POLCLASS_MOBILE))
      var Time := ReadGameClock() + 6;
      while(!CheckLineOfSight(Caster, CastOn) and Time > ReadGameClock())
        sleepms(250);
      endwhile
    endif

    /// musowo sprawdzic czy npc moze tluc w strefie nonpvp
    if(Noto & TRGO_HARMFUL)
      if(!CanVsAttack(Caster, CastOn))
        return 0;
      endif
    endif

    if(!CheckCastOn(Caster, CastOn))
      return 0;
    endif
  else
    CastOn := GetCastOn(Caster, SpellID, Type, Noto);
  endif

  if(!CastOn)
    return 0;
  endif

  case(SpellGlob.CastType)
    CAST_SCROLL:
                   if(!SubtractAmount(CastObject,1))
                     Fizzle(caster,"Stracil"+ggm(Caster,3)+" koncetracje.");
                     return 0;
                   endif
    CAST_WAND:
                   if(!CastObject)
                     Fizzle(Caster,"Stracil"+ggm(Caster,3)+" koncetracje.");
                     return 0;
                   endif

                   ReleaseItem(CastObject);

                   if(IsMagicWand(CastObject))
                     var charges := Cint(GetObjProperty(CastObject, "charges"));
                     charges     := charges - 1;
                     SetObjProperty(CastObject,"charges",charges);

                     if(charges <= 0)
                       SendSysMessage(Caster,"Rozdzka rozladowala sie.",FONT_NORMAL,COLOR_RED);
                       DestroyItem(CastObject);
                     elseif(charges <= 3)
                       SendSysMessage(Caster,"Rozdzka powoli traci swa moc.",FONT_NORMAL,COLOR_RED+1);
                     endif
                   endif

  endcase

  if(SpellGlob.Flags & SPF_RETCASTON)
    return CastOn;
  elseif(CastOn)
    if(Steps)
      var x := SpellGlob.CasterLoc.x - Caster.x;
      var y := SpellGlob.CasterLoc.y - Caster.y;

      if(Caster.multi.isa(POLCLASS_BOAT))
        x := Abs(x - Caster.multi.x);
        y := Abs(y - Caster.multi.y);
      endif

      if(x > Steps or x < -Steps or y > Steps or y < -Steps or SpellGlob.CasterLoc.realm != Caster.realm)
        Fizzle(caster,"Stracil"+ggm(Caster,3)+" koncetracje.");
        return 0;
      endif
    endif

    SpellAction(Caster, CastOn);
    return 1;
  endif

  return 0;

endfunction

//------------------------------------------------------------------------

function GetMagicAnimId(Caster, SpellID)

  var AnimId := 0;

  case(Caster.graphic)
    CID_GRIZZLY:
    CID_WOLF:        AnimId := 0xA;

    default:         if(IsHuman(Caster))
                       AnimId := GetSpellValue(SpellID, "CastAnim");
                       if(!AnimId)
                         AnimId := ANIM_CAST_DIR;
                       endif
                     else
                       AnimId := ANIM_CAST_NPC;
                     endif
  endcase

  if(SpellGlob.CastType == CAST_NPC)
    var cfgElem := GetNpcConfig(Caster);

    if(CInt(cfgElem.CastAnim))
      AnimId  := CInt(cfgElem.CastAnim);
    endif
  endif

  return AnimId;

endfunction

//------------------------------------------------------------------------

function GetCasterSkillValue(Caster)

  var SpType := SpellGlob.Type;
  VerifyMagicSpellType(Caster, SpType, SpellGlob.Circle);

  var SkID   := GetMagicSkillID(SpType);
  var Val    := CInt(GetEffectiveSkillBaseValue(Caster, SkID) * 0.75);

  return Val;

endfunction

//------------------------------------------------------------------------

function MainCast(caster,SpellID)

  if(SpellGlob.CastType != CAST_NPC)
    if(!CanEquippedCast(caster))
      return 0;
    endif
  endif

  var SkillReq := GetSpellValue(SpellID,"SkillReq");
  if(!SkillReq)
    SkillReq := MAGE_SKILL_NEEDED;
  endif

  if(SkillReq)
    var SpType := SpellGlob.Type;
    VerifyMagicSpellType(Caster, SpType, SpellGlob.Circle);
    var SkID   := GetMagicSkillID(SpType);

    if(GetEffectiveSkill(Caster, SkID) < SkillReq)
      Fizzle(Caster,"Twa wiedza jest za mala na rzucenie tego czaru.");
      return;
    endif
  endif

  if(!(SpellGlob.Flags & SPF_NOREGS))
    if(!ConsReagents(Caster, SpellID))
      Fizzle(caster,"Brakuje Ci skladnikow.");
      SendMagicReagentsList(Caster, SpellID);
      return 0;
    endif
  endif

  return 1;

endfunction

//------------------------------------------------------------------------

function DoAnimation(Caster, SpellID)

  var MaxHp           := GetMaxHp(Caster);
  var Hp              := GetHp(Caster);
  var n               := 0;
  var CastDelay       := 1000 + (750 - GetCasterSkillValue(Caster)) - (GetTempModAmount(Caster,MOD_FASTER_CAST) * 250);

  case(SpellGlob.Type)
    SPTYPE_MAGE:
    SPTYPE_NECRO:
                     var AnimTimes := CInt(GetSpellValue(SpellID,"val"));

                     if(!CheckDisturb(caster, MaxHp, Hp))
                       Fizzle(caster,"Stracil"+ggm(Caster,3)+" koncentracje.");
                       return 0;
                     endif

                     for(n:=1;n<=AnimTimes;n:=n+1)
                       PlayAction(caster, GetMagicAnimId(Caster, SpellID));
                       sleepms(CastDelay);
                       if(!CheckDisturb(caster, MaxHp, Hp))
                         Fizzle(caster,"Stracil"+ggm(Caster,3)+" koncentracje.");
                         return 0;
                       endif
                     endfor

    SPTYPE_VAMPIRE:
    SPTYPE_DRUID:
                     var Animation := GetSpellValue(SpellID,"animation",ARRAY_ID);

                     foreach motion in Animation
                       if(!IsHuman(Caster))
                         motion := GetMagicAnimId(Caster, SpellID);
                       endif
                       PlayAction( Caster, CInt(motion));
                       sleepms(CastDelay);
                       if(!CheckDisturb(caster, MaxHp, Hp))
                         Fizzle(caster,"Stracil"+ggm(Caster,3)+" koncentracje.");
                         return 0;
                       endif
                     endforeach
  endcase

  return 1;

endfunction

//------------------------------------------------------------------------

function CheckSpellSkill(Caster, Diff)

  if(SpellGlob.Flags & SPF_NOSKILL)
    return 1;
  endif

  var SpType    := SpellGlob.Type;
  VerifyMagicSpellType(Caster, SpType, SpellGlob.Circle);

  var MindSkill := GetMindSkillValue(Caster, SpType);
  var MageSkId  := GetMagicSkillID(SpType);

  if(SpType != SpellGlob.Type)
    SpellGlob.Flags := SpellGlob.Flags | SPF_NOGAIN;
  endif

  if(MindSkill + 15 >= Diff or SpellGlob.CastType == CAST_NPC)
    if(CheckSkill(Caster, MageSkId, Diff, 0))
      return 1;
    endif
  endif

  Fizzle(caster,"Czar sie nie udal.");
  return 0;

endfunction

//------------------------------------------------------------------------

function IsItemCast(CastOn)

  if(CastOn.isa(POLCLASS_ITEM))
    var Cfg := GetItemConfig(CastOn);

    return Cfg.CanMagicTarget;
  endif

  return 0;

endfunction

//------------------------------------------------------------------------

function CheckItemCast(Caster, CastOn)
  
  if(CastOn.isa(POLCLASS_ITEM))
    var Cfg := GetItemConfig(CastOn);

    if(Cfg.CanMagicTarget)
      var Script := Cfg.MagicScript;
      if(!Cfg.MagicScript)
        LogError("Przedmiot niby mozna wycelowac z magii, ale nie ma skryptu MagicScript!! Item: "+hex(CastOn.objtype)+"/"+hex(CastOn.serial));
      else
        var Dmg := CalcSpellDamage(SpellGlob.Circle, Caster, CastOn, SpellGlob.Type);
        start_script(Script, array(Caster, CastOn, SpellGlob.SpellID, Dmg));
        return 1;
      endif
    endif

    return 0;
  endif

  return -1;

endfunction

//------------------------------------------------------------------------

function GetCastOn(caster,SpellID,type,noto)

  var NotoType;

  case(noto)
    TRGO_HELPFUL:  NotoType := TGTOPT_HELPFUL + TGTOPT_CHECK_LOS;
    TRGO_HARMFUL:  NotoType := TGTOPT_HARMFUL + TGTOPT_CHECK_LOS;
    TRGO_NOLOS:    NotoType := TGTOPT_NOCHECK_LOS;
    default:       NotoType := TGTOPT_CHECK_LOS;
  endcase

  var Sk1     := GetMagicSkillValue(Caster, SpellGlob.Type, SpellGlob.Circle);
  var Sk2     := GetMindSkillValue(Caster, SpellGlob.Type, SpellGlob.Circle);
  var CastOn  := 0;
  var tgParam := struct;

  tgParam.+Caster      := Caster;
  tgParam.+SpellID     := SpellID;
  if(SpellGlob.FastCast)
	tgParam.+CasterSteps := 35 + 5;
  else
	tgParam.+CasterSteps := CInt(Sk1 / 15) + CInt(Sk2 / 15) + 5;
  endif
  tgParam.+StartTime   := ReadGameClock();

  case(type)
    TRGT_NONE:       CastOn := 1;

    TRGT_OBJECT:     SendGood(Caster, "Wskaz obiekt.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(Caster,CastOn, tgParam.Tele))
                       return 0;
                     endif

                     if(!CastOn.isa(POLCLASS_UOBJECT))
                       Fizzle(Caster,"To nie obiekt.");
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     endif

    TRGT_DEAD:       SendGood(Caster, "Wskaz ducha.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(Caster,CastOn, tgParam.Tele))
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     elseif(!CastOn.isa(POLCLASS_MOBILE))
                       Fizzle(Caster,"To nie osoba.");
                       return 0;
                     elseif(CastOn.isa(POLCLASS_NPC))
                       if(!IsNpcElemental(CastOn, NPCELEM_UNDEAD))
                         Fizzle(Caster,"To stworzenie nie wyglada na nieumarle.");
                         return 0;
                       endif
                     elseif(!CastOn.dead)
                       Fizzle(Caster,"Ta osoba wyglada na w pelni zywa.");
                       return 0;
                     endif

    TRGT_MOB:
    TRGT_NPC:
    TRGT_PLAYER:     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     elseif(CastOn.dead or CastOn.mobile.dead)
                       Fizzle(caster,"Ta osoba nie zyje!");
                       return 0;
                     elseif(!CastOn.isa(POLCLASS_MOBILE))
                       Fizzle(caster,"To nie jest zyjace stworzenie.");
                       return 0;
                     elseif(Type == TRGT_NPC)
                       if(!CastOn.isa(POLCLASS_NPC))
                         Fizzle(caster,"To ani zwierzak ani potwor.");
                         return 0;
                       endif
                     elseif(Type == TRGT_PLAYER)
                       if(!IsPlayer(CastOn))
                         Fizzle(caster,"To nie jest osoba.");
                         return 0;
                       endif
                     endif

    TRGT_CORPSE:     SendGood(Caster, "Wskaz zwloki.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);

                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     elseif(Coordist(Caster.x, Caster.y, CastOn.x, CastOn.y) > 3)
                       Fizzle(caster,"Stoisz za daleko!");
                       return 0;
                     endif

    TRGT_RUNE:       SendGood(Caster, "Wskaz runo.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     endif

                     if(CastOn.objtype != UOBJ_RUNE or !CastOn.isa(POLCLASS_ITEM))
                       Fizzle(caster,"To nie runo!");
                       return 0;
                     endif

    TRGT_CBALL:      SendGood(Caster, "Wskaz krysztalowa kule.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     endif

                     if(CastOn.objtype != UOBJ_CRYSTAL_BALL or !CastOn.isa(POLCLASS_ITEM))
                       Fizzle(caster,"To nie krysztalowa kula!");
                       return 0;
                     endif

    TRGT_ITEM:       SendGood(Caster, "Wskaz przedmiot.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     endif

                     if(!CastOn.isa(POLCLASS_ITEM))
                       Fizzle(caster,"To nie przedmiot.");
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     endif

    TRGT_LOC:        CastOn := PcTargetLoc(caster, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     endif

                     if(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     endif

    TRGT_UNDEAD:     SendGood(Caster, "Wskaz umarlaka.");
                     CastOn := ObjTargetEx(Caster, NotoType, tgParam);
                     if(!CheckCastOn(caster,CastOn, tgParam.Tele))
                       return 0;
                     elseif(IsItemCast(CastOn))
                       if(!CheckItemCast(Caster, CastOn))
                         return 0;
                       endif
                     elseif(!CastOn.isa(POLCLASS_NPC))
                       Fizzle(caster,"To nie potwor!");
                       return 0;
                     elseif(!IsNpcElemental(CastOn, NPCELEM_UNDEAD))
                       Fizzle(caster,"To nie wyglada na umarle.");
                       return 0;
                     endif
  endcase

  if(GetSpellValue(SpellID, "NotInBuild"))
    if(IsInBuilding(CastOn.x, CastOn.y, CastOn.realm))
      Fizzle(Caster,"Nie uda Ci sie wyczarowac tego w budynku.");
      return 0;
    endif
  endif
  
  if(CastOn.isA(POLCLASS_NPC))
	if(CastOn.enabled("invul"))
		Fizzle(Caster, "Ten cel nie moze zostac uzyty.");
		return 0;
	endif
  endif

  NoLongerBusy(Caster);
  Caster.hidden := 0;

  return CastOn;

endfunction

//------------------------------------------------------------------------

function CheckCastOn(Caster, CastOn, Tele := 0)

  if(!Caster)
    return 0;
  endif

  if(Caster.paralyzed or Caster.frozen)
    Fizzle(Caster, "Zostal"+ggm(Caster,3)+" sparalizowan"+ggm(Caster,6)+"!");
    return 0;
  endif

  if(Caster.dead)
    Fizzle(Caster, "Jestes martw"+ggm(Caster,6)+"!");
    return 0;
  endif

  if(CastOn == error and CastOn.errortext)
    Fizzle(Caster,CastOn.errortext);
    return 0;
  endif

  if(!CastOn)
    Fizzle(caster,"Anulowano.");
    return 0;
  endif

  if(CastOn.item)
    CastOn := CastOn.item;
  elseif(CastOn.mobile)
    CastOn := CastOn.mobile;
  endif

  if(CastOn.isa(POLCLASS_MOBILE))
    if(CastOn.hidden or CastOn.concealed)
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif
  elseif(CastOn.isa(POLCLASS_ITEM))
    if(CastOn.invisible)
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif
  endif

  if(CastOn.isa(POLCLASS_MOBILE) or CastOn.isa(POLCLASS_ITEM))
    if(!CheckLineOfSight(Caster,CastOn))
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif
  endif

  if(!CastOn.isa(POLCLASS_ITEM))
    if(!CheckLosAt(Caster,CastOn.x,CastOn.y,CastOn.z))
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif

    if(DistZ(Caster,CastOn) >= 20)
      Fizzle(caster,"Cel jest zbyt wysoko!");
      return 0;
    endif
  endif

  if(CastOn.isa(POLCLASS_NPC))
    if(coordist(caster.x,caster.y,CastOn.x,CastOn.y) > 12 - GetTempModAmount(Caster,MOD_CASTSIGHT) )
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif
  elseif(!CastOn.container)
    if(coordist(caster.x,caster.y,CastOn.x,CastOn.y) > 15 - GetTempModAmount(Caster,MOD_CASTSIGHT) )
      Fizzle(caster,"Nie widzisz tego.");
      return 0;
    endif
  endif

  if(CastOn.isa(POLCLASS_ITEM) and CastOn.movable)
    if(!Accessible(Caster,CastOn))
      Fizzle(caster,"Przedmiot jest niedostepny.");
      return 0;
    endif

    if(!ReserveItem(CastOn))
      Fizzle(caster,"Nie mozesz tego uzyc.");
      return 0;
    endif
    ReleaseItem(CastOn);
  endif

  if(Tele)
    if(!IsPlayer(CastOn))
      Fizzle(Caster,"Stracil"+ggm(Caster,3)+" koncentracje.");
      return 0;
    endif
  endif

  if(CastOn)
    CheckUseSthWatch(Caster, CastOn);
  endif

  return 1;

endfunction

//------------------------------------------------------------------------

function PcTargetWaitFunc(byref Param)

  if(SpellGlob.CastType == CAST_NPC)
    return 0;
  endif

  if(Param.Caster.dead or !Param.Caster or Param.Caster.paralyzed or Param.Caster.frozen)
    return 0;
  endif

  var CasterLoc := struct;
  CasterLoc.+x := Param.Caster.x;
  CasterLoc.+y := Param.Caster.y;

  if(Param.Caster.multi.isa(POLCLASS_BOAT))
    CasterLoc.x := Abs(CasterLoc.x - Param.Caster.multi.x);
    CasterLoc.y := Abs(CasterLoc.y - Param.Caster.multi.y);
  endif
  
  
  if(!SpellGlob.FastCast)
	  if(CoorDist(SpellGlob.CasterLoc.x, SpellGlob.CasterLoc.y, CasterLoc.x, CasterLoc.y) > Param.CasterSteps or SpellGlob.CasterLoc.realm != Param.Caster.realm)
		if(!GetSpellValue(Param.SpellID, "NoTelePCast"))
		  var lMove := GetLastMove(Param.Caster);
		  if(lMove[1] == MOVETYPE_TELE and lMove[3] and lMove[4])
			if(CoorDist(lMove[3][1], lMove[3][2], SpellGlob.CasterLoc.x, SpellGlob.CasterLoc.y) <= Param.CasterSteps and SpellGlob.CasterLoc.realm == lMove[3][4])
			  SpellGlob.CasterLoc.x     := lMove[4][1];
			  SpellGlob.CasterLoc.y     := lMove[4][2];
			  SpellGlob.CasterLoc.realm := lMove[4][4];
			  Param.+Tele := 1;
			  return 1;
			endif
		  endif
		endif

		Fizzle(Param.Caster,"Stracil"+ggm(Param.Caster,3)+" koncentracje.");
		return 0;
	  endif
  endif

  if(Param.StartTime + 4 < ReadGameClock())
    if(!ConsumeManaAmount(Param.Caster, CInt(GetMaxMana(Param.Caster)*0.1)))
      Fizzle(Param.Caster,"Stracil"+ggm(Param.Caster,3)+" koncentracje.");
      return 0;
    endif
  endif

  if(SpellGlob.CasterMount != GetEquipmentByLayer(Param.Caster, LAYER_MOUNT))
    Fizzle(Param.Caster,"Stracil"+ggm(Param.Caster,3)+" koncentracje.");
    return 0;
  endif

  return 1;

endfunction

//------------------------------------------------------------------------

function GetDisturbChance(Caster)

  var ItemCfg   := ReadConfigFile(ITEMS_CFG);
  var SkDisturb := 1;

  if(ItemCfg[caster.weapon.objtype].MageDisturb)
    if(ItemCfg[caster.weapon.objtype].Attribute)
      SkDisturb := GetEffectiveSkill(Caster, FindSkillidByIdentifier(ItemCfg[caster.weapon.objtype].Attribute));
    endif
  endif

  return SkDisturb;

endfunction

//------------------------------------------------------------------------

function CheckDisturb(Caster, byref MaxHp, byref Hp)



  if(SpellGlob.FastCast)
	if(Caster.cmdlevel)
		SendSysMessage(Caster, "Nie patrzymy na Disturb");
		return 1;
	endif
	return 1;
  endif
  
  var CasterLoc := struct;
  CasterLoc.+x := Caster.x;
  CasterLoc.+y := Caster.y;

  if(Caster.multi.isa(POLCLASS_BOAT))
    CasterLoc.x := Abs(CasterLoc.x - Caster.multi.x);
    CasterLoc.y := Abs(CasterLoc.y - Caster.multi.y);
  endif

  if(SpellGlob.CasterLoc.x != CasterLoc.x or SpellGlob.CasterLoc.y != CasterLoc.y or SpellGlob.CasterLoc.realm != Caster.realm)
    return 0;
  endif

  if(Caster.dead)
    return 0;
  endif

  if(!MaxHp)
    MaxHp := GetMaxHp(Caster);
  endif

  if(MaxHp > GetMaxHp(Caster))
    Hp    := Hp - (MaxHp - GetMaxHp(Caster));
    MaxHp := GetMaxHp(Caster);
  endif

  if(Hp > GetHp(caster))
    if(RandomInt(100) > (GetDisturbChance(Caster) - CInt((Hp - GetHp(Caster)) * 2)))
      return 0;
    endif
    Hp := GetHp(Caster);
  endif

  if(Caster.paralyzed or Caster.frozen)
    return 0;
  endif

  if(SpellGlob.CasterMount != GetEquipmentByLayer(Caster, LAYER_MOUNT))
    return 0;
  endif

  return 1;

endfunction

//------------------------------------------------------------------------

function CanEquippedCast(caster)

  if(Caster.isa(POLCLASS_NPC) or Caster.cmdlevel)
    return 1;
  endif

  var CasterMagery  := GetMagicSkillValue(Caster, SpellGlob.Type, SpellGlob.Circle);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(CInt(ItemCfg[Caster.weapon.objtype].NeededMagCastSk))
    if(CInt(ItemCfg[Caster.weapon.objtype].NeededMagCastSk) > CasterMagery)
      Fizzle(caster,"Jestes jeszcze za slaby by moc czarowac z tym w reku!");
      return 0;
    endif
  endif

  if(!CInt(ItemCfg[Caster.weapon.objtype].CanEquippedCast) AND GetBmReqSkills(Caster) < 2400 )
    Fizzle(caster,"Nie mozesz czarowac z tym w reku!");
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------
function GetBmReqSkills(who)

  var SkSum := GetBaseSkillBaseValue(who, SKILLID_TACTICS) + GetBaseSkillBaseValue(who, SKILLID_ANATOMY);

  var OSks  := {
    GetBaseSkillBaseValue(who, SKILLID_MACEFIGHTING),
    GetBaseSkillBaseValue(who, SKILLID_WRESTLING),
    GetBaseSkillBaseValue(who, SKILLID_FENCING),
    GetBaseSkillBaseValue(who, SKILLID_SWORDSMANSHIP),
    GetBaseSkillBaseValue(who, SKILLID_AXESMANSHIP),
    0
  };

  var i;
  var maxFight := 0;
  for ( i := 1; i <= 5; i := i+1)
    if ( OSks[i] >= maxFight)
      maxFight := OSks[i]; 
    endif;
  endfor
  SkSum := SkSum + maxFight;

  return SkSum;

endfunction

//------------------------------------------------------------------------

function GetSpellCaster( SpellParms )

  var Parms           := GetSpellParms( SpellParms );
  var Caster          := Parms[1];

  return Caster;

endfunction

//------------------------------------------------------------------------

function GetSpellParms(Parms)
  var RetParms := {};

  if(Parms.size() > 1)
    RetParms := Parms;
  else
    RetParms[1] := Parms;
    RetParms[2] := CAST_PLAYER;
    RetParms[3] := 0;
    RetParms[4] := 0;
  endif

  return RetParms;

endfunction

//------------------------------------------------------------------------

function GainSpellPoints(Caster, CastOn, Type)

  if(!CanSpellGain(Caster, CastOn, Type))
    return 0;
  endif
  
    if(CastOn.enabled("invul") || CastOn.script["storeman"] || CastOn.script["banker"] || CastOn.script["merchant"] || CastOn.script["townperson"])
		PrintTextAbove(Caster, "*oszust*");
		return 0;
  endif

  if(Caster == CastOn and Type == MAGICGAIN_OFFENSIVE)
    return 0;
  endif

  var Points := GetCircleValue(SpellGlob.Circle,"Points");

  if(SpellGlob.CastType == CAST_SCROLL)
     Points  := GetCircleValue(Max(SpellGlob.Circle - 1, 1),"Points");
  endif

  case(Type)
    MAGICGAIN_OFFENSIVE:
    MAGICGAIN_DEFENSIVE: 
      Points *= MAGIC_GAIN_MOD;
    MAGICGAIN_NORMAL:     
      Points *= MAGIC_GAIN_NORMAL_MOD;
    MAGICGAIN_SUMMON:
     
      Points *= MAGIC_GAIN_SUMMON_MOD;
  endcase

  if(!Caster.isa(POLCLASS_NPC))
    if(!(SpellGlob.Flags & SPF_NOSKLOSS))
      ReduceMagicSkill(Caster, SpellGlob.Type);
    endif
  endif

  if(!(SpellGlob.Flags & SPF_NOGAIN))
  
    if(Type != MAGICGAIN_OFFENSIVE)
      var Diff   := GetCircleValue(SpellGlob.Circle,"Diff");
      var SkillId := GetMagicSkillID(SpellGlob.Type);
      Points := GetRestrictedPoints(Caster, SkillId, Max(0, Diff+20), Points);
    endif
	
	Points := CInt(Points * 1.8);

    return GainMagicPoints(Caster, SpellGlob.Type,  Points, SpellGlob.Circle);
  endif

  return 0;

endfunction

//------------------------------------------------------------------------

function CheckCityWallSpell(Caster, CastLoc)

  if(FindCity(CastLoc))
    //SeekInformTownGuard(Caster, "Gdzie z tymi czarami tutaj!!", GMR_WALL_CASTING);
    Fizzle(Caster, "Nie mozesz rzucic tego czaru w miescie.");
    return 0;
  else
    foreach Rune in ListObjectsInBox(CastLoc.x-8,CastLoc.y-8,-127,CastLoc.x+8,CastLoc.y+8,127,Caster.realm)
      if(Rune.objtype == UOBJ_TELE)
        Fizzle(Caster, "Sciana zapadnie sie tak blisko przejscia...");
        return 0;
      endif
    endforeach
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function IsDungeonSpellCasted(SpellID)

  if(GetSpellValue(SpellID, "SpellType") != SPTYPE_VAMPIRE)
    return 0;
  endif

  var DayCast := GetSpellValue(SpellID,"DayCast");

  if(!DayCast)
    return 1;
  endif

  DayCast := SplitWords(DayCast);

  if(DayCast[1] != "1")
    return 1;
  endif

  return 0;

endfunction 

//------------------------------------------------------------------------
