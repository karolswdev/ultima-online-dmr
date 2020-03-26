//-----------------------------------------------------------------------------

include "include/client/sounds";
include "include/client/fonts";
include "include/client/skills";
include "include/effects";
include "include/skills/misc";
include "include/regs";
include "include/logerror";
include "include/buffs";
include "include/mobile/npcs/properties";
include "include/attributes/npcs_cag";
include "include/npcs";

include ":magiceffects:effects";
include ":magic:common";
include "properties";

//=============================================================================
//-----------------------------------------------------------------------------
// Magic Attributes
//-----------------------------------------------------------------------------
//=============================================================================

const DMGTYPE_FIRE           := 1;
const DMGTYPE_COLD           := 2;
const DMGTYPE_ELECTRIC       := 3;
const DMGTYPE_MISC           := 4;
const DMGTYPE_POISON         := 5;

const SPELLELEM_FIRE         := "fire";
const SPELLELEM_COLD         := "cold";
const SPELLELEM_LIGHT        := "light";
const SPELLELEM_MISC         := "misc";
const SPELLELEM_POISON       := "poison";

const NPC_DMG_MULTIPLIER     := 2;
const NPC_HEAL_MULTIPLIER    := 1.9;

const MAX_NOMAGERY_CIRCLE    := 2;
const MAGE_SKILL_NEEDED      := 25;
const MAX_MOD_AMOUNT         := 36;

//-----------------------------------------------------------------------------
// Cast
//-----------------------------------------------------------------------------

const CAST_PLAYER            := 0;
const CAST_NPC               := 1;
const CAST_SCROLL            := 2;
const CAST_WAND              := 3;

//-----------------------------------------------------------------------------
// Spell Consts
//-----------------------------------------------------------------------------

const SPF_NOREGS             := 0x00000001;
const SPF_NOMANA             := 0x00000002;
const SPF_NOWORDS            := 0x00000004;
const SPF_NOSKILL            := 0x00000008;
const SPF_NODELAY            := 0x00000010;
const SPF_NOGAIN             := 0x00000020;
const SPF_NOSKLOSS           := 0x00000040;
const SPF_RETCASTON          := 0x00000080;
const SPF_USEMOREMANA        := 0x00000100;

const SPTYPE_NONE            := 0;
const SPTYPE_MAGE            := 1;
const SPTYPE_DRUID           := 2;
const SPTYPE_NECRO           := 3;
const SPTYPE_VAMPIRE         := 4;

//-----------------------------------------------------------------------------
// Targeting
//-----------------------------------------------------------------------------

const TRGT_NONE              := 0;
const TRGT_OBJECT            := 1;
const TRGT_MOB               := 2;
const TRGT_PLAYER            := 3;
const TRGT_NPC               := 4;
const TRGT_DEAD              := 5;
const TRGT_UNDEAD            := 6;
const TRGT_LOC               := 7;
const TRGT_ITEM              := 8;
const TRGT_CORPSE            := 9;
const TRGT_RUNE              := 10;
const TRGT_CBALL             := 11;

const TRGO_NONE              := 0;
const TRGO_HELPFUL           := 1;
const TRGO_HARMFUL           := 2;
const TRGO_NOLOS             := 3;

//------------------------------------------------------------------------
// Field Types
//------------------------------------------------------------------------

const MFIELDT_FIRE           := 1;
const MFIELDT_ENERGY         := 2;
const MFIELDT_POISON         := 3;
const MFIELDT_PARA           := 4;
const MFIELDT_STONE          := 5;
const MFIELDT_FROST          := 6;

//------------------------------------------------------------------------

