//-----------------------------------------------------------------------

include "include/clock";

//-----------------------------------------------------------------------

const VAMPIRE_CHANCE                   := 2;
const VPSK_MINCURE                     := 70;
const VAMP_COLOR                       := 1154;


const VAMPIRE_HOUR                     := 21;
const VAMPIRE_END_HOUR                 := 9;
const VP_GARGOYLE_GRAPHIC              := 754;

//-----------------------------------------------------------------------

const WC_BLOOD_ARMOR                   := 31;
const WC_FIRE_BLOOD                    := 30;
const WC_ICE_BLADE                     := 28;
const WC_LIFE_STOLEN                   := 100;

//-----------------------------------------------------------------------

function VampireBite(who)

  if(IsPlayer(who))
    if(RandomInt(100) < VAMPIRE_CHANCE)
      if(!GetBaseSkillBaseValue(who, SKILLID_KENSAI) and !GetBaseSkillBaseValue(who, SKILLID_BARBARIAN) and !IsWearWolf(who) and !IsVampire(who) and !GetObjProperty(who,"CanBeWearWolf") and !IsDruid(who))
        SetObjProperty(who,"CanBeVampire",1);
        return 1;
      endif
    endif
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------

function IsInVampireChange(who)

  return ReadGameClock() - CInt(GetObjProperty(who,"#VampChangeComplete")) < CInt(GetDayLength()/2);

endfunction

//-----------------------------------------------------------------------

function GetColorBefVamp(who)

  var Color := CInt(GetObjProperty(who,"ColorBeforeVamp"));
  if(!Color)
    Color := 0x8405;
  endif

  return Color;

endfunction

//-----------------------------------------------------------------------

function IsInVampBurningArea(who)
  if(!IsDungeon(who.x,who.y,who.realm) and !IsInUmbra(who) and !IsProtectedArea(who) and !IsInGa(who.x, who.y, who.realm) and !IsInVampireHouse(who))
    return 1;
  else
    return 0;
  endif
endfunction

//------------------------------------------------------------------------

function IsProtectedArea(who)

  var areaCfg := ReadConfigFile(VAMP_AREA_CFG);

  foreach AreaId in GetConfigStringKeys(areaCfg)
    if(who.x >= areaCfg[AreaId].sX and who.x <= areaCfg[AreaId].eX and who.y >= areaCfg[AreaId].sY and who.y <= areaCfg[AreaId].eY and who.realm == areaCfg[AreaId].Realm)
      return AreaId;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckVampireAttackGain(who, defender)

  if(!IsVampire(who))
    return;
  endif

  if(CanMod(who, MOD_ONHIT_AR) == 0)
    GainWarcastPoints(who, defender, WC_BLOOD_ARMOR, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_FIRE_BLOOD) == 0)
    GainWarcastPoints(who, defender, WC_FIRE_BLOOD, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_COLD_DMG) == 0)
    GainWarcastPoints(who, defender, WC_ICE_BLADE, WCGAIN_ATTACK);
  elseif(CanMod(who, MOD_LIFE_STOLEN) == 0)
    GainWarcastPoints(who, defender, WC_LIFE_STOLEN, WCGAIN_ATTACK);
  endif

endfunction

//--------------------------------------------------------------------------------

function IsVampireModTimePlace(Caster)

  var hour := GetHour();
 
  if(hour < VAMPIRE_HOUR and hour >= VAMPIRE_END_HOUR)
    if(!IsDungeon(Caster.x, Caster.y, Caster.realm))
      return 0;
    endif
  endif

  return 1;

endfunction

//------------------------------------------------------------------------
