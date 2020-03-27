//--------------------------------------------------------------------------------

use uo;
use math;
use cfgfile;
use vitals;
use attributes;

//--------------------------------------------------------------------------------

include "include/const/attributes";
include "include/const/cfgs";

include "include/attributes/stats";
include "include/attributes/vitals";
include "include/attributes/skills";
include "include/attributes/award";

include "include/client/skills";
include "include/checks/stdchecks";
include "include/player/target";
include "include/finding";

include ":character:common";

//================================================================================
//--------------------------------------------------------------------------------
// Attributes
//--------------------------------------------------------------------------------
//================================================================================

function IsSetAttrFlag(who, byref AttrId, Flag)

  var AttrVal := GetObjProperty(who, PROP_ATTRIBUTE + AttrId);

  if(AttrVal)
    if(AttrVal[2] & Flag)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetAttrFlag(who, byref AttrId, Flag)

  var AttrVal := GetObjProperty(who, PROP_ATTRIBUTE + AttrId);

  if(!AttrVal or !AttrVal.size())
    AttrVal := array(BaseToRawSkill(GetAttributeBaseValue(who, AttrId)), Flag);
  else
    SetBitOn(AttrVal[2], Flag);
  endif
  
  SetObjProperty(who, PROP_ATTRIBUTE + AttrId, AttrVal);

endfunction

//-----------------------------------------------------------------------------

function UnSetAttrFlag(who, byref AttrId, Flag)

  var AttrVal := GetObjProperty(who, PROP_ATTRIBUTE + AttrId);

  if(!AttrVal or !AttrVal.size())
    AttrVal := array(BaseToRawSkill(GetAttributeBaseValue(who, AttrId)), 0);
  else
    SetBitOff(AttrVal[2], Flag);
  endif
  
  SetObjProperty(who, PROP_ATTRIBUTE + AttrId, AttrVal);

endfunction

//-----------------------------------------------------------------------------

