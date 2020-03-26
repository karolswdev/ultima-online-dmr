//-----------------------------------------------------------------------------

include "include/const/cfgs";
include "include/mobile/titles";

include ":party:party";

//-----------------------------------------------------------------------------

const KARMA_PVP_TIME := 43200;

//-----------------------------------------------------------------------------

//=============================================================================
//-----------------------------------------------------------------------------
// Noto
//-----------------------------------------------------------------------------
//=============================================================================

function AdjustNoto(attacker, defender)

  if(IsTamed(attacker))
    attacker := GetMaster(attacker);
  endif

  if(!IsPlayer(attacker) or attacker.cmdlevel)
    return 0;
  endif
  
  var NpcTemp;
  
  if(defender.isa(POLCLASS_CORPSE))
    NpcTemp := GetObjProperty(defender, PROP_NPCTEMP);
  elseif(defender.isa(POLCLASS_NPC))
    NpcTemp := defender.npctemplate;
  elseif(IsPlayer(defender))
    return AdjustPlayerNoto(attacker, defender);
  endif
  
  if(!NpcTemp)
    return 0;
  endif

  var cfgElem    := GetNpcConfig(NpcTemp);

  var WhoKarma   := GetKarma(attacker);
  var WhoFame    := GetFame(attacker);
  
  var KarmaGain  := CInt(cfgElem.KarmaGain);
  var FameGain   := CInt(cfgElem.FameGain);
  
  var KLvl       := GetKarmaLvl(WhoKarma);
  var FLvl       := GetFameLvl(WhoFame);

  KarmaGain      := ModerateGain(KarmaGain,KLvl);
  FameGain       := ModerateGain(FameGain,FLvl);

  if(!SplitNoto(attacker, KarmaGain, FameGain))
    SetFame(attacker, WhoFame + FameGain);
    SetKarma(attacker, WhoKarma + KarmaGain);
    RefreshNotoTitle(attacker);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function ModerateGain(Gain,Lvl)

  var Perc := 100;

  if((Gain > 0 and Lvl > 0) or (Gain < 0 and Lvl < 0))
    Perc := Perc - (Abs(Lvl) * 20);
  endif

  if(Perc < 5)
    Perc := 5;
  endif
  
  Perc := Perc * 0.01;
  Gain := CInt((Gain * 5) * Perc);
  
  return Gain;
  
endfunction

//-----------------------------------------------------------------------------

function GetKarma(who)

  return CInt(GetObjProperty(who, "Karma"));

endfunction

//-----------------------------------------------------------------------------

function GetFame(who)

  return CInt(GetObjProperty(who, "Fame"));

endfunction

//-----------------------------------------------------------------------------

function SetKarma(who,NewKarma)

  if(IsBlockedKarma(who))
    if(NewKarma - GetKarma(who) > 0)
      return;
    endif
  endif
  
  SendGainMsg(who,"Karmy",NewKarma - GetKarma(who));
  SetObjProperty(who,"Karma",CInt(NewKarma));

endfunction

//-----------------------------------------------------------------------------

function SetFame(who,NewFame)

  SendGainMsg(who,"Slawy",NewFame - GetFame(who));
  SetObjProperty(who,"Fame",CInt(NewFame));

endfunction

//-----------------------------------------------------------------------------

