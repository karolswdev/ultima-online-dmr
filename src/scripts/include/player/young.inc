//-----------------------------------------------------------------------------
// include zawierajacy potrzebne funkcje dla statusu mlody
//-----------------------------------------------------------------------------
include "include/mobile/titles";
include "include/attack";
const PROP_YOUNG                := "Young";
const YOUNG_MAX_ACCOUNT_TIME    := 8640000;// 100 dni uptimeserwera.
const YOUNG_MAX_ONLINE_TIME     := 432000; // 120h online = 5d online
const YOUNG_MAX_SKILL_SUM_TENTH := 450;

//-----------------------------------------------------------------------------

function IsNewPlayer(who)

  if(GetObjProperty(who, PROP_ONLINETIMER) > 86400)
    return 0;
  endif

  return 1;

endfunction

//---------------------------------------------------------------------------------------

function IsYoung(who)

  if(IsPlayer(who))
    return CInt(GetObjProperty(who, PROP_YOUNG)) > 0;
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function SetYoung(who)

  if(!IsYoung(who))
    SetObjProperty(who, PROP_YOUNG, ReadGameClock());
    SetTitleGuild(who, "Mlod"+ggm(who, 6));
    //__NOPVP__  SetPvPStatus(who,0);
    SendSysMessage(who, "Jestes uznawan"+ggm(who, 6)+" za mloda osobe.", FONT_NORMAL, COLOR_WHITE);
    return 1;
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function DelYoung(who)

  if(!IsYoung(who))
    return 0;
  endif

  EraseObjProperty(who, PROP_YOUNG);
  SetTitleGuild(who, "");
  //__NOPVP__  SetPvPStatus(who,1);
  SendSysMessage(who, "Twoj status mlodej osoby zakonczyl sie!", FONT_NORMAL, COLOR_WHITE);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanBeYoung(who)

  if(IsAdmin(who))
    return 1;
  elseif(GetOnlineTimer(who) > YOUNG_MAX_ONLINE_TIME)
    return 0;
  elseif(GetSumSkillGE(who,50) > YOUNG_MAX_SKILL_SUM_TENTH)
    return 0;
  elseif(IsRealMurderer(who))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetSumSkillGE(who, value)
  var sum := 0;
  var skill;
    for skillid := 0 to GetMaxSkillId()
      skill :=  GetBaseSkill(who, skillid);
      if(skill>= value)
        sum := sum + skill;
      endif
    endfor

    return sum;
     
endfunction

//-----------------------------------------------------------------------------

function CheckYoung(who)

  if(IsYoung(who))

    if(!CanBeYoung(who))

      DelYoung(who);

    endif

  endif

endfunction

//---------------------------------------------------------------------------------------

function GiveYoungItems(who)
  
    var Bank :=  FindBankBox(_who);
    if(Bank)
      CreateLoot(Bank,"BankItems",STEQP_CFG);
      SendSysMessage(who, "W Twojej skrzyni bankowej pojawilo sie kilka przedmiotow.", FONT_NORMAL, COLOR_BLUE);
    endif

endfunction

//---------------------------------------------------------------------------------------
