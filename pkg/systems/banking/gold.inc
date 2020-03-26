//-----------------------------------------------------------------------------

include "common";
include "include/items";
include "include/player/misc";
include "include/attributes/main";
include "include/objtypes/misc";

//=============================================================================
//-----------------------------------------------------------------------------
// Cheque / Gold
//-----------------------------------------------------------------------------
//=============================================================================

function GetChequeVal(Cheque)

  return CInt(GetObjProperty(Cheque,"Value"));

endfunction

//-----------------------------------------------------------------------------

function SetChequeVal(Cheque,Val)

  SetObjProperty(Cheque,"Value",CInt(Val));
  SetChequeName(Cheque);

endfunction

//-----------------------------------------------------------------------------

function CreateChequeInPack(who, Amount)

  var Cheque := CreateItemInPack(who, UOBJ_CHEQUE, 1);

  if(Cheque)
    SetChequeVal(Cheque, Amount);
  endif

  return Cheque;

endfunction

//-----------------------------------------------------------------------------

function SetChequeName(Cheque)

  if(GetChequeVal(Cheque) <= 0)
    SetName(Cheque,"Bezwartosciowy czek");
  else
    SetName(Cheque,"Czek na "+GetChequeVal(Cheque)+" zlota");
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckPackForCheque(who)

  var Gold := 0;
  
  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(Item.objtype == UOBJ_CHEQUE)
      Gold := Gold + GetChequeVal(Item);
    endif
  endforeach
  
  SetObjProperty(who,"ChGold",Gold);
  
  UpdateStatus(who);

endfunction

//-----------------------------------------------------------------------------

function SubtractCheque(who, Cheque, Val)

  var curAmt := GetChequeVal(Cheque);

  if(curAmt >= Val)
    SetChequeVal(Cheque, curAmt - Val);
    CheckPackForCheque(who);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetGold(who)

  return CInt(who.gold) + CInt(GetObjProperty(who,"ChGold"));

endfunction

//-----------------------------------------------------------------------------

function HaveGold(who, Gold)

  Gold := CInt(Gold);
  
  if(GetGold(who) < Gold)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SpendGold(who, Gold)

  Gold := CInt(Gold);
  
  if(GetGold(who) < Gold)
	if(SpendBankAccountAmount(who, Gold))
		return 1;
	endif
    return 0;
  endif

  if(who.SpendGold(Gold))
    return 1;
  endif
  
  var pGold := who.gold;
  var sGold := Gold - pGold;
  
  var Cheques := array;
  
  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(Item.objtype == UOBJ_CHEQUE)
      Cheques.append(Item);
      if(GetChequeVal(Item) < sGold)
        sGold := sGold - GetChequeVal(Item);
      else
        sGold := 0;
        break;
      endif
    endif
  endforeach
  
  if(sGold)
    return 0;
  endif
  
  if(pGold)
    if(!who.SpendGold(pGold))
      return 0;
    endif
  endif
  
  sGold := Gold - pGold;
  
  if(!Cheques.size())
    return 0;
  endif
  
  foreach Cheque in Cheques
    if(GetChequeVal(Cheque) < sGold)
      sGold := sGold - GetChequeVal(Cheque);
      SetChequeVal(Cheque,0);
    else
      SetChequeVal(Cheque,GetChequeVal(Cheque) - sGold);
      sGold := 0;
      break;
    endif
  endforeach

  CheckPackForCheque(who);
  
  return !sGold;

endfunction

//-----------------------------------------------------------------------------

function SendGoldCheck(who)

  var Pid := GetProcess(CInt(GetObjProperty(who, PROP_PLAYERSPEECH_PID)));
  var ev  := struct;
  ev.+type := EVID_GOLDCHECK;
  
  if(Pid)
    Pid.SendEvent(ev);
  endif

endfunction

//-----------------------------------------------------------------------------

function CountGoldInCont(Cont)

  var Amt := 0;

  foreach Item in EnumerateItemsInContainer(Cont)
    if(item.objtype == UOBJ_GOLDCOIN)
      Amt := Amt + item.amount;
    endif
  endforeach

  return Amt;
  
endfunction

//-----------------------------------------------------------------------------

function SubtractGold(cont,amount)

  amount := CInt(amount);

  if(amount <= 0)
    return 1;
  endif

  var Fin   := 0;
  var ToDes := {};
  var ToSub := 0;

  foreach Item in EnumerateItemsInContainer(cont)
    if(item.objtype == UOBJ_GOLDCOIN)
      if(item.amount <= amount)
        amount := amount - item.amount;
        ToDes.append(item);
      else
        Fin   := 1;
        ToSub := item;
        break;
      endif
    endif
    
    if(amount <= 0)
      Fin := 1;
      break;
    endif
  endforeach

  if(Fin)
    foreach item in ToDes
      if(!DestroyItem(item))
        return 0;
      endif
    endforeach

    if(ToSub)
      if(!SubtractAmount(ToSub,amount))
        return 0;
      endif
    endif
    
    return 1;
  endif
 
  return 0;

endfunction

//-----------------------------------------------------------------------------

function CreateGold(cont,amount,who := 0)

  if(amount <= 0)
    return 0;
  endif

  if(amount > 5000000)
    return 0;
  endif

  var gold,ga := {};

  while(amount >= 60000)
    gold := CreateItemInCont(cont,UOBJ_GOLDCOIN,60000,who);
    if(gold)
      gold.graphic := INVIS_ITEM_GRAP;
      ga.append(gold);
      amount := amount - 60000;
    else
      DestroyItemArray(ga);
      return 0;
    endif
  endwhile

  if(amount)
    gold := CreateItemInCont(cont,UOBJ_GOLDCOIN,amount,who);
    if(!gold)
      DestroyItemArray(ga);
      return 0;
    endif
  endif

  foreach gold in ga
    gold.graphic := gold.objtype;
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------