function GetFameLvl(fame)

  if(Fame.isa(POLCLASS_MOBILE))
    Fame := GetFame(Fame);
  endif

  if(Fame > 0 and Fame < 5000)
    return 1;
  elseif(Fame >= 5000 and Fame < 15000)
    return 2;
  elseif(Fame >= 15000 and Fame < 75000)
    return 3;
  elseif(Fame >= 75000 and Fame < 150000)
    return 4;
  elseif(Fame >= 150000)
    return 5;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetKarmaLvl(karma)

  if(karma.isa(POLCLASS_MOBILE))
    karma := GetKarma(karma);
  endif

  if(Karma < -150000)
    return -5;
  elseif(Karma >= -150000 and Karma < -75000)
    return -4;
  elseif(Karma >= -75000  and Karma < -30000)
    return -3;
  elseif(Karma >= -30000  and Karma < -10000)
    return -2;
  elseif(Karma >= -10000  and Karma < -1500)
    return -1;
  elseif(Karma >= -1500 and Karma < 1500)
    return 0;
  elseif(Karma >= 1500 and  Karma < 10000)
    return 1;
  elseif(Karma >= 10000 and Karma < 30000)
    return 2;
  elseif(Karma >= 30000 and Karma < 75000)
    return 3;
  elseif(Karma >= 75000 and Karma < 150000)
    return 4;
  elseif(Karma >= 150000)
    return 5;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function RefreshNotoTitle(who, Force := 0)

  if(who.cmdlevel or !IsPlayer(who))
    return;
  endif

  var TitlesCfg := ReadConfigFile(NOTO_CFG);

  var Karma  := GetKarma(who);
  var Fame   := GetFame(who);
  
  var KLvl   := GetKarmaLvl(Karma);
  var FLvl   := GetFameLvl(Fame);

  var Titles := SplitWords(GetConfigString(TitlesCfg["NotoTitles"+who.gender],"Title_"+CInt(KLvl+5)));
  var Title  := Titles[FLvl];
  
  if(Title == "None" or !Title)
    Title := "";
  else
    Title := Title;
  endif
  
  while(Title["_"])
    Title["_"] := " ";
  endwhile
  
  case(FLvl)
    3:
    4:  case(who.gender)
          0: if(Title)
               Title += " Sir";
             else
               Title := "Sir";
             endif

          1: if(Title)
               Title += " Dama";
             else
               Title := "Dama";
             endif

        endcase

    5:  case(who.gender)
          0: if(Title)
               Title += " Lord";
             else
               Title := "Lord";
             endif

          1: if(Title)
               Title += " Lady";
             else
               Title := "Lady";
             endif

        endcase
  endcase
  
  if(Title)
    if(GetTitlePrefix(who) != Title or Force)
      SetTitlePrefix(who, Title);
      SendSysMessage(who, "Jestes teraz znan"+ggm(who,6)+" jako "+Title+" "+who.name,FONT_NORMAL,COLOR_BLUE);
    endif
  else
    SetTitlePrefix(who, "");
  endif
  
endfunction

//-----------------------------------------------------------------------------

function SendGainMsg(who, type, amount)

  var msgtext := "";
  
  if(amount < 0)
    msgtext := "Stracil"+ggm(who,3)+" ";
    amount := Abs(amount);
  else
    msgtext := "Zdobyl"+ggm(who,3)+" ";
  endif
  
  if(amount > 200)
    msgtext := msgtext + "wielka ilosc ";
  elseif(amount > 150)
    msgtext := msgtext + "spora ilosc ";
  elseif(amount > 125)
    msgtext := msgtext + "duza ilosc ";
  elseif(amount > 75)
    msgtext := msgtext + "troche ";
  elseif(amount > 30)
    msgtext := msgtext + "mala ilosc ";
  else
    return 0;
  endif
  
  msgtext := msgtext + type + ".";
  SendSysMessage(who, msgtext,FONT_NORMAL,COLOR_GREEN);
  
endfunction

//-----------------------------------------------------------------------------

function IsBlockedKarma(who)

  return CInt(GetObjProperty(who,"BlockedKarma"));

endfunction

//-----------------------------------------------------------------------------

function FameLost(who,Perc := 2)

  var Fame := GetFame(who);
  Fame     := Fame - CInt(Fame * (Perc * 0.01));
  SetFame(who,Fame);
  RefreshNotoTitle(who);

endfunction

//-----------------------------------------------------------------------------

function KarmaLost(who,Perc := 2)

  var Karma := GetKarma(who);
  Karma     := Karma - CInt(Karma * (Perc * 0.01));
  SetKarma(who,Karma);
  RefreshNotoTitle(who);

endfunction

//-----------------------------------------------------------------------------

function AdjustPlayerNoto(attacker,defender)

  var KarmaPers := GetObjProperty(attacker,"KarmaPersons");
  if(!KarmaPers)
    KarmaPers := dictionary;
  endif
  
  var TempPers := dictionary;
  foreach Pers in KarmaPers
    if(Pers + KARMA_PVP_TIME > ReadGameClock())
      TempPers[_Pers_iter] := Pers;
    endif
  endforeach
  
  KarmaPers := TempPers;
  
  if(CInt(KarmaPers[defender.serial]) + KARMA_PVP_TIME > ReadGameClock())
    return 0;
  endif
  
  if(defender.murderer or defender.criminal)
    SetKarma(attacker,GetKarma(attacker) + Abs(GetKarmaLvl(GetKarma(defender))) * 40);
  else
    SetKarma(attacker,GetKarma(attacker) - Abs(GetKarmaLvl(GetKarma(defender))) * 40);
  endif
  
  KarmaPers[defender.serial] := ReadGameClock();
  SetObjProperty(attacker,"KarmaPersons",KarmaPers);

  return 1;

endfunction

//-----------------------------------------------------------------------------
