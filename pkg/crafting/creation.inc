//-----------------------------------------------------------------------------

include "include/player/login";

//-----------------------------------------------------------------------------

const CRAFT_MAX_AMOUNT       := 6;
const CRAFT_PRE_TIMES        := 2;
const CRAFT_MIN_TIMES        := 4;

const CRAFT_MAX_ITERATIONS   := 50;

//-----------------------------------------------------------------------------

function GetCraftTimes(Amount, maxIterAmount := CRAFT_MAX_AMOUNT)

  var Num := CInt((Amount / maxIterAmount) * 0.80);

  return Max(Num, CRAFT_MIN_TIMES);

endfunction

//-----------------------------------------------------------------------------

function GetCraftAmountOnce(Amount, maxIterAmount := CRAFT_MAX_AMOUNT)

  var AmountOnce := CInt(Amount/CRAFT_MIN_TIMES);

  return CInt(MinMax(AmountOnce, 1, maxIterAmount));

endfunction

//-----------------------------------------------------------------------------

function CraftPreDelay(byref who, byref objMaterials, ProdAmount, Sound := 0, Anim := 0)

  var pointer := Malloc(who);
  if(pointer)
	return 0;
  endif

  var FinalAmount := objMaterials.sumAmount * ProdAmount;
  var AmountOnce := GetCraftAmountOnce(FinalAmount, objMaterials.maxIterAmount);
  var Amount := CInt(Min(AmountOnce*CRAFT_PRE_TIMES, FinalAmount));

  return CraftDelay(who, objMaterials, Amount, AmountOnce, CRAFT_PRE_TIMES, Sound, Anim);

endfunction

//-----------------------------------------------------------------------------

function CraftPostDelay(byref who, byref objMaterials, ProdAmount, Sound := 0, Anim := 0)

  var FinalAmount := objMaterials.sumAmount * ProdAmount;
  var AmountOnce := GetCraftAmountOnce(FinalAmount, objMaterials.maxIterAmount);
  var Amount := CInt(FinalAmount - Min(AmountOnce*CRAFT_PRE_TIMES, FinalAmount));

  var aNum := GetCraftTimes(Amount, objMaterials.maxIterAmount) - CRAFT_PRE_TIMES;

  return CraftDelay(who, objMaterials, Amount, AmountOnce, aNum, Sound, Anim);

endfunction

//-----------------------------------------------------------------------------

function CraftNormalDelay(byref who, byref Materials, CraftIters := 1, Sound := 0, Anim := 0)
  
  var AmountOnce := Max(CInt(Materials.sumAmount / CraftIters),  1);
  return CraftDelay(who, Materials, Materials.sumAmount, AmountOnce, CraftIters, Sound, Anim);

endfunction

//-----------------------------------------------------------------------------

function CraftDelay(byref who, byref objMaterial, Amount, AmountOnce, aNum, Sound := 0, Anim := 0)

  var x := who.x;
  var y := who.y;
  var Rlm := who.realm;
  var i, s;
  var whoSer := who.serial;
  var Res := 1;
  
  // make sure single-threaded handling occurred
  var pointer := Malloc(who);
  if(pointer)
	return 0;
  endif
  
  Amount := CInt(Amount);
  AmountOnce := CInt(AmountOnce);

  for(i:=1;i<=aNum;i:=i+1)
    if(sound)
      PlaySoundEffect(who, sound);
    endif
    if(Anim)
      PerformAction(who, Anim);
    endif

    // Nie chcemy przypadku, aby dodawal krok dla resztek w postaci 1 czy 2 ilosci.
    // dlatego tez ilosc krokow jest juz obliczona prawidlowo, trzeba teraz tylko zebrac odpowiednia ilosc skladnika
    if(i == aNum)
      AmountOnce := Amount;
    endif

    if(Amount and AmountOnce)
      Res := SubtractCraftMaterial(objMaterial, AmountOnce);
      if(!Res)
        SendSysMessage(who, Res.errortext, FONT_NORMAL, COLOR_RED);
        return 0;
      endif
    endif

    if(Amount > 0)
      Amount -= AmountOnce;
    endif

    for(s:=1; s<=3; s+=1)
      sleepms(415);

      if(!CheckPlayerRef(who, whoSer))
        Res := 0;
        break;
      elseif(who.dead or who.paralyzed or who.frozen)
        Res := 0;
        break;
      endif

      if(!IsLoggedIn(who))
        if(!WaitForLogon(who, 30))
          Res := 0;
          break;
        endif
      endif

      if(who.x != x or who.y != y or who.realm != Rlm)
        SendSysMessage(who,"Oddalil"+ggm(who,3)+" sie i przerwal"+ggm(who,3)+" wytwarzanie!",FONT_NORMAL,COLOR_RED);
        Res := 0;
        break;
      endif
    endfor

    if(!Res)
      break;
    endif

  endfor

  if(Amount > 0)
    Res := 0;
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function HaveCraftStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#CraftStopGump_"+SkillID);

  if(ExPid(CurGump))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendCraftStopGump(who, SkillID, MaxAmount)

  start_script(":crafting:craft_stop", array(who, SkillID, MaxAmount));

endfunction

//-----------------------------------------------------------------------------

function CloseCraftStopGump(who, SkillID)

  var CurGump := GetObjProperty(who, "#CraftStopGump_"+SkillID);

  if(ExPid(CurGump))
    return CloseGump(who, CurGump);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsCraftStop(who, SkillID)

  if(GetObjProperty(who, "#StopCraft_"+SkillID))
    EraseObjProperty(who, "#StopCraft_"+SkillID);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCraftLastCreation(who, SkillID, craftType := "")

  var LastCraft := GetObjProperty(who, "LastCraft"+craftType+"_"+SkillID);

  if(!LastCraft or LastCraft.objtype == error)
    LastCraft := struct;
    LastCraft.objtype := 0;
    LastCraft.iters   := 1;
  endif

  LastCraft.objtype := CInt(LastCraft.objtype);
  LastCraft.iters   := CInt(LastCraft.iters);
  
  if(LastCraft.objtype < 0 or LastCraft.objtype > 0xFFFF)
    LastCraft.objtype := 0;
  endif

  LastCraft.iters := MinMax(LastCraft.iters, 1, CRAFT_MAX_ITERATIONS);

  return LastCraft;
  
endfunction

//-----------------------------------------------------------------------------

function SetCraftLastCreation(who, SkillID, byref LastCraft, craftType := "")

  SetObjProperty(who, "LastCraft"+craftType+"_"+SkillID, LastCraft);

endfunction

//-----------------------------------------------------------------------------

function GetCraftItersFromGumpRes(GumpRes, Id)

 return MinMax(CInt(CroppedId(GumpRes[Id])), 1, CRAFT_MAX_ITERATIONS);

endfunction

//-----------------------------------------------------------------------------
