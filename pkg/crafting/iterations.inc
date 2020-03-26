//--------------------------------------------------------------------------------

function DefineCraftIterations(who, SkId, MaxIters, Materials)

  EraseObjProperty(who, "#StopCraft_"+SkId);

  var Data := struct;

  Data.+curIter := 0;
  Data.+maxIters := MaxIters;
  Data.+SkillId  := SkId;
  Data.+failCount  := 0;
  Data.+failed := 0;
  Data.+availAmount := GetProductAmountForMaterials(Materials);

  return Data;

endfunction

//--------------------------------------------------------------------------------

function CraftIteration(who, byref IterData)

  if(!IterData.failed)
    IterData.failCount := 0;
  else
    IterData.maxIters += 1;
  endif

  if(IterData.curIter == IterData.maxIters or IsCraftStop(who, IterData.SkillId) or IterData.failCount > 10)
    FinishCraftIteration(who, IterData);
    return 0;
  endif

  IterData.curIter += 1;
  IterData.failed := 0;
  return 1;

endfunction

//--------------------------------------------------------------------------------

function FailCraftIteration(byref IterData)

  IterData.failCount += 1;
  IterData.failed := 1;

endfunction

//--------------------------------------------------------------------------------

function FinishCraftIteration(who, byref IterData)

  CloseCraftStopGump(who, IterData.SkillId);

endfunction

//--------------------------------------------------------------------------------

function GetCraftIterDesc(byref IterData)

  return "("+IterData.curIter+" z "+IterData.maxIters+")";

endfunction

//--------------------------------------------------------------------------------
// ProdAmount - ilosc tworzonego produktu, w stosunku 1 produkt = pobranie calej grupy itemow zdefinowanych jako materialy.
// MultAmount - to jest ilosc, jaka zostanie utworzona w stosunku do zuzycia materialu na jeden produkt.
// przyklad dobry to blankscrolls - gdzie materialy to 1 deska, ale chcemy utworzyc 10 scrolli (dla gma).
//--------------------------------------------------------------------------------

function CraftIterationGump(who, byref IterData, ProdAmount, MultAmount := 1)  

  var maxAvailIters := CInt(IterData.availAmount / ProdAmount);

  if(maxAvailIters * ProdAmount < IterData.availAmount)
    maxAvailIters += 1;
  endif

  IterData.maxIters := Min(IterData.maxIters, maxAvailIters);
  IterData.maxAmount := Min((IterData.maxIters - IterData.curIter+1)*ProdAmount, IterData.availAmount);

  if(IterData.maxIters > 1)
    SendCraftStopGump(who, IterData.SkillId, IterData.maxAmount*MultAmount);
  endif

endfunction

//--------------------------------------------------------------------------------
