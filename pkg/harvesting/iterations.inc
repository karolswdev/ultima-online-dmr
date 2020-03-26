//--------------------------------------------------------------------------------

function DefineHarvestIterations(who, SkId, MaxIters := 0)

  EraseObjProperty(who, "#StopHarvest_"+SkId);

  if(!MaxIters)
    if(GetObjProperty(who, PROP_HARVEST_ONE))
      MaxIters := 1;
    else
      MaxIters := HARVEST_MAX_ITERATIONS;
    endif
  endif

  var Data := struct;

  Data.+curIter := 0;
  Data.+maxIters := MaxIters;
  Data.+SkillId  := SkId;

  return Data;

endfunction

//--------------------------------------------------------------------------------

function HarvestIteration(who, byref IterData)

  if(IterData.curIter == IterData.maxIters or IsHarvestStop(who, IterData.SkillId))
    FinishHarvestIteration(who, IterData);
    return 0;
  endif

  IterData.curIter += 1;
  return 1;

endfunction

//--------------------------------------------------------------------------------

function FinishHarvestIteration(who, byref IterData)

  CloseHarvestStopGump(who, IterData.SkillId);

endfunction

//--------------------------------------------------------------------------------

function GetHarvestIterDesc(byref IterData)

  return "("+IterData.curIter+" z "+IterData.maxIters+")";

endfunction

//--------------------------------------------------------------------------------
// ProdAmount - ilosc tworzonego produktu, w stosunku 1 produkt = pobranie calej grupy itemow zdefinowanych jako materialy.
// MultAmount - to jest ilosc, jaka zostanie utworzona w stosunku do zuzycia materialu na jeden produkt.
// przyklad dobry to blankscrolls - gdzie materialy to 1 deska, ale chcemy utworzyc 10 scrolli (dla gma).
//--------------------------------------------------------------------------------

function HarvestIterationGump(who, byref IterData, Type := 0)  

  if(IterData.maxIters > 1)
    SendHarvestStopGump(who, IterData.SkillId, Type);
  endif

endfunction

//--------------------------------------------------------------------------------
