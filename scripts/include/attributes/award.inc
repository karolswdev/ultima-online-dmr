//--------------------------------------------------------------------------------

use util;

//--------------------------------------------------------------------------------

include "include/teach";
include "include/attributes/skills";
include "include/attributes/stats";
include "include/attributes/npcs";
include "include/player/basic";
include "include/skills/misc";
include "include/checks/locchecks";
include "include/clock";
include "include/std";
include "include/const/props";

//--------------------------------------------------------------------------------

const MAX_GAIN_BASE_VALUE          := 7;
const SKMASTER_GAIN_MOD            := 0.15; // 15% wiecej gdy juz raz bylismy GMem w danym skillu

const NO_STAT_AWARD                := 0x01;
const TEACH_AWARD                  := 0x02;
const GAIN_RESTRICT                := 0x04;
const STAT_GAIN_MODIFIED           := 0x08;
const STAT_GAIN_ENHANCED           := 0x10;
const STAT_ALWAYS_GAIN             := 0x20;
const GAIN_HALF_RESTRICT           := 0x40;

const AWARD_BONUS_ACTIVE           := 1;
const AWARD_BONUS_PERC             := 2.1;

const HAPPY_HOUR_GAIN_MOD          := 2.25;

//================================================================================
//--------------------------------------------------------------------------------
// Attribute Award
//--------------------------------------------------------------------------------
//================================================================================

function CalculateBonusStrengthPoints(who)

	if(GetBaseStrength(who) < 50 && GetBaseStrength(who) > 0)
		return 12;
	endif
	
	if(GetBaseStrength(who) < 60 && GetBaseStrength(who) > 50)
		return 10;
	endif
	
	if(GetBaseStrength(who) < 70 && GetBaseStrength(who) > 60)
		return 5;
	endif
	
	if(GetBaseStrength(who) < 80 && GetBaseStrength(who) > 70)
		return 4;
	endif
	
	if(GetBaseStrength(who) < 90 && GetBaseStrength(who) > 80)
		return 1;
	endif
	
	return 0;

endfunction

function CalculateBonusDexterityPoints(who)

	if(GetBaseDexterity(who) < 50 && GetBaseDexterity(who) > 0)
		return 12;
	endif
	
	if(GetBaseDexterity(who) < 60 && GetBaseDexterity(who) > 50)
		return 10;
	endif
	
	if(GetBaseDexterity(who) < 70 && GetBaseDexterity(who) > 60)
		return 5;
	endif
	
	if(GetBaseDexterity(who) < 80 && GetBaseDexterity(who) > 70)
		return 4;
	endif
	
	if(GetBaseDexterity(who) < 90 && GetBaseDexterity(who) > 80)
		return 1;
	endif
	
	return 0;

endfunction

function CalculateBonusIntelligencePoints(who)

	if(GetBaseIntelligence(who) < 50 && GetBaseIntelligence(who) > 0)
		return 12;
	endif
	
	if(GetBaseIntelligence(who) < 60 && GetBaseIntelligence(who) > 50)
		return 10;
	endif
	
	if(GetBaseIntelligence(who) < 70 && GetBaseIntelligence(who) > 60)
		return 9;
	endif
	
	if(GetBaseIntelligence(who) < 80 && GetBaseIntelligence(who) > 70)
		return 8;
	endif
	
	if(GetBaseIntelligence(who) < 90 && GetBaseIntelligence(who) > 80)
		return 4;
	endif
	
	return 0;

endfunction

