//-----------------------------------------------------------------------------

include "include/player/basic";

//-----------------------------------------------------------------------------

const ACCSTATUS_DELAY            := 3600;
const NEW_ACCOUNT_TIME           := 1209600; // 14dni.

//-----------------------------------------------------------------------------

function GenerateAccountPassword()

  var Pass := "";
  var i;

  for(i:=1;i<=6;i+=1)
    Pass += CChr(0x61 + RandomInt(25));
  endfor

  return Pass;

endfunction

//-----------------------------------------------------------------------------
// Active account
//-----------------------------------------------------------------------------

function IsActiveAccount(acc)

  if(acc)
    if(ReadGameClock() - Acc.GetProp("LastLogin") < ACC_ACTIVE_TIME and !Acc.banned)
      if(!IsAccountFrozen(Acc))
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsNewAccount(acct)

  if(acct.GetProp(PROP_CREATE_TIME) + NEW_ACCOUNT_TIME < ReadGameClock())
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
// funkcja sprawdza, czy konto jest zdezaktywowane - w sensie wyczyszczone i zaznaczone do skasowania
//-----------------------------------------------------------------------------

function IsAccountDeactivated(Acc)

  return Acc.GetProp("NOACTIVEACCOUNT");

endfunction

//-----------------------------------------------------------------------------
// Kasowanie / banowanie
//-----------------------------------------------------------------------------

function DeleteAccount(act, Reason := "")

  var Res := error;

  if(IsAccountFrozen(act))
    Res.+errortext := "Konto jest zamrozone.";
    return Res;
  endif

  if(!Reason)
    if(act.banned and !IsAccountDeactivated(act))
      Reason := ClStr(act.GetProp("BanReason"));

      if(!Reason)
        Reason := "Konto zostalo zbanowane (brak powodu)";
      endif
    else
      Reason := "nieaktywnosc";
    endif
  endif

  var i;

  for(i:=1; i<=5; i+=1)
    if(act.GetCharacter(i))
      DeleteChar(act, i);
    endif
  endfor

  var actdel := act.delete();

  if(!actdel)
    act.ban();
    act.disable();
    act.EraseProp("TIMEDACCOUNT");
    act.SetProp("NOACTIVEACCOUNT", 1);

    if(actdel.errortext)
      actdel := actdel.errortext;
    endif
    LogNotice("[DeleteAccount] ERR: Konto "+act.name+" NIE zostalo skasowane za "+Reason+" ("+actdel+").");

    Res.+errortext := "Nie udalo sie skasowac konta, zostalo zdezaktywowane.";
    return Res;
  endif

  LogDeleteAccount(act.name, Reason);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function LogDeleteAccount(actname, Reason)

  LogNotice("[DeleteAccount] Konto "+actname+" zostalo skasowane za "+Reason);

  var Parms := array(actname, Reason, PolCore().systime);
  AppendToFile(ACCS_LOG, array(Pack(Parms)));

endfunction

//-----------------------------------------------------------------------------

function BanAccount(Acc, Reason, Time, bywho := 0)

  if(bywho.serial)
    Acc.SetProp("WhoBan",bywho.serial);
  endif

  Acc.EraseProp("TIMEDACCOUNT");
  Acc.SetProp("BanReason",Reason);
  Acc.SetProp("BanTime",Time);

  var char, i;
  for(i:=1;i<=5;i:=i+1)
    char := Acc.GetCharacter(i);
    if(IsLoggedIn(char))
      InstantDisconnect(char);
    endif
  endfor

  Acc.ban();
  Acc.disable();

endfunction

//-----------------------------------------------------------------------------
// Zamrozenie konta
//-----------------------------------------------------------------------------

function IsAccountFrozen(Acc)

  return CInt(Acc.GetProp("TIMEDACCOUNT"));

endfunction

//-----------------------------------------------------------------------------

function GetAccountFreezeTime(Acc)

  return CInt(Acc.GetProp("TIMEDACCOUNT") + ACC_TBANEXPIRE) - ReadGameClock();

endfunction

//-----------------------------------------------------------------------------

function UnFreezeAccount(Acc)

  var Res := error;

  if(!IsAccountFrozen(Acc))
    Res.+errortext := "Konto o nazwie '"+Acc.name+"' NIE jest zamrozone.";
    return Res;
  endif

  Acc.unban();
  Acc.enable();
  Acc.EraseProp("TIMEDACCOUNT");
  Acc.SetProp("LastLogin", ReadGameClock());

  return 1;

endfunction

//-----------------------------------------------------------------------------

function ActivateAccount(Acc)

  if(IsAccountDeactivated(Acc))

    Acc.EraseProp("NOACTIVEACCOUNT");
    Acc.SetProp("LastLogin",ReadGameClock());
    Acc.unban();
    Acc.enable();

  else

    var Res := error;
    Res.+errortext := "Konto nie zostalo zdezaktywowane.";
    return Res;

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function FreezeAccount(Acc)

  var Res := error;

  if(IsAccountFrozen(Acc))
    Res.+errortext := "Konto o nazwie '"+Acc.name+"' jest juz zamrozone.";
    return Res;
  endif

  if(Acc.banned)
    Res.+errortext := "Konto o nazwie '"+Acc.name+"' jest zbanowane!";
    return Res;
  endif

  if(IsAccountDeactivated(Acc))
    Res.+errortext := "Konto o nazwie '"+Acc.name+"' zostalo zdezaktywowane.";
    return Res;
  endif

  if(GetOldestCharTime(Acc) < ACC_MINCHAR_TIME)
    Res.+errortext := "Aby moc zamrozic konto, musisz miec jedna postac na koncie, ktora istnieje chociaz "+RealTime(ACC_MINCHAR_TIME)+" (online).";
    return Res;
  endif

  Acc.SetProp("TIMEDACCOUNT",ReadGameClock());
  Acc.ban();
  Acc.disable();

  var i;

  for(i:=1; i<=5; i:=i+1)
    var Char := Acc.GetCharacter(i);
    if(Char and IsLoggedIn(Char))
      SendSysMessage(Char,"Twoje konto zostalo zamrozone.",FONT_NORMAL,COLOR_YELLOW);
      InstantDisconnect(char);
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------
