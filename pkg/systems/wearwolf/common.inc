//-----------------------------------------------------------------------

include "include/clock";

//-----------------------------------------------------------------------

const WEARWOLF_CHANCE                  := 5;
const WWSK_MINCURE                     := 70;
const WW_MORPH_COLOR_WARRIOR           := 1109;
const WW_MORPH_COLOR_MAGE              := 1050;

//-----------------------------------------------------------------------

const WC_BERSERK                       := 13;
const WC_CLAWOFDEATH                   := 11;
const WC_PREDATOR                      := 15;

//-----------------------------------------------------------------------

function WearWolfBite(who)

  if(IsPlayer(who))
    if(RandomInt(100) < WEARWOLF_CHANCE)
      if(!GetBaseSkillBaseValue(who, SKILLID_KENSAI) and !GetBaseSkillBaseValue(who, SKILLID_BARBARIAN) and !IsWearWolf(who) and !IsVampire(who) and !GetObjProperty(who,"CanBeVampire") and !IsNecro(who))
        SetObjProperty(who,"CanBeWearWolf",1);
        return 1;
      endif
    endif
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsWearWolfMorphed(who)

  return CInt(GetObjProperty(who,"#WearWolfMorph"));

endfunction

//-----------------------------------------------------------------------------

function IsWolfMorphed(who)

  if(IsWearWolfMorphed(who))
    return 1;
  elseif(who.graphic == CID_WOLF or who.graphic == CID_GRIZZLY)
    return 1;
  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetWStSk(who)

  if(!GetObjProperty(who,"#WearWolfStartSkill"))
    SetObjProperty(who,"#WearWolfStartSkill",GetEffectiveSkill(who,SKILLID_WEARWOLF));
  endif
  return CInt(GetObjProperty(who,"#WearWolfStartSkill"));

endfunction

//-----------------------------------------------------------------------

function GetWstSkAbove(who)

  return ToMin(GetWstSk(who) - 100, 0);

endfunction

//-----------------------------------------------------------------------

function GetWearwolfName(who)

  var Name := "wilkolak";

  if(IsWearwolf(who))
    if(GetEffectiveSkill(who, SKILLID_WEARWOLF) >= 80)
      Name := GetObjProperty(who, "WolfName");
      if(!Name)
        Name := "wilkolak";
      endif
    endif
  endif

  return Name;

endfunction

//-----------------------------------------------------------------------

function GetMorphColor(who)

  var MorphColor;

  if(IsCharClass(who, CLASS_MAGE))
    MorphColor := WW_MORPH_COLOR_MAGE;
  else
    MorphColor := WW_MORPH_COLOR_WARRIOR;
  endif

  return (MorphColor - (4 - CInt(GetWStSk(who) / 25)));

endfunction

//-----------------------------------------------------------------------

function IsWearWolfAffected(who)

  return GetObjProperty(who,"#WWInitiateComplete") or GetObjProperty(who,"#EndedMorph");

endfunction

//-----------------------------------------------------------------------

function CheckWearWolfAttackGain(who, defender)

  if(!IsWearWolf(who))
    return;
  endif

  if(CanMod(who, MOD_FIRE_DMG) == 0)
    GainWarcastPoints(who, defender, WC_BERSERK, WCGAIN_ATTACK);
  endif

  if(CanMod(who, MOD_POISON_DMG) == 0)
    GainWarcastPoints(who, defender, WC_CLAWOFDEATH, WCGAIN_ATTACK);
  endif

  if(CanMod(who, MOD_RETORT) == 0)
    GainWarcastPoints(who, defender, WC_PREDATOR, WCGAIN_ATTACK);
  endif

endfunction

//------------------------------------------------------------------------