function DoMagicDamage(CastOn, Caster, Dmg, Circle, DmgType, SpellType := SPTYPE_NONE, Flags := 0)

  if(!CastOn.isa(POLCLASS_MOBILE))
    return 0;
  endif

  var DmgParms := {};
  DmgParms[1]  := Caster;
  DmgParms[2]  := CastOn;
  DmgParms[3]  := Dmg;
  DmgParms[4]  := DmgType;
  DmgParms[5]  := Circle;
  DmgParms[6]  := SpellType;
  DmgParms[7]  := Flags;

  if(start_script(":magiceffects:domagicdmg",DmgParms))
    if(IsImmuneToAttack(CastOn, DmgType))
      return -1;
    else
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsImmuneToAttack(CastOn, DmgType)

  if(CastOn.isa(POLCLASS_NPC))
    var Elems   := GetNpcElementals(CastOn);

    case(DmgType)
      DMGTYPE_FIRE:  
        if(NPCELEM_FIRE in Elems)
          return 1;
        endif
                        
      DMGTYPE_COLD:     
        if(NPCELEM_COLD in Elems)
          return 1;
        endif

      DMGTYPE_ELECTRIC:
        if(NPCELEM_LIGHT in Elems)
          return 1;
        endif

      DMGTYPE_POISON:
        if(NPCELEM_POISON in Elems)
          return 1;
        endif

    endcase
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function Reflected(CastOn, Caster)


  if(IsReflected(CastOn) == 1)
    PlaySoundEffect(CastOn, SFX_SPELL_MAGIC_REFLECTION);
    PlayObjectCenteredEffect(CastOn, FX_CURSE_EFFECT, 0x10, 0x10);
    SendSysMessage(CastOn, "Odbil"+ggm(CastOn,3)+" czar!",FONT_NORMAL,COLOR_BLUE);
    EraseObjProperty(CastOn, "mr");
    SendDelBuff(CastOn, BFICON_MAGIC_REFLECT);

    if(IsPlayer(Caster))
      if(CanGetCriminal(Caster, CastOn))
        SetCriminal(Caster, 1, "Reflected", "Rzucil czar w '"+GetRealName(Caster)+"', ktory go odbil.");
      endif
    endif

    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsReflected(CastOn)

  if(CastOn.enabled("invul"))
	return 1;
  endif

  if(CastOn.isa(POLCLASS_MOBILE))
    return Cint(GetObjProperty(CastOn,"mr"));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetReflect(CastOn, mr)

  if(mr > 0)
    SetObjProperty(CastOn,"mr", mr);
    SendAddBuff(CastOn, BFICON_MAGIC_REFLECT, 0, 1075817, CLILOC_EMPTY, 0);
  else
    EraseObjProperty(CastOn,"mr");
    SendDelBuff(CastOn, BFICON_MAGIC_REFLECT);
  endif

endfunction

//-----------------------------------------------------------------------------

function Dispersed(Caster, CastOn)

  if(IsPlayer(Caster) and IsPlayer(CastOn) and IsDispersed(CastOn))
    PlaySoundEffect(CastOn, SFX_SPELL_MAGIC_REFLECTION);
    PlayPcEffect(CastOn, FX_CURSE_EFFECT, 10, 0x10, 0x10, 3);
    SendSysMessage(CastOn, "Zniwelowal"+ggm(CastOn,3)+" czar!",FONT_NORMAL,COLOR_BLUE);
    SendSysMessage(Caster, GetName(CastOn)+" zniwelowal"+ggm(CastOn,6)+" twoj czar!",FONT_NORMAL,COLOR_RED_LIGHT);
    EraseObjProperty(CastOn, "SpellDisperse");
    SendDelBuff(CastOn, BFICON_MAGIC_REFLECT);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsDispersed(CastOn)

  if(CastOn.isa(POLCLASS_MOBILE))
    return Cint(GetObjProperty(CastOn, "SpellDisperse"));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetDisperse(CastOn, lvl)

  if(lvl > 0)
    SetObjProperty(CastOn,"SpellDisperse", lvl);
    SendAddBuff(CastOn, BFICON_MAGIC_REFLECT, 0, 1075817, CLILOC_EMPTY, 0);
  else
    EraseObjProperty(CastOn,"SpellDisperse");
    SendDelBuff(CastOn, BFICON_MAGIC_REFLECT);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMagicSkillID(SpellType)

  case(SpellType)
    SPTYPE_MAGE:    return SKILLID_MAGERY;
    SPTYPE_NECRO:   return SKILLID_NECRO;
    SPTYPE_VAMPIRE: return SKILLID_VAMPIRE;
    SPTYPE_DRUID:   return SKILLID_DRUID;
    default:        return -1;
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetMindSkillID(SpellType)

  case(SpellType)
    SPTYPE_MAGE:    return SKILLID_EVALINT;
    SPTYPE_NECRO:   return SKILLID_SPIRITSPEAK;
    SPTYPE_VAMPIRE: return SKILLID_SPIRITSPEAK;
    SPTYPE_DRUID:   return SKILLID_ANIMALLORE;
    default:        return -1;
  endcase

