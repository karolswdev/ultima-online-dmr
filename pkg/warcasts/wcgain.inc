//--------------------------------------------------------------------------------

const WARCASTS_CFG           := ":*:warcasts";
const WCTYPES_CFG            := ":*:wctypes";

//-----------------------------------------------------------------------------

const WCGAIN_DEFENSIVE       := 1;
const WCGAIN_OFFENSIVE       := 2;
const WCGAIN_NORMAL          := 3;
const WCGAIN_ATTACK          := 4;

const WC_GAIN_MOD            := 2.5;
const WC_GAIN_NORMAL_MOD     := 0.23;
const WC_GAIN_ATTACK_MOD     := 0.30;

//------------------------------------------------------------------------

function CanWarcastGain(Caster, CastOn, Type)

  if(!IsPlayer(Caster))
    return 1;
  elseif(Caster == CastOn)
    if(Type == WCGAIN_DEFENSIVE)
      return 1;
    else
      return 0;
    endif
  endif

  if(Type == WCGAIN_OFFENSIVE)
    if(IsPlayer(CastOn))
      return 0;
    elseif(CastOn.isa(POLCLASS_NPC))
      if(!IsMobGainable(CastOn, Caster))
        return 0;
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GainWarcastPoints(Caster, CastOn, WcID, Type)

  if(!CanWarcastGain(Caster, CastOn, Type))
    return 0;
  endif

  if(CInt(GetObjProperty(Caster, "#LWG_"+WcID)) > ReadGameClock())
    return 0;
  endif

  SetObjProperty(Caster, "#LWG_"+WcID, ReadGameClock()+2);

  if(!CanGainOnNpcWithCag(CastOn, Caster))
    return 0;
  endif

  var WarCfg  := ReadConfigFile(WARCASTS_CFG);
  var WctCfg  := ReadConfigFile(WCTYPES_CFG);
  
  var Cast := WarCfg[wcId];
  
  var GainUntil := CInt(WctCfg[Cast.Type].GainUntil);
  var MainSkId  := CInt(WctCfg[Cast.Type].MainSkId);
  var MindSkId  := CInt(WctCfg[Cast.Type].MindSkId);
  var MainBSk   := GetEffectiveSkillBaseValue(Caster, MainSkId);
  var Diff      := CInt(Cast.Diff);
  var NoRestr   := CInt(Cast.NoGainRestrict);
  var Points    := CInt(Cast.Points);

  case(Type)
    WCGAIN_OFFENSIVE:
    WCGAIN_DEFENSIVE: 
      if(Caster != CastOn) // dla defensywnych mozna
        Points *= WC_GAIN_MOD;
      endif
    WCGAIN_NORMAL:     
      Points *= WC_GAIN_NORMAL_MOD;
    WCGAIN_ATTACK:
      Points *= WC_GAIN_ATTACK_MOD;
  endcase

  CagNpcDamaged(CastOn, MinMax(CInt(Points/5), 3, 8), Caster);

  if(!GainUntil or (GainUntil and CInt(MainBSk/10) < GainUntil))
    if(NoRestr)
      AwardPoints(Caster, MainSkId, Points);
    else
      AwardPoints(Caster, MainSkId, Points, GAIN_HALF_RESTRICT, Diff);
    endif
    AwardPoints(Caster, MindSkId, CInt(Points * SECOND_SKILL_GAIN_MOD), NO_STAT_AWARD);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
