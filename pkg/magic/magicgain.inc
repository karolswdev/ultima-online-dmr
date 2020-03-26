//------------------------------------------------------------------------

const MAGICGAIN_OFFENSIVE        := 1;
const MAGICGAIN_DEFENSIVE        := 2;
const MAGICGAIN_NORMAL           := 3;
const MAGICGAIN_SUMMON           := 4;

const MAGICGAIN_ATTACK           := 5;

const MAGIC_GAIN_MOD             := 1.75;
const MAGIC_GAIN_NORMAL_MOD      := 0.25;
const MAGIC_GAIN_SUMMON_MOD      := 0.5;  
const MAGIC_GAIN_ATTACK_MOD      := 0.4;

//------------------------------------------------------------------------

function CanSpellGain(Caster, CastOn, Type)

  if(!IsPlayer(Caster))
    return 1;
  elseif(Caster == CastOn)
    return 0;
  endif
  
  if(CastOn.enabled("invul") || CastOn.script["storeman"] || CastOn.script["banker"] || CastOn.script["merchant"])
		PrintTextAbove(Caster, "*oszust*");
		return 0;
  endif

  if(Type != MAGICGAIN_OFFENSIVE)
    if(IsPlayer(CastOn))
      return 0;
    elseif(CastOn.isa(POLCLASS_NPC))
	
      if(!IsMobGainable(CastOn, Caster))
        return 0;
      endif
	  
	  if(CastOn.enabled("invul") || CastOn.script["storeman"] || CastOn.script["banker"] || CastOn.script["merchant"])
		PrintTextAbove(Caster, "*oszust*");
		return 0;
	  endif
    endif
  endif

  return 1;

endfunction

//------------------------------------------------------------------------

function GainMagicSpellPoints(Caster, CastOn, SpellID, Type)

	if(CastOn.enabled("invul") || CastOn.script["storeman"] || CastOn.script["banker"] || CastOn.script["merchant"])
		PrintTextAbove(Caster, "*oszust*");
		return 0;
	  endif

  if(CInt(GetObjProperty(Caster, "#LMG_"+SpellID)) > ReadGameClock())
    return 0;
  endif

  SetObjProperty(Caster, "#LMG_"+SpellID, ReadGameClock()+1);

  if(!CanGainOnNpcWithCag(CastOn, Caster))
    return 0;
  endif

  var Circle := GetSpellValue(SpellID, "Circle");

  if(!CanSpellGain(Caster, CastOn, Type))
    return 0;
  endif

  var Points := GetCircleValue(Circle,"Points");

  case(Type)
    MAGICGAIN_OFFENSIVE:
    MAGICGAIN_DEFENSIVE: 
      Points *= MAGIC_GAIN_MOD;
    MAGICGAIN_NORMAL:     
      Points *= MAGIC_GAIN_NORMAL_MOD;
    MAGICGAIN_SUMMON:
      Points *= MAGIC_GAIN_SUMMON_MOD;
    MAGICGAIN_ATTACK:
      Points *= MAGIC_GAIN_ATTACK_MOD;
  endcase

  if(Type != MAGICGAIN_OFFENSIVE)
    var Diff   := GetCircleValue(Circle,"Diff");
    var SkillId := GetMagicSkillID(Type);
    Points := GetRestrictedPoints(Caster, SkillId, Max(0, Diff+20), Points);
  endif

  var SpellType := GetSpellValue(SpellID, "SpellType");

  CagNpcDamaged(CastOn, MinMax(CInt((100-Points)/5), 6, 22), Caster);
  return GainMagicPoints(Caster, SpellType, Points, Circle);

endfunction

//------------------------------------------------------------------------

function GainMagicPoints(Caster, SpellType, Points, Circle)

  var SkillId := GetMagicSkillID(SpellType);

  if(Points)
    AwardPoints(Caster, SkillId, Points, STAT_GAIN_MODIFIED, 7.0 / CDbl(Max(0, Circle-2))); // 50% + 7% za kazdy krag od drugiego.
    AwardPoints(Caster, GetMindSkillID(SpellType), CInt(Points * SECOND_SKILL_GAIN_MOD), NO_STAT_AWARD);
  endif

  return 1;

endfunction

//------------------------------------------------------------------------
