//---------------------------------------------------------------------

function GenMaxDrunkness(who)

  var MaxDrunk := (GetExponent(GetBaseStrengthBaseValue(who) * 0.55) * 0.4) + (GetDexterity(who) * 0.15) + RandomInt(10);

  SetObjProperty(who, PROP_MAX_DRUNKNESS, CInt(MinMax(MaxDrunk, 10, 200) + RandomInt(6)));

endfunction

//---------------------------------------------------------------------

function GetDrunkPercentage(who)

  var Drunk    := GetObjProperty(who, PROP_DRUNKNESS);
  var maxDrunk := GetObjProperty(who, PROP_MAX_DRUNKNESS);

  if(!Drunk or !maxDrunk)
    return 0;
  endif

  var Perc := CInt(Drunk * 100 / maxDrunk);

  return MinMax(Perc, 0, 100);

endfunction

//---------------------------------------------------------------------

function GetDrunkFactor(who)

  return CInt(GetDrunkPercentage(who) / 20);

endfunction

//---------------------------------------------------------------------