endfunction

//-----------------------------------------------------------------------------

function VerifyMagicSpellType(Caster, byref SpellType, Circ)

  if(SpellType == SPTYPE_MAGE and Circ > 0 and Circ <= MAX_NOMAGERY_CIRCLE)
    if(GetBaseSkill(Caster, SKILLID_MAGERY) == 0)
      if(GetBaseSkill(Caster, SKILLID_DRUID) > 0)
        SpellType := SPTYPE_DRUID;
      elseif(GetBaseSkill(Caster, SKILLID_NECRO) > 0)
        SpellType := SPTYPE_NECRO;
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function GetMagicSkillValue(who, SpellType, Circ := 0)

  VerifyMagicSpellType(who, SpellType, Circ);

  case(SpellType)
    SPTYPE_MAGE:    return GetEffectiveSkill(who, SKILLID_MAGERY);
    SPTYPE_NECRO:   return GetEffectiveSkill(who, SKILLID_NECRO);
    SPTYPE_VAMPIRE: return GetEffectiveSkill(who, SKILLID_VAMPIRE);
    SPTYPE_DRUID:   return GetEffectiveSkill(who, SKILLID_DRUID);
    default:        return 0;
  endcase

endfunction

//-----------------------------------------------------------------------------

function GetMindSkillValue(who,SpellType, Circ := 0)

  VerifyMagicSpellType(who, SpellType, Circ);

  case(SpellType)
    SPTYPE_MAGE:    return GetEffectiveSkill(who, SKILLID_EVALINT);
    SPTYPE_NECRO:   return GetEffectiveSkill(who, SKILLID_SPIRITSPEAK);
    SPTYPE_VAMPIRE: return GetEffectiveSkill(who, SKILLID_SPIRITSPEAK);
    SPTYPE_DRUID:   return GetEffectiveSkill(who, SKILLID_ANIMALLORE);
    default:        return 0;
  endcase

endfunction

//------------------------------------------------------------------------

function ReduceMagicSkill(Caster, SpellType)

  case(SpellType)
    SPTYPE_NECRO:     ReduceSkill(Caster, SKILLID_DRUID, 0.99);
                      ReduceSkill(Caster, SKILLID_KENSAI, 0.99);
                      ReduceSkill(Caster, SKILLID_BARBARIAN, 0.99);
                      ReduceSkill(Caster, SKILLID_WEARWOLF, 0.99);
                      
    SPTYPE_DRUID:     ReduceSkill(Caster, SKILLID_NECRO, 0.99);
                      ReduceSkill(Caster, SKILLID_KENSAI, 0.99);
                      ReduceSkill(Caster, SKILLID_BARBARIAN, 0.99);
                      ReduceSkill(Caster, SKILLID_VAMPIRE, 0.99);
                      
    SPTYPE_VAMPIRE:   ReduceSkill(Caster, SKILLID_DRUID, 0.99);
                      ReduceSkill(Caster, SKILLID_TAMING, 0.99);
                      ReduceSkill(Caster, SKILLID_KENSAI, 0.99);
                      ReduceSkill(Caster, SKILLID_BARBARIAN, 0.99);
                      ReduceSkill(Caster, SKILLID_WEARWOLF, 0.99);
  endcase

