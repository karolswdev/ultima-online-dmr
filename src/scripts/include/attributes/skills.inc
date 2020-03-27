//--------------------------------------------------------------------------------

include "include/const/misc";
include "include/client/skills";
include "include/const/eventid";
include "include/misc";
include "include/str";
include "include/attributes/max_skills";

include ":character:common";

//--------------------------------------------------------------------------------

const DEFSK_ALWAYS_GAIN_UNTIL      := 100;
const DEFSK_CHECK_MAX_CHANCE       := 1000;
const DEFSK_CHECK_MIN_CHANCE       := 0;

const DEF_SK_MAX_VALUE             := 1000;
const MAX_RUN_SPEED                := 250;
const MAX_SK_MAX_VALUE             := 1150;

const SK_CRAFT_SKILLCAP_ADD        := 1000;

//================================================================================
//--------------------------------------------------------------------------------
// Configs
//--------------------------------------------------------------------------------
//================================================================================

function GetDefaultPointsValue(skillid)

  var elem := GetSkillConfigElem(skillid);
  if(!elem)
    return 0;
  else
    return Cint(elem.DefaultPoints);
  endif

endfunction

//--------------------------------------------------------------------------------

function GetDefaultPointsForDiff(who, skillid, Diff)

  if(Diff != error)
    return GetRestrictedPoints(who, skillid, Diff, GetDefaultPointsValue(skillid));
  else
    return GetDefaultPointsValue(skillid);
  endif

endfunction

//--------------------------------------------------------------------------------

function GetSkillConfigElemByName(AttrId)

  var SkId := GetSkillIdByAttributeId(AttrId);

  return GetSkillConfigElem(SkId);

endfunction

//--------------------------------------------------------------------------------

function GetSkillConfigElem(SkId)

  SkId := CInt(SkId);
  var UoSkCfg := ReadConfigFile(UOSKILLS_CFG);
  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);

  if(!AttrCfg)
    return AttrCfg;
  elseif(!UoSkCfg)
    return UoSkCfg;
  else
    return FindConfigElem(AttrCfg, UoSkCfg[SkId].Attribute);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetSkillDelay(SkID)

  var SkElem := GetSkillConfigElem(SkID);
  return Cint(SkElem.delay);
  
endfunction

//--------------------------------------------------------------------------------

function FindSkIDByShortName(sName)

  var i,Elem;
  var MaxSkId := GetMaxSkillId();
  sName := lower(sName);

  for(i:=0;i<=MaxSkId;i:=i+1)
    Elem := GetSkillConfigElem(i);
    if(lower(Elem.ShortName) == sName)
      return i;
    endif
  endfor

  return error;

endfunction

//--------------------------------------------------------------------------------

function GetShortSkillName(SkillId)

 var Elem := GetSkillConfigElem(SkillId);
 return Elem.ShortName;

endfunction

//--------------------------------------------------------------------------------

function GetMaxSkillId()

  var SkCfg := ReadConfigFile(UOSKILLS_CFG);
  return GetConfigMaxIntKey(SkCfg);

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Skills
//--------------------------------------------------------------------------------
//================================================================================

function GetEffectiveSkill(who, skillid)

  return GetAttribute(who, GetAttributeIdBySkillId(skillid));

endfunction

//--------------------------------------------------------------------------------

function GetEffectiveSkillBaseValue(who, skillid)

  var AttrId    := GetAttributeIdBySkillId(skillid);
  var BaseVal   := GetAttributeBaseValue(who, AttrId);
  var TempMod   := GetAttributeTemporaryMod(who, AttrId);

  return ToMin(BaseVal + TempMod, 0);

endfunction

//--------------------------------------------------------------------------------

function GetBaseSkill(who, skillid)

  return Cint(GetBaseSkillBaseValue(who, skillid) / 10);

endfunction

//--------------------------------------------------------------------------------

function GetOverSkill(who, skillid)

  return MinMax(GetEffectiveSkill(who, skillid) - 100, 0, 20);

endfunction

//--------------------------------------------------------------------------------

function GetBaseSkillBaseValue(who, skillid)

  return GetAttributeBaseValue(who, GetAttributeIdBySkillId(skillid));

endfunction

//--------------------------------------------------------------------------------

function GetSkillTemporaryMod(who, skillid)

  return Cint(GetSkillTemporaryModBaseValue(who, skillid)/ 10);

endfunction

//--------------------------------------------------------------------------------

function GetSkillTemporaryModBaseValue(who, skillid)

  return GetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid));

endfunction

//--------------------------------------------------------------------------------

function ModifySkillTemporaryMod(who, skillid, moda)

  return ModifySkillTemporaryModBaseValue(who, skillid, Cint(moda)*10);

endfunction

//--------------------------------------------------------------------------------

function ModifySkillTemporaryModBaseValue(who, skillid, moda)

  moda := CInt(moda);

  if(moda)
    if(ModifyAttributeTemporaryModBaseValue(who, GetAttributeIdBySkillId(skillid), moda))
      if(skillid > SKILLID__NORMAL_HIGHEST)
        var SkVal := GetEffectiveSkillBaseValue(who, skillid);
        if(moda > 0)
          SendSysMessage(who,"Twa umiejetnosc "+ClStr(GetSkillConfigElem(skillid).Name)+" wzrosla o "+Cdbl(moda*0.1)+"% do wartosci "+Cdbl(SkVal*0.1)+"%.",FONT_NORMAL,COLOR_BLUE);
        elseif(moda < 0)
          SendSysMessage(who,"Twa umiejetnosc "+ClStr(GetSkillConfigElem(skillid).Name)+" zmalala o "+Abs(Cdbl(moda*0.1))+"% do wartosci "+Cdbl(SkVal*0.1)+"%.",FONT_NORMAL,COLOR_BLUE);
        endif
      endif
    endif

    
  endif

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Set Skills
//--------------------------------------------------------------------------------
//================================================================================

