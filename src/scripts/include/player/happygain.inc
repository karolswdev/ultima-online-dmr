//--------------------------------------------------------------------------------
// include uzywany w attrcore.src - tam przyspieszany jest bezposrednio raw gain o happyMultipier
// + komenda do uruchomienia happy gaina na czas z attr.cfg GainTime (w sekundach)
//--------------------------------------------------------------------------------

include "include/const/cfgs";
include "include/const/attributes";



//--------------------------------------------------------------------------------

function IsHappyGain(who)

  return CInt(GetObjProperty(who, "HappyGain")) > ReadGameClock();

endfunction

//--------------------------------------------------------------------------------

function SetHappyGain(who)

  var Res := error;

  if(IsHappyGain(who))
    Res.+errortext := "Przyspieszony przyrost juz trwa.";
    return Res;
  endif
  
  var AttrCfg    := ReadConfigFile(ATTR_CFG);
  var MinStatSum := CInt(AttrCfg["HappyGain"].MinStatSum) * 10;

  if(CInt(GetObjProperty(who, PROP_STATSUM)) < MinStatSum)
    Res.+errortext := "Jeszcze nie czas dla Ciebie na uzywanie przyspieszonego przyrostu.";
    return Res;
  endif

  var CurCumul      := CheckHappyGainCumulation(who);
  var LastHappyGain := who.acct.GetProp("LastHappyGain");
  var curDate       := CUnixTime(0, RLT_SDATE);

  if(LastHappyGain)

    if(LastHappyGain == curDate or (CurCumul and CurCumul[2] > 0))
      if(CurCumul and CurCumul[2] > 0)
        CurCumul[2] := CurCumul[2] - 1;
        who.acct.SetProp("CumulHappyGain", CurCumul);
      elseif(!IsOnDMRTest())
        Res.+errortext := "Jeszcze nie minal wystarczajacy czas od ostatniego przyspieszonego przyrostu.";
        return Res;
      endif
    endif

  endif

  SetObjProperty(who, "HappyGain", ReadGameClock() + GetHappyGainTime());
  who.acct.SetProp("LastHappyGain", curDate);

  if(!CurCumul)
    CheckHappyGainCumulation(who);
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetHappyGainCumulation(who)

  var CurCumul := who.acct.GetProp("CumulHappyGain");

  return CInt(CurCumul[2]);

endfunction

//--------------------------------------------------------------------------------

function CheckHappyGainCumulation(who)

  var LastDate := who.acct.GetProp("LastHappyGain");
  var CurCumul := who.acct.GetProp("CumulHappyGain");

  if(LastDate == error)
    return CurCumul;
  endif

  var CurDate  := CUnixTime(0, RLT_SDATE);
  var CurTime  := MakeUnixTimeByDate(CurDate);

  if(!CurCumul)
    CurCumul := array(LastDate, 0);
  elseif(CurCumul[1] == CurDate)
    return CurCumul;
  endif

  var LastTime := MakeUnixTimeByDate(CurCumul[1]);

  if(LastTime == error)
    return CurCumul;
  endif

  var AttrCfg  := ReadConfigFile(ATTR_CFG);  
  var Diff     := CInt((CurTime - LastTime) / CInt(AttrCfg["HappyGain"].Interval));
  var MaxCumul := CInt(AttrCfg["HappyGain"].MaxCumulation);

  CurCumul[2]  += Diff;

  if(CurCumul[2] > MaxCumul)
    CurCumul[2] := MaxCumul;
  endif

  CurCumul[1] := CurDate;

  who.acct.SetProp("CumulHappyGain", CurCumul);

  return CurCumul;

endfunction

//--------------------------------------------------------------------------------

function GetHappyGainTime()

  var AttrCfg := ReadConfigFile(ATTR_CFG);
  
  return CInt(AttrCfg["HappyGain"].GainTime);

endfunction

//--------------------------------------------------------------------------------

function CheckHappyGainTime(who)

  if(GetObjProperty(who, "HappyGain") != error)
    if(GetObjProperty(who, "HappyGain") > ReadGameClock())
      return 1;
    else
      SendSysMessage(who,"Koniec szybkiego przyrostu statystyk.",FONT_NORMAL, COLOR_BLUE);
      EraseObjProperty(who,"HappyGain");
    endif
   endif
  return 0;
  
endfunction
