//-----------------------------------------------------------------------------

function AddReferal(who, referer)

  var Data := GetReferalsData(referer);

  if(Data.invited.exists(who.acctname))
    return error;
  endif

  Data.invited[who.acctname] := GetReferalInfo(who);
  referer.acct.SetProp("Referals", Data);
  who.acct.SetProp("ReferalInvited", GetReferalInfo(referer));

  return 1;

endfunction

//-----------------------------------------------------------------------------

function EraseReferal(referer, who)

  if(!referer)
    return 0;
  endif

  var Data := GetReferalsData(referer);

  if(!Data.invited.exists(who.acctname))
    return error;
  endif

  Data.invited.erase(who.acctname);
  referer.acct.SetProp("Referals", Data);
  who.acct.EraseProp("ReferalInvited");

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetRewardsForDegree(Degree)

  var Cfg := ReadConfigFile(REF_REWARDS_CFG);
  var Names := GetConfigStringArray(Cfg["Degree"+Degree], "Reward");

  return Names;

endfunction

//-----------------------------------------------------------------------------

function GetReferalRewardData(rewName)

  var Cfg := ReadConfigFile(REF_REWARDS_CFG);

  var Data := struct;
  var Obj := SplitWords(Cfg[rewName].Item);

  Data.+objtype := GetItemObjType(Obj[1]);
  Data.+amount := CInt(Obj[2]);

  return Data;

endfunction

//-----------------------------------------------------------------------------

function CreateReward(referer, invAccName, Degree, rewName)

  var Refs := GetReferalsData(referer);
  var Acc := FindAccount(invAccName);
  
  if(!Refs.invited.exists(invAccName) or !Acc)
    SendSysMessage(referer, "Nie mozesz odebrac nagrody.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(Refs.invited[invAccName].reward >= Degree)
    SendSysMessage(referer, "Juz wzial"+ggm(referer,3)+" nagrode!", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Data := GetReferalRewardData(rewName);

  if(!Data.objtype or !Data.amount)
    SendSysMessage(referer, "Nagroda nie jest mozliwa do odebrania w tym momencie.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  var Item;

  if(Data.objtype == UOBJ_GOLDCOIN)
    Item := CreateGold(FindBankBox(referer), Data.amount, referer);
  else
    Item := CreateItemInCont(FindStoreBox(referer), Data.objtype, Data.amount);
  endif

  if(!Item)
    SendSysMessage(referer, "Nie masz miejsca w plecaku!", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(Refs.numActive == error)
    Refs.+numActive := 0;
  endif

  if(Degree == 1)
    Refs.numActive += 1;
    Refs.numLeft += 1;
  endif

  if(Refs.invited[invAccName].reward == error)
    Refs.invited[invAccName].+reward := 0;
  endif

  Refs.invited[invAccName].reward := Degree;
  referer.acct.SetProp("Referals", Refs);
  
  SendSysMessage(referer, "Odebral"+ggm(referer,3)+" nagrode w postaci "+Data.amount+" "+GetDescForItem(Data.objtype)+".", FONT_NORMAL, COLOR_WHITE);

  if(Data.objtype == UOBJ_GOLDCOIN)
    SendSysMessage(referer, "Nagroda znajduje sie w Twojej skrzyni bankowej.", FONT_NORMAL, COLOR_GREEN);
  else
    SendSysMessage(referer, "Nagroda znajduje sie w Twoim magazynie.", FONT_NORMAL, COLOR_GREEN);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsAlreadyRefered(who, referer)

  var Data := GetReferalsData(referer);

  if(Data.invited.exists(who.acctname))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetReferalInviteData(who)

  var Data := who.acct.GetProp("ReferalInvited");

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetReferalInfo(who)

  return struct{"serial" := who.serial, "name" := GetRealName(who), "acctname" := who.acctname, "systime" := PolCore().systime, "gametime" := ReadGameClock()};

endfunction

//-----------------------------------------------------------------------------

function SetReferals(who, byref Refs)

  who.acct.SetProp("Referals", Refs);
  
endfunction

//-----------------------------------------------------------------------------

function GetReferalsData(who)

  var Data := who.acct.GetProp("Referals");

  if(!Data)
    Data := GetObjProperty(who, "Referals");

    if(Data)
      EraseObjProperty(who, "Referals");
      who.acct.SetProp("Referals", Data);
    endif
  endif

  if(!Data or !IsVarType(Data, VAR_STRUCT))
    Data := struct;
    Data.+invited := dictionary;
    Data.+numActive := 0;
    Data.+numLeft := 0;
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------