function SetBaseSkill(who, skillid, skill)

  return SetBaseSkillBaseValue(who, skillid, Cint(skill)*10);

endfunction

//--------------------------------------------------------------------------------

function SetBaseSkillBaseValue(who, skillid, skill_tenth)

  if(skill_tenth < CORE_SETTINGS_MIN_ATTRIBUTE)
    skill_tenth := CORE_SETTINGS_MIN_ATTRIBUTE;
  elseif(skill_tenth > CORE_SETTINGS_MAX_ATTRIBUTE)
    skill_tenth := CORE_SETTINGS_MAX_ATTRIBUTE;
  endif
  
  if(who.isa(POLCLASS_NPC))
    return SetNpcBaseSkillBaseValue(who, skillid, skill_tenth);
  elseif(who.isa(POLCLASS_MOBILE))
    if(GetBaseSkillBaseValue(who, skillid) != skill_tenth)
      SendAttrEvent(who,EVID_SK_SET, GetAttributeIdBySkillId(skillid), skill_tenth);
    endif
    return 1;
  endif
  
  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function SetSkillTemporaryMod(who, skillid, mod_amt)

  return SetAttributeTemporaryMod(who, skillid, Cint(mod_amt)*10);

endfunction

//--------------------------------------------------------------------------------

function SetSkillTemporaryModBaseValue(who, skillid, mod_tenth)

  return SetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid), mod_tenth);

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Exponents
//--------------------------------------------------------------------------------
//================================================================================

function GetSkillExp(who, SkID)

  return CInt(GetSkillExpBase(who, SkID) / 10);

endfunction

//-----------------------------------------------------------------------------

function GetSkillExpBase(who, SkID)

  return GetAttrExponent(GetEffectiveSkillBaseValue(who, SkID));

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Check Skill
//--------------------------------------------------------------------------------
//================================================================================

function CheckMoreSkill( who, SkillId, Diff, Points )

  if(SkillId == error)
    return 0;
  endif

  if(IsBlockedSkillForClass(who, SkillId))
    return 0;
  endif

  Diff := CDbl(Diff);

  if(Diff == error)
    return 0;
  elseif(Diff == 0.0)
    return 1;
  else
    Diff := Diff * 10.0;
  endif
  
  var Chance    := 0;
  var SkVal     := GetEffectiveSkillBaseValue(who, SkillId);

  if(SkVal < 0)
    SkVal := 0;
  else
    ModifyGainByNutr(who, SkVal, GetAttributeIdBySkillId(SkillId));
  endif
  
  if(Diff < 0)
    Chance := SkVal + ((Diff+10) * 15);
  else
    Chance := CInt((SkVal - Diff) * 3.34) + 500;
  endif
  
  if(Chance < DEFSK_CHECK_MIN_CHANCE)
    Chance := DEFSK_CHECK_MIN_CHANCE;
  elseif(Chance > DEFSK_CHECK_MAX_CHANCE)
    Chance := DEFSK_CHECK_MAX_CHANCE;
  endif

  if(IsGainBlockTimed(who))
    var Elem := GetSkillConfigElem(SkillId);
    if(Elem.AttrGroup == "crafting" or Elem.AttrGroup == "harvest" or Elem.AttrGroup == "mind")
      Chance := CInt(Chance * 0.4);
    endif
  endif
  
  // goqsane: trying to set the gains faster
  Points := CInt(Points * 1.2);

  if(RandomInt(1000) < Chance)
    AwardPoints(who, SkillId, Points, GAIN_RESTRICT, CInt(Diff / 10));
    return 1;
  else
    if(SkVal < DEFSK_ALWAYS_GAIN_UNTIL)
      AwardPoints( who, SkillId, CInt(Points / 2), NO_STAT_AWARD);
    endif
    return 0;
  endif

endfunction

//--------------------------------------------------------------------------------

function ReduceSkill(who, Id, rMult)

  var SkValue := GetBaseSkillBaseValue(who,Id);

  if(SkValue)
    SetBaseSkillBaseValue(who, Id, Cint(SkValue*rMult));
    return 1;
  endif

  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function HasForgot(who, SkId, Time)

  var Sk := GetObjProperty(who, "Forgot_"+SkId);

  if(Sk)
    if(Sk + Time > ReadGameClock())
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function HasBlocked(who, SkId, Time)

  var Sk := GetObjProperty(who, "Blocked_"+SkId);

  if(Sk)
    if(Sk + Time > ReadGameClock())
      return 1;
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsBlockedSkillForClass(who, SkillId)

  if(who.cmdlevel)
	return 0;
  endif
  
  var AttrId := GetAttributeIdBySkillId(SkillId);

  return IsBlockedAttributeForClass(who, AttrId);

endfunction

//--------------------------------------------------------------------------------

function IsBlockedAttributeForClass(who, AttrId)

  if(!AttrId)
    return 0;
  endif

  var cfgClass := GetCharClassConfig(who);

  if(!cfgClass)
    return 0;
  endif

  if(CInt(GetConfigInt(cfgClass, "Block_"+AttrId)) > 0)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsCraftHarvestAttribute(AttrId)

  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);
  var Group := AttrCfg[AttrId].AttrGroup;

  return (Group == "crafting") or (Group == "harvest");

endfunction

//--------------------------------------------------------------------------------