endfunction

//-----------------------------------------------------------------------------

function CalcSpellDamage(Circle, Caster, CastOn, SpellType := SPTYPE_NONE, NoFieldReduce := 0)

  if(CastOn.dead)
    return 0;
  endif

  var Sk1;
  var Sk2;

  if(SpellType)
    Sk1 := GetMindSkillValue(Caster, SpellType, Circle);
    Sk2 := GetMagicSkillValue(Caster, SpellType, Circle);
  else
    Sk1 := GetEffectiveSkill(Caster, SKILLID_EVALINT);
    Sk2 := GetEffectiveSkill(Caster, SKILLID_MAGERY);
  endif

  var ArtMod     := GetTempModAmount(Caster,MOD_SPELL_DMG);
  var RetDmg     := CInt((Circle*((Sk1) + (Sk2/2)))*4) + RandomInt(Circle*50);
  RetDmg         := CInt(RetDmg / 100);

  if(IsPlayer(Caster))
    RetDmg := (RetDmg * 0.8) + CInt((RetDmg/2) * (CInt((GetBaseIntelligence(Caster)*2) * (Circle * 0.09)) * 0.016));
  endif

  var Tact := GetBaseSkill(Caster,SKILLID_TACTICS);
  var Anat := GetBaseSkill(Caster,SKILLID_ANATOMY);

  if(Tact)
    RetDmg := RetDmg - CInt((CInt(Tact / 7) * 0.01) * RetDmg);
  endif

  if(Anat)
    RetDmg := RetDmg - CInt((CInt(Anat / 7) * 0.01) * RetDmg);
  endif

    RetDmg := RedPvPDamage(Caster,CastOn,RetDmg, MAGIC_DMG);

  if(ArtMod)
    RetDmg := CInt(RetDmg + (RetDmg * (ArtMod * 0.01)));
  endif

  RetDmg := CInt(RetDmg);
  ModifyGainByNutr(Caster, RetDmg, ATTRIBUTEID_INTELLIGENCE);

  if(RetDmg > ((Circle * 10) + Circle))
    RetDmg := (Circle * 10) + Circle;
  endif

  if(GetObjProperty(Caster, "#HighSpellDamage"))
    RetDmg := CInt(RetDmg * 1.5);
  endif

  return CheckForceField(CastOn,RetDmg,NoFieldReduce);

endfunction

//-----------------------------------------------------------------------------

const NO_RESIST_GAIN         := 0x00000001;
const TIME_RESIST_GAIN       := 0x00000002;
const NO_RESIST              := 0x00000080;

//-----------------------------------------------------------------------------
// Specjalna funkcja, ktora zachowuje dzialanie funkcji Resisted, przemianowanej na ResistedExtended 
//-----------------------------------------------------------------------------

function Resisted(CastOn, Caster, Amt, Circle := 1, SpellType := SPTYPE_NONE, Flags := 0)

  var ret := ResistedExtended(CastOn, Caster, Amt, Circle, SpellType, Flags);

  return ret.value;

endfunction

//-----------------------------------------------------------------------------
// Zmiana zwracanej wartosci funkcji Resisted oraz jej nazwa (nowa funkcja zwraca wartosc oraz czy czar zostal zresowany).
//-----------------------------------------------------------------------------