function SetRawSkillGainWatcher(Watcher, Targ := 0)

  if(!Targ)
    Targ := ObjTarget(Watcher, TGTOPT_NOCHECK_LOS);
  endif
  
  if(!Targ or !Targ.isa(POLCLASS_MOBILE))
    SendSysMessage(Watcher, "Anulowano.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  if(!CanUseCmd(Watcher, "tinfo sk") and !CmdHaveAccess(Watcher, Targ, CMDA_READ))
    SendSysMessage(Watcher, "Nie masz dostepu do tej czynnosci.", FONT_NORMAL, COLOR_RED);
    return;
  endif

  EraseRawSkillGainWatcher(Watcher);
  SetObjProperty(Targ, PROP_SKILLGAIN_WATCHER, Watcher.serial);
  SetObjProperty(Watcher, PROP_SKILLGAIN_WATCH, Targ.serial);
  SendSysMessage(Watcher, "Sledzenie Sk: "+Targ.name+".",FONT_NORMAL,COLOR_BLUE);

endfunction

//-----------------------------------------------------------------------------

function EraseRawSkillGainWatcher(Watcher)

  if(GetObjProperty(Watcher, PROP_SKILLGAIN_WATCH))
    var Targ := FindPlayerBySerial(GetObjProperty(Watcher, PROP_SKILLGAIN_WATCH));
    if(Targ)
      SendSysMessage(Watcher, "Koniec sledzenia Sk: "+Targ.name+".", FONT_NORMAL, COLOR_RED);
      EraseObjProperty(Targ, PROP_SKILLGAIN_WATCHER);
      EraseObjProperty(Watcher, PROP_SKILLGAIN_WATCH);
      return 1;
    endif
  endif

  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function CheckGainAttributeWatcher(who, byref AttrId, RwPoints)
 
  if(AttrId)
    var CurRwVal := GetRawAttributeValue(who, AttrId);
    SetLastUse(who, LAST_USE_SKILL, AttrId, "RP: "+RwPoints+" WA: "+CurRwVal+" SK: "+(RawToBaseSkill(CurRwVal)*0.1));

    if(GetObjProperty(who, PROP_SKILLGAIN_WATCHER))
      var Watcher := FindPlayerBySerial(GetObjProperty(who, PROP_SKILLGAIN_WATCHER));
      if(!Watcher)
        EraseObjProperty(who, PROP_SKILLGAIN_WATCHER);
      elseif(!IsLoggedIn(watcher) or (!watcher.cmdlevel and !IsOnDMRTest()))
        EraseObjProperty(who, PROP_SKILLGAIN_WATCHER);
        EraseObjProperty(watcher, PROP_SKILLGAIN_WATCH);
      else
        var CurRaw := BaseToRawSkill(RawToBaseSkill(CurRwVal));
        var NextRaw := BaseToRawSkill(RawToBaseSkill(CurRwVal)+1) - CurRaw;
        var Color, sPerc := "";
        case(AttrId)
          ATTRIBUTEID_STRENGTH:
          ATTRIBUTEID_DEXTERITY:
          ATTRIBUTEID_INTELLIGENCE:
            Color := 1085;
          default:
            sPerc := "%";
            Color := 1153;
        endcase
        SendSysMessage(watcher, who.name+" # "+AttrId+": "+(RawToBaseSkill(CurRwVal)*0.1) + sPerc + " RP: "+RwPoints+" ("+((CurRwVal-CurRaw)/RwPoints)+"/"+(NextRaw / RwPoints)+" "+(CurRwVal-CurRaw)+"/"+NextRaw+")", FONT_SMALL_LIGHT, Color);
      endif
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function BaseToRawSkill(basevalue)

  if(basevalue <= 200)
    var rawvalue := basevalue * 20.48;
    if(rawvalue > Cint(rawvalue))
      return Cint(rawvalue + 1);
    else
      return Cint(rawvalue);
    endif
  else
    var scale  := basevalue / 100.0;
    var remain := scale - Cint(scale);
    return Cint(Pow(2, Cint(scale))*(1024 *(1.0 + remain)+ 1));
  endif

endfunction

//--------------------------------------------------------------------------------

function RawToBaseSkill(rawvalue)

  if(rawvalue == error)
    return 0;
  endif

  var scale := Cint(Log10(rawvalue)/Log10(2));
  if(scale <= 11)
    return Cint(rawvalue / 20.48);
  else
    return Cint((rawvalue / Pow(2,scale)+ scale - 11)* 100);
  endif

endfunction

//--------------------------------------------------------------------------------

function GetAttributeIdBySkillId(SkId)

  var UoSkCfg := ReadConfigFile(UOSKILLS_CFG);

  if(!UoSkCfg[SkId].Attribute)
    return error;
  else
    return UoSkCfg[SkId].Attribute;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetSkillIdByAttributeId(byref AttrId)

  var AttrCfg := ReadConfigFile(ATTRIBUTES_CFG);

  if(AttrCfg[AttrId].SkillID == error)
    return error;
  else
    return AttrCfg[AttrId].SkillID;
  endif

endfunction

//--------------------------------------------------------------------------------

function GetAttributeGroupConfig(Name)

  var Cfg := ReadConfigFIle(ATTRGROUPS_CFG);
  
  return Cfg[Name];

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Temporary Mods
//--------------------------------------------------------------------------------
//================================================================================

function ModifyAttributeTemporaryMod(who, byref AttrId, ModAmt)

  if(AttrId and who.isa(POLCLASS_MOBILE))
    return SetAttributeTemporaryMod(who, AttrId, GetAttributeTemporaryMod(who, AttrId) + CInt(ModAmt)*10);
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function ModifyAttributeTemporaryModBaseValue(who, byref AttrId, ModAmt)

  if(AttrId and who.isa(POLCLASS_MOBILE))
    return SetAttributeTemporaryMod(who, AttrId, GetAttributeTemporaryMod(who, AttrId) + CInt(ModAmt));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetLastUse(who, lType, uType, Msg)

  case(lType)
    LAST_USE_TARGET:  SetObjProperty(who, PROP_LTRG_USE, array(uType, PolCore().systime, Msg));
    LAST_USE_SKILL:   SetObjProperty(who, PROP_LSK_USE,  array(uType, PolCore().systime, Msg));
  endcase

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Exponents
//--------------------------------------------------------------------------------
//================================================================================

function GetExponent(Val, Div := 100)

  var Scale := CInt(Val / Div);

  if(Scale < 8)
    return (Val / (Div/25));
  else
    var Exp   := Pow(2, Scale);
    var nExp  := Pow(2, Scale+1);
    var Perc  := (Div - (((Scale+1) * Div) - Val)) * 0.01;

    return Exp + ((nExp - Exp) * Perc);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetAttrExponent(Val)

  var Exp := GetExponent(Val, 150);

  if(Exp > 1024)
    Exp := 1024 + ((Exp - 1024) / 6);
  endif

  return CInt(Exp);

endfunction

//-------------------------------------------------------------------------------

function GetAttributeHundrethValue(who, AttrId)

  if(who.isa(POLCLASS_NPC))
    return GetAttributeBaseValue(who, AttrId)*10;
  endif

  var baseRaw := BaseToRawSkill(GetAttributeBaseValue(who, AttrId));
	
  var Exp := CDbl(GetRawAttributeValue(who, AttrId) - baseRaw) / (BaseToRawSkill(GetAttributeBaseValue(who, AttrId)+1) - baseRaw);
  Exp := ToMin(Exp, 0.0);
  Exp := (GetAttributeBaseValue(who, AttrId) + Exp) * 10;

  return CInt(Exp);
	
endfunction
	
//-------------------------------------------------------------------------------
