//-----------------------------------------------------------------------------

function CreateBankAccount(who)

  var Box := FindBankBox(who);
  
  if(!Box)
    return 0;
  endif
  
  if(GetObjProperty(Box, "AccountAmount") == error)
    SetObjProperty(Box, "AccountAmount", 0);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function WithdrawFromBankAccount(who, Amt, Type, Cost := 0)

  var Box := FindBankBox(who);
  
  if(!Box)
    return RetError("Nie posiadasz skrzyni bankowej!");
  endif
  
  var AccountAmt := GetObjProperty(Box, "AccountAmount");
  
  if(AccountAmt == error)
    return RetError("Przykro mi, ale nie posiadasz konta bankowego. Jak chcesz to mo¿esz zawsze za³o¿yæ nowe!");
  endif

  if(AccountAmt < Amt+Cost)
    return RetError("Nie masz tyle z³ota na koncie.");
  endif

  if(Type == UOBJ_GOLDCOIN)
    AccountAmt -= Amt+Cost;
    
    if(!CreateGold(__IfElse(Amt <= 60000, who.backpack, Box), Amt, who))
      return RetError("Nie moge daæ Ci tyle z³ota. Nie masz miejsca by je pomieœciæ.");
    endif
    
  elseif(Type == UOBJ_CHEQUE)
  
    AccountAmt -= Amt+Cost;
    
    if(!CreateChequeInPack(who, Amt))
      return RetError("Nie mogê daæ Ci teraz czeku. Plecak masz pe³ny?");
    endif
    
  else
    return RetError("Hm.. tak z³ota nie wyp³acisz.");
  endif
  
  SetObjProperty(Box, "AccountAmount", AccountAmt);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function PutToBankAccount(who, Item)

  var Box := FindBankBox(who);
  
  if(!Box)
    return RetError("Nie posiadasz skrzyni bankowej!");
  endif
  
  if(Item.objtype != UOBJ_GOLDCOIN and Item.objtype != UOBJ_CHEQUE)
    if(!GetObjProperty(Item, "Value"))
        return RetError("Hm.. tego na konto nie w³o¿ysz.");
    endif
  endif

  var auctionItem := 0;

  var Amt := GetObjProperty(Box, "AccountAmount");
  var valueAmt := GetObjProperty(Item, "Value");

  if(valueAmt != error)
    auctionItem := 1;
  endif
  
  if(Amt == error)
    return RetError("Przykro mi, ale nie posiadasz konta bankowego. Jak chcesz to mo¿esz zawsze za³o¿yæ nowe!");
  endif
  
  var AddAmt := 0;

  if(Item.objtype == UOBJ_GOLDCOIN)
    AddAmt := Item.amount;
    Amt += AddAmt;
    
    if(!DestroyItem(Item))
      return RetError("Nie moge wzi¹æ tego z³ota.");
    endif
    
  elseif(Item.objtype == UOBJ_CHEQUE)
  
    AddAmt := GetChequeVal(Item);
    Amt += AddAmt;
    
    if(!DestroyItem(Item))
      return RetError("Nie moge wzi¹æ tego czeku.");
    endif
    
  elseif(auctionItem)
    AddAmt := CInt(GetObjProperty(Item, "Value"));
    Amt += AddAmt;
    if(!DestroyItem(Item))
	return RetError("Nie moge wziac tego czeku...!");
    endif
  else
    return RetError("Hm.. tego na konto nie w³o¿ysz.");
  endif
  
  SetObjProperty(Box, "AccountAmount", Amt);
  return AddAmt;

endfunction

//-----------------------------------------------------------------------------

function GetBankStats(BankBox)

  if(IsPlayer(BankBox))
    BankBox := FindBankBox(BankBox);
  endif

  if(!BankBox.isa(POLCLASS_CONTAINER))
    return 0;
  endif

  var Stats := struct;
  Stats.+GoldAmount := 0;
  Stats.+ChequeAmount := 0;
  Stats.+AccountAmount := GetBankAccountAmount(BankBox);

  foreach item in EnumerateItemsInContainer(BankBox)
    if(item.objtype == UOBJ_GOLDCOIN)
      Stats.GoldAmount += item.amount;
    elseif(item.objtype == UOBJ_CHEQUE)
      Stats.ChequeAmount += GetChequeVal(Item);
    endif
  endforeach
  
  Stats.+SumAmount := Stats.GoldAmount + Stats.ChequeAmount + Stats.AccountAmount;
  
  return Stats;

endfunction

//-----------------------------------------------------------------------------