function ResistedExtended(CastOn, Caster, Amt, Circle := 1, SpellType := SPTYPE_NONE, Flags := 0)

  var res :=  struct{"value":=0,"resisted":=0};
   
  if(CastOn.dead or (Flags & NO_RESIST))
    res.value := Amt;
    return res;
  endif

  var Magery := 50;
  var MindSk := 50;

  if(Caster.isa(POLCLASS_MOBILE) and SpellType)
    MindSk := GetMindSkillValue(Caster, SpellType, Circle);
    Magery := GetMagicSkillValue(Caster, SpellType, Circle);
  endif

  var Resist := CInt(GetEffectiveSkill(CastOn,  SKILLID_MAGICRESISTANCE));

  if(IsPlayer(Caster) and IsPlayer(CastOn) and Caster != CastOn)
    Resist := CInt(Resist * 0.80);
  endif

  var chk1 := CInt(Resist / 5);
  var chk2 := Resist - (((Magery - 20) / 5) + (Circle * 5));

  if(chk2 < 0)
    chk2 := 0;
  endif

  var diff := 0;
  if(chk1 > chk2)
    diff := chk1;
  else
    diff := chk2;
    if(IsPlayer(Caster))
      chk2 := chk2 - CInt(GetBaseIntelligence(Caster) / 10);
    endif
  endif

  var points := 5 + GetRestrictedPoints(CastOn, SKILLID_MAGICRESISTANCE, CInt(GetCircleValue(Circle,"Diff")), GetCircleValue(Circle,"ResistPts"));
  if(CastOn.dead)
    return res;
  endif

  if(Caster.isa(POLCLASS_NPC) and IsPlayer(CastOn))
    Points := Points * 2;
  endif

  if((RandomInt(99) + 1) <= diff)
    SendSysMessage(CastOn, "Czujesz swoja odpornosc na magie!",FONT_NORMAL,COLOR_GREEN);
    res.resisted := 1;
    PlaySoundEffect(CastOn, SFX_SPELL_WEAKEN);
    PlayObjectCenteredEffect(CastOn, FX_CURSE_EFFECT, 5,5);
    if(!(Flags & NO_RESIST_GAIN))
      if(Flags & TIME_RESIST_GAIN)
        if(GetObjProperty(CastOn,"#ResistGainWall") < ReadGameClock())
          AwardPoints(CastOn, SKILLID_MAGICRESISTANCE, Points*2);
          SetObjProperty(CastOn,"#ResistGainWall",ReadGameClock() + 5);
        endif
      elseif(IsMobGainable(Caster, CastOn) and !GetObjProperty(CastOn, PROP_NO_FIGHT))
        AwardPoints(CastOn, SKILLID_MAGICRESISTANCE, Points*3);
        CagNpcDamaged(Caster, Max(CInt(Points / 2), 3), CastOn);
      endif
    endif
    amt := (amt * 0.5);
    if(amt < 1)
      amt := 1;
    endif
    var modamt := 1;
    if(Resist >= MindSk)
      modamt := (1 + ((MindSk - Resist) / 200.0));
    elseif(MindSk > Resist)
      modamt := (1 + ((MindSk - Resist) / 500.0));
    endif
    amt := (amt * modamt);
  endif

  res.value := CInt(amt);

  return res;

endfunction

//-----------------------------------------------------------------------------

function Fizzle(Caster, Msg := "")

  PlaySoundEffect (Caster, SFX_SPELL_FAIL);
  PlayObjectCenteredEffect (Caster, FX_SPELL_FAIL, 0x0a, 0x1e);

  if(Msg and !Caster.isa(POLCLASS_NPC))
    SendSysMessage(Caster,msg,FONT_NORMAL,COLOR_RED);
  endif

  Caster.hidden := 0;

endfunction

//-----------------------------------------------------------------------------

function GetMageSkId(Caster)

  if(GetBaseSkill(Caster, SKILLID_MAGERY) > 0)
    return SKILLID_MAGERY;
  elseif(GetBaseSkill(Caster, SKILLID_DRUID) > 0)
    return SKILLID_DRUID;
  elseif(GetBaseSkill(Caster, SKILLID_NECRO) > 0)
    return SKILLID_NECRO;
  endif

  return SKILLID_MAGERY;

