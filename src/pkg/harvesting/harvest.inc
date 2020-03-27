//-----------------------------------------------------------------------------

const HARVEST_MAX_AMOUNT := 20;
const HARVEST_MAX_ITERATIONS := 30;

//-----------------------------------------------------------------------------

function GetHarvestAmount(who, SkillID)

  var Skill := GetBaseSkill(who, SkillID);
  var OverSkill := Min(20, Max(GetEffectiveSkill(who, SkillID) - 100, 0));
  return 30 + CInt(Skill / 2) + OverSkill;

endfunction

//-----------------------------------------------------------------------------
// punkty za przetwarzanie - tylko do skilla, brak pkt do statow w tym przypadku
// przetwarzanie, czyli obcinanie ziol czy przetapianie sztab.
//-----------------------------------------------------------------------------

function GetHarvestProcessPoints(SkillID, Diff)

  var Points := GetDefaultPointsValue(SkillID) * 0.25;
  var Perc := 1.0 + CDbl(Diff) / 100.0;

  if(Perc < 0.1)
    Perc := 0.1;
  endif

  Points := CInt(Min(CDbl(Points) * Perc, Points*1.5));

  return Points;

endfunction

//-----------------------------------------------------------------------------

function GetHarvestPoints(SkillID, Diff)

  var Points := GetDefaultPointsValue(SkillID);
  var Perc := 1.0 + CDbl(Diff) / 100.0;

  Points := CInt(Min(CDbl(Points) * Perc, Points*1.5));

  return Points;

endfunction

//--------------------------------------------------------------------------------

function HarvestDelay(who, byref IterData, SoundId := 0, AnimId := 0, Type := 0, TimeMod := 1.0)

  HarvestIterationGump(who, IterData, Type);

  if(!MakeNoise(who, SoundId, MinMax(CInt(1500*TimeMod), 900, 1500), AnimId))
    return error;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsHarvestStop(who, SkillID)

  if(GetObjProperty(who, "#StopHarvest_"+SkillID))
    EraseObjProperty(who, "#StopHarvest_"+SkillID);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveHarvestStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#HarvestStopGump_"+SkillID);

  if(ExPid(CurGump))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendHarvestStopGump(who, SkillID, Type := 0)

  start_script(":harvesting:harvest_stop", array(who, SkillID, Type));

endfunction

//-----------------------------------------------------------------------------

function CloseHarvestStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#HarvestStopGump_"+SkillID);

  if(ExPid(CurGump))
    return CloseGump(who, CurGump);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetObjTypesForHarvest(SkID)

  var Data := array;

  case(SkID)
    SKILLID_MINING:
      Data := GetIngotsObjTypes();
      Data += GetOresObjTypes();
      
    SKILLID_LUMBERJACKING:
      Data := GetCraftMatGroupObjTypes("woods");

    SKILLID_TRACKING:
      Data := GetCraftMatGroupObjTypes("leathers");
      Data += GetCraftMatGroupObjTypes("hides");
  endcase

  return Data;

endfunction

//-----------------------------------------------------------------------------

function HarvestOverflowed(who)

  if(HaveChance(75))
    var Overflowed := GetHarvestOverflowChance(who);
    if(Overflowed and HaveChance(Overflowed))
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetHarvestOverflowChance(who)

  return 1;

  var Chance := 0;
  var SkCount := 0;
  var Cfg := ReadConfigFile(ATTRIBUTES_CFG);

  foreach AttrId in GetConfigStringKeys(Cfg)
    if(Cfg[AttrId].AttrGroup == "harvest")
      var SkId := CInt(Cfg[AttrId].SkillID);

      if(GetBaseSkillBaseValue(who, SkId) >= 300)
        SkCount += 1;
      endif
    endif
  endforeach

  if(SkCount > 2)
    Chance := (SkCount-2) * 10;
  endif

  return Chance;

endfunction

//-----------------------------------------------------------------------------