function AwardPoints(who, SkId, SkPoints, GainFlags := 0, GainValue := -1)



  if(who.isa(POLCLASS_NPC))
    return AwardNpcPoints(who, SkId, SkPoints);
  endif

  if(IsBlockedSkillForClass(who, SkId))
    return 0;
  endif

  if(IsGainBlockTimed(who))
	SendSysMessage(who, "Niestety, ale nie mozesz sie teraz uczyc.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(WasEquipingItem(who, 10))
    return 0;
  endif

  SkPoints := CInt(SkPoints);
  
  if(IsSkillMastered(who, SkId))
    SkPoints += CInt(CDbl(SkPoints) * SKMASTER_GAIN_MOD);
  endif

  if(GetObjProperty(who, "#AFight"))
    if(GetBaseSkill(who, SkId) >= 60)
      return 0;
    endif
    SetBitOn(GainFlags, NO_STAT_AWARD);
  endif

  if(!who.isA(POLCLASS_MOBILE) or SkId == error or !SkPoints)
    return 0;
  endif

  var elem := GetSkillConfigElem(SkId);

  if(!elem)
    return elem;
  endif

  if(!IsBitOn(GainFlags, TEACH_AWARD))
    // uczy? to nie ma gaina dla siebie.
    if(IsLearning(who))
      return 0;
    endif
  else
    // podczas uczenia nie ma gainow statow w ogole
    SetBitOn(GainFlags, NO_STAT_AWARD);
  endif
  
  SkId  := Cint(SkId);

  if(IsBitOn(GainFlags, GAIN_RESTRICT) and GainValue >= 0)
    SkPoints := GetRestrictedPoints(who, SkId, GainValue, SkPoints);
    
    // blokada na staty, jak ktos robi zbyt latwy koks na swoj poziom (wtedy SkPoints bedzie 0, gdy diff-15 < skill)
    if(!SkPoints and GainValue >= 0 and GainValue <= 40)
      SetBitOn(GainFlags, NO_STAT_AWARD);
    endif
  elseif(IsBitOn(GainFlags, GAIN_HALF_RESTRICT) and GainValue >= 0)
    SkPoints := GetHalfRestrictedPoints(who, SkId, GainValue, SkPoints);
  endif

  var AttrId := GetAttributeIdBySkillId(SkId);

  if(SkPoints > 0)
    ModifyGainByFactor(who, SkPoints, AttrId);
  endif
  
  // Mulander: Faster gains are needed
  if(GetBaseSkillBaseValue(who, SkId) < 800)
       SkPoints := CInt(SkPoints * 2.5);
  elseif(GetBaseSkillBaseValue(who, SkId) < 900)
       SkPoints := CInt(SkPoints * 1.7);
  else
       SkPoints := CInt(SkPoints * 0.7);
  endif

  var winterTime := CInt(GetObjProperty(who, "WinterPrizeGain"));
  if(winterTime > 0)
    if(winterTime > ReadGameClock())
      SkPoints += CInt(SkPoints * 0.25);
    else
     EraseObjProperty(who, "WinterPrizeGain");
     SendSysMessage(who, "Juz nie nauczysz sie wiecej podczas Zimy!", FONT_NORMAL, COLOR_RED);
    endif
  endif

  var skillPotion := CInt(GetObjProperty(who, "SkillPotion"));
  if(skillPotion > 0)
    if(skillPotion > ReadGameClock())
	SkPoints += CInt(SkPoints * 0.5);
    else
	EraseObjProperty(who, "SkillPotion");
	SendSysMessage(who, "Mikstura nauki przestaje dzialac...", FONT_NORMAL, COLOR_RED);
    endif
  endif

  if(SkPoints)
    AwardRawAttributePoints(who, AttrId, SkPoints, SkId);
    if(IsBitOn(GainFlags, GAIN_RESTRICT))
      TeachStuds(who, SkId, SkPoints, GainValue, Elem);
    endif
  endif
  
  if(!IsBitOn(GainFlags, NO_STAT_AWARD))
  
    var adv := elem.StrAdv;
	
	// Faster gains are needed for stats
	if(GetBaseStrength(who) < 78)
	   if(adv[1] < 10)
		   adv[1] := CInt(adv[1]*8);
	    endif
	endif
	
		

    if(PrepareStatAdvance(adv, GainFlags, GainValue))
      if((RandomInt(1000) < (adv[1])) or IsBitOn(GainFlags, STAT_ALWAYS_GAIN))
        ModifyGainByFactor(who, adv[2], ATTRIBUTEID_STRENGTH);
        AwardRawAttributePoints(who, ATTRIBUTEID_STRENGTH, adv[2], SkId, GainFlags);
      endif
    endif

    adv := elem.DexAdv;
	
	// Faster gains are needed for stats
        if(GetBaseDexterity(who) < 78)
		if(adv[1] < 10)
			adv[1] := CInt(adv[1]*8);
		endif
	endif

    if(PrepareStatAdvance(adv, GainFlags, GainValue))
      if((RandomInt(1000) < (adv[1])) or IsBitOn(GainFlags, STAT_ALWAYS_GAIN))
        ModifyGainByFactor(who, adv[2], ATTRIBUTEID_DEXTERITY);
        AwardRawAttributePoints(who, ATTRIBUTEID_DEXTERITY, adv[2], SkId, GainFlags);
      endif
    endif

    adv := elem.IntAdv;
	
	// 	// Faster gains are needed for stats
	if(GetBaseIntelligence(who) < 78)
		if(adv[1] < 10)
			adv[1] := CInt(adv[1]*8);
		endif
	endif

    if(PrepareStatAdvance(adv, GainFlags, GainValue))
      if((RandomInt(1000) < (adv[1])) or IsBitOn(GainFlags, STAT_ALWAYS_GAIN))
        ModifyGainByFactor(who, adv[2], ATTRIBUTEID_INTELLIGENCE);
        AwardRawAttributePoints(who, ATTRIBUTEID_INTELLIGENCE, adv[2], SkId, GainFlags);
      endif
    endif
	
  endif
  
  return 1;

endfunction

//--------------------------------------------------------------------------------

function AwardDefaultPoints(who, skillid, Diff := error)

  var Points := GetDefaultPointsForDiff(who, skillid, Diff);

  return AwardPoints(who, skillid, Points);

endfunction

//--------------------------------------------------------------------------------

function ModAwardByBonus(Points)

  if(AWARD_BONUS_ACTIVE and Points)
    return max(1, CInt(Points* AWARD_BONUS_PERC));
  endif

  return Points;

endfunction

//--------------------------------------------------------------------------------

function IsGainBlockTimed(who)

  var Time := CInt(GetObjProperty(who, PROP_GAIN_BLOCK));

  if(Time and Time > ReadGameClock())
    return 1;
  endif 

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GainBlockTimed(who, Time)

  SetObjProperty(who, PROP_GAIN_BLOCK, ReadGameClock() + Time);

endfunction

//--------------------------------------------------------------------------------

function PrepareStatAdvance(byref adv, GainFlags, GainValue)

  if(!adv)
    return 0;
  endif

  adv := SplitWords(adv);

  if(adv.size() != 2)
    return 0;
  endif

  adv[1] := Cdbl(adv[1])*10;

  // Mulander: stat gain must have more chances
  adv[1] := Cdbl(adv[1]*3);

  adv[2] := RandomDiceRoll(adv[2]);

  if(IsBitOn(GainFlags, STAT_GAIN_MODIFIED))
    GainValue := GainValue * 0.01;
    adv[1] := adv[1] / 2;
    adv[2] := adv[2] / 2;
    adv[1] := CInt( adv[1] + (adv[1] * GainValue) );
    adv[2] := CInt( adv[2] + (adv[2] * GainValue) );
  elseif(IsBitOn(GainFlags, STAT_GAIN_ENHANCED))
    adv[1] := adv[1] * GainValue;
    adv[2] := adv[2] * GainValue;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function AwardRawAttributePoints(who, byref AttrId, rawpoints, SkId, GainFlags := 0)

  rawpoints := CInt(rawpoints);
  
  var strBonus, dexBonus, intBonus;
  
  strBonus := CalculateBonusStrengthPoints(who);
  dexBonus := CalculateBonusDexterityPoints(who);
  intBonus := CalculateBonusIntelligencePoints(who);

  if(who.isa(POLCLASS_NPC) or !who.isA(POLCLASS_MOBILE) or !rawpoints)
    return;
  endif

  if(IsSetAttrFlag(who, AttrId, ATTRF_BLOCKED))
    if(!who.cmdlevel and !IsStatAttribute(AttrId))
      var MaxVal := GetSkillBaseMaxValue(who, SkId);
      if(GetBaseSkillBaseValue(who, SkId) > MaxVal)
        SendAttrEvent(who, EVID_SK_SET, AttrId, MaxVal);
      endif
    endif

    return;
  endif

  if(AWARD_BONUS_ACTIVE and RawPoints)
    RawPoints := max(1, CInt(RawPoints*AWARD_BONUS_PERC));
  endif
  
  if(GetObjProperty(who, "OfflineGain"))
	var gain := CInt(GetObjProperty(who, "OfflineGain"));
	if(gain > ReadGameClock())
		RawPoints := RawPoints * 3;
	else
		EraseObjProperty(who, "OfflineGain");
		SendSysMessage(who, "Szybsza nauka znika...", FONT_NORMAL, COLOR_RED);
	endif
  endif

  if (RawPoints < 1)
    return;
  endif
 

  if(IsStatAttribute(AttrId))
	  // GOQSANE: slowing down.
	  if(AttrId == ATTRIBUTEID_STRENGTH)
		if(GetBaseStrength(who) > 68)
			rawpoints := CInt(rawpoints * 0.5);
		else
			rawpoints := CInt(rawpoints * 1.2);
		endif
	  endif

	  if(AttrId == ATTRIBUTEID_DEXTERITY)
		if(GetBaseDexterity(who) > 68)
			rawpoints := CInt(rawpoints * 0.5);
		else
			rawpoints := CInt(rawpoints * 1.2);
		endif
	  endif

	  if(AttrId == ATTRIBUTEID_INTELLIGENCE)
		if(GetBaseIntelligence(who) > 68)
			rawpoints := CInt(rawpoints * 0.5);
		else
			rawpoints := CInt(rawpoints * 1.2);
		endif
	  endif
	  
    var Cfg := ReadConfigFile(CLASSES_CFG);
    var cfgClass := Cfg[GetCharClass(who)];

    case(AttrId)
      ATTRIBUTEID_STRENGTH:      rawpoints := CInt(rawpoints * (CInt(cfgClass.StrGain) * 0.01)) + strBonus;
      ATTRIBUTEID_DEXTERITY:     rawpoints := CInt(rawpoints * (CInt(cfgClass.DexGain) * 0.01)) + dexBonus;
      ATTRIBUTEID_INTELLIGENCE:  rawpoints := CInt(rawpoints * (CInt(cfgClass.IntGain) * 0.01)) + intBonus;
    endcase

    SendAttrEvent(who,EVID_ST_AWARD,AttrId,RawPoints, IsBitOn(GainFlags, STAT_ALWAYS_GAIN));
  else
 
    if(GetGlobalProperty("#HH") and RawPoints)
      RawPoints := max(1, CInt(RawPoints*HAPPY_HOUR_GAIN_MOD));
    endif
	
	var desc := GetObjProperty(who, PROP_DESCRIPTION);
	var size := CInt(desc.size());
	if(size > 360)
		RawPoints := max(1, CInt(RawPoints*1.1));
	endif
	
	if(GetObjProperty(who, "SuperGain"))
		RawPoints := CInt(RawPoints * 99);
		var gainAmount := CInt(GetObjProperty(who, "SuperGain"));
		if(gainAmount == 1)
		    EraseObjProperty(who, "SuperGain");
		else
		    SetObjProperty(who, "SuperGain", gainAmount-1);
		endif
	endif
	

    SendAttrEvent(who,EVID_SK_AWARD,AttrId,RawPoints);
  endif
  
  if (RawPoints < 1)
    return;
  endif

  CheckGainAttributeWatcher(who, AttrId, rawpoints);

endfunction

//--------------------------------------------------------------------------------

function IsStatAttribute(AttrId)

  if(AttrId == ATTRIBUTEID_STRENGTH or AttrId == ATTRIBUTEID_DEXTERITY or AttrId == ATTRIBUTEID_INTELLIGENCE)
    return 1;
  else
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function SetSkillMastered(who, SkillID)

  if(IsPlayer(who))
    if(!IsSkillMastered(who, SkillID))
      var AttrId := GetAttributeIdBySkillId(SkillID);
      SetObjProperty(who, PROP_SKMASTER + AttrId, ReadGameClock());
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function IsSkillMastered(who, SkillID)

  if(IsPlayer(who))
    var AttrId := GetAttributeIdBySkillId(SkillID);
    if(CInt(GetObjProperty(who, PROP_SKMASTER + AttrId)) > 0)
      return 1;
    endif
  endif

  return 0;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Nutrition Skill & Stat Gain
//--------------------------------------------------------------------------------
//================================================================================

function ModifyGainByNutr(who, byref Gain, byref AttrId)

  var Perc := GetNutrValue(who, NVI_HUNGER);

  if(Perc >= NUTR_FULL_FACTOR)
    Perc := ((NUTR_MAX_VAL - NUTR_FULL_FACTOR) - (Perc - NUTR_FULL_FACTOR)) / 5;
  else
    Perc := Perc / 10;
  endif

  var aPerc := GetNutrValue(who, GetNviByAttrId(AttrId));

  if(aPerc >= NUTR_FULL_FACTOR)
    aPerc := ((NUTR_MAX_VAL - NUTR_FULL_FACTOR) - (aPerc - NUTR_FULL_FACTOR)) / 5;
  else
    aPerc := aPerc / 10;
  endif

  // 0% (86-100) do 17% (0-85)

  Perc := (Perc + aPerc);

  if(Perc > 20)
    Perc := 20;
  endif

  Gain := (Gain * 0.90) + (Gain * (Perc * 0.01));
  Gain := CInt(Gain);

endfunction

//funkcja to zmniejszania jelsi strefa
//-------------------------------------------------------------------------------
function ModifyGainByFactor(who, byref Gain, byref AttrId)
  
  /*
  __NONPVP__ DELETED
  if(IsInNonPvPArea(who.x,who.y,who.realm))
    Gain := CInt(Gain * 0.9);
  endif
  */

  ModifyGainByNutr(who, Gain, AttrId);

endfunction
//--------------------------------------------------------------------------------

function GetNviByAttrId(AttrId)

  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);

  case(AttrCfg[AttrId].NutrId)
    NVIID_PROTEIN:  return NVI_PROTEIN;
    NVIID_SUGAR:    return NVI_SUGAR;
    NVIID_VITAMIN:  return NVI_VITAMIN;
    NVIID_HUNGER:   
    default:        return NVI_HUNGER;
  endcase

endfunction

//--------------------------------------------------------------------------------

function IsMobGainable(mob, attacker := 0)
  
  if(!mob.isa(POLCLASS_NPC))
    return 1;
  endif

  if(!CanGainOnNpcWithCag(mob, attacker))
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------