endfunction

//--------------------------------------------------------------------------------

function GetMindSkId(Caster)

  if(GetBaseSkill(Caster, SKILLID_MAGERY) > 0)
    return SKILLID_EVALINT;
  elseif(GetBaseSkill(Caster, SKILLID_DRUID) > 0)
    return SKILLID_ANIMALLORE;
  elseif(GetBaseSkill(Caster, SKILLID_NECRO) > 0)
    return SKILLID_SPIRITSPEAK;
  endif

  return SKILLID_EVALINT;

endfunction

//--------------------------------------------------------------------------------

function CalcModAmount(Caster, Circ := 0)

  var CasterSk, CasterEi;

  if(Circ > 0 and Circ <= MAX_NOMAGERY_CIRCLE)
    CasterSk := GetEffectiveSkill(Caster, GetMageSkId(Caster));
    CasterEi := GetEffectiveSkill(Caster, GetMindSkId(Caster));
  else
    CasterSk := GetEffectiveSkill(Caster, SKILLID_MAGERY);
    CasterEi := GetEffectiveSkill(Caster, SKILLID_EVALINT);
  endif
  
  var additionalMod := RandomInt(CInt(GetEffectiveSkill(Caster, SKILLID_INSCRIPTION)/23));
  

  var ModAmount := CInt((CasterSk/10) + (CasterEi/10)) + additionalMod;

  if(IsPlayer(Caster) and CasterSk >= 100 and CasterEi >= 100)
	
    //ModAmount := ToMax(ModAmount + BonusInt(caster),MAX_MOD_AMOUNT) + additionalMod;

    ModAmount := ToMax(ModAmount + RandomInt(6) + BonusInt(caster),MAX_MOD_AMOUNT) + additionalMod;
  else
    ModAmount := ToMax(ModAmount + RandomInt(3) + BonusInt(caster),MAX_MOD_AMOUNT) + additionalMod;
  endif
  
  return ModAmount;

endfunction

//-----------------------------------------------------------------------------

function BonusInt(caster)

  var int := GetBaseIntelligenceBaseValue(caster);
  
  if(int > 999)
    return 6;
  elseif(int > 899)
    return 5;
  elseif(int > 799)
    return 4;
  elseif(int > 699)
    return 3;
  elseif(int > 599)
    return 2;
  elseif(int > 499)
    return 1;
  endif
  
  return 0;

endfunction
//-----------------------------------------------------------------------------

function CalcModDuration(caster, Circ := 0)

  var CasterSk, CasterEi;

  if(Circ > 0 and Circ <= MAX_NOMAGERY_CIRCLE)
    CasterSk := GetEffectiveSkill(Caster, GetMageSkId(Caster));
    CasterEi := GetEffectiveSkill(Caster, GetMindSkId(Caster));
  else
    CasterSk := GetEffectiveSkill(Caster, SKILLID_MAGERY);
    CasterEi := GetEffectiveSkill(Caster, SKILLID_EVALINT);
  endif

  return CInt( ((CasterSk/2) + (CasterEi)) * 6);

endfunction

//-----------------------------------------------------------------------------

function CalcDecModDuration(Caster, Circ := 0)

  var Dur := CInt(CalcModDuration(Caster, Circ) / 10);

  if(Dur < 15)
    Dur := 15;
  endif

  return Dur;

endfunction


//-----------------------------------------------------------------------------

function GetSpellDur(Caster, Circ := 0)

  var CasterSk, CasterEi;
  var CasterMed := GetEffectiveSkill(Caster, SKILLID_MEDITATION);

  if(Circ > 0 and Circ <= MAX_NOMAGERY_CIRCLE)
    CasterSk := GetEffectiveSkill(Caster, GetMageSkId(Caster));
    CasterEi := GetEffectiveSkill(Caster, GetMindSkId(Caster));
  else
    CasterSk := GetEffectiveSkill(Caster, SKILLID_MAGERY);
    CasterEi := GetEffectiveSkill(Caster, SKILLID_EVALINT);
  endif

  return CInt((CasterMed/4 + CasterEi/2 + CasterSk/4)*6);

endfunction

//-----------------------------------------------------------------------------

function GetNecroSpellDur(caster)

  var CasterNecroSkill   := GetEffectiveSkill(caster,SKILLID_NECRO);
  var CasterSpSkill      := GetEffectiveSkill(caster,SKILLID_SPIRITSPEAK);
  var CasterEiSkill      := GetEffectiveSkill(caster,SKILLID_EVALINT);
  var CasterMedSkill     := GetEffectiveSkill(caster,SKILLID_MEDITATION);

  return CInt((CasterMedSkill/4 + CasterNecroSkill/4 + CasterEiSkill/4 + CasterSpSkill/4)*6);

endfunction

//-----------------------------------------------------------------------------

function GetDruidSpellDur(caster)

  var CasterAnimalLore    := GetEffectiveSkill(caster,SKILLID_ANIMALLORE);
  var CasterDruid         := GetEffectiveSkill(caster,SKILLID_DRUID);
  var CasterTaming        := GetEffectiveSkill(caster,SKILLID_TAMING);
  var CasterMed           := GetEffectiveSkill(caster,SKILLID_MEDITATION);

  return CInt((CasterAnimalLore/4 + CasterDruid/4 + CasterTaming/4 + CasterMed/4)*6);

endfunction

//-----------------------------------------------------------------------------

function GetVampireSpellDur(caster)

  var CasterVamp          := GetEffectiveSkill(caster,SKILLID_VAMPIRE);
  var CasterSp            := GetEffectiveSkill(caster,SKILLID_SPIRITSPEAK);
  var CasterEvalint       := GetEffectiveSkill(caster,SKILLID_EVALINT);
  var CasterMed           := GetEffectiveSkill(caster,SKILLID_MEDITATION);

  return CInt((CasterSp/4 + CasterVamp/4 + CasterEvalInt/4 + CasterMed/4)*6);

endfunction

//-----------------------------------------------------------------------------

function StartSpell(Caster, SpId, CastType := 0, CastObject := 0, Flags := 0)

  var MParms := {};
  MParms[1]  := Caster;
  MParms[2]  := CastType;
  MParms[3]  := CastObject;
  MParms[4]  := Flags;

  if(!start_script(GetSpellValue(SpId,"Script",STRING_ID), MParms))
    LogError("StartSpell: Nie udalo sie uruchomic skryptu czaru "+SpId+" ["+GetSpellValue(SpId,"Script",STRING_ID)+"]");
  endif

endfunction

//------------------------------------------------------------------------

function ConsReagents(Caster,SpellID)

  var Reagents := GetSpellValue(SpellID,"SReagent",ARRAY_ID);
  return EatReagents(Caster,Reagents);

endfunction

//-----------------------------------------------------------------------------

function SendMagicReagentsList(Caster, SpellID)

  var Reagents := GetSpellValue(SpellID,"SReagent",ARRAY_ID);
  SendReagentsList(Caster, Reagents);

endfunction

//-----------------------------------------------------------------------------

function GetSpellIdsSortedByDiff(byref IdsArr, IdSet, IdEnd)

  var SpCfg  := ReadConfigFile(SPELLS_CFG);
  var Ids    := dictionary;
  var i;

  for(i:=IdSet;i<=IdEnd;i:=i+1)
    if(SpCfg[i])
      Ids[ZeroFill(SpCfg[i].Circle, 2)+"_"+i] := i;
    endif
  endfor

  IdsArr := array;

  foreach Id in Ids
    IdsArr.append(Id);
  endforeach

endfunction

//-----------------------------------------------------------------------------
