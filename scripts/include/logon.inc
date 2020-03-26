//--------------------------------------------------------------------------------

include "include/misc";
include "include/events";
include "include/tempmods";
include "include/finding";
include "include/staff";
include "include/move";
include "include/buffs";
include "include/cmds/cmds";
include "include/gumpboxes";
include "include/player/hooks";

include ":carpentry:bed";
include ":mail:mail";
include ":magic:magicatt";
include ":reslogon:petsecure";

include ":eventcore:logged";
include ":eventcore:basic";

//================================================================================
//--------------------------------------------------------------------------------
// Logon
//--------------------------------------------------------------------------------
//================================================================================

function CheckLogOn(who)

  StartDeathControl(who);

  var acct := who.acct;

  if(acct.GetProp("RestIP"))
    if(!VerifyIP(who))
      InstantDisconnect(who);
      return 0;
    endif
  endif
  
  

  
  if(!CheckAccount(who, acct))
    InstantDisconnect(who);
    return 0;
  endif

  StartPlayerHooks(who);
  SetObjProperty(who,"#LoggedIn", 1);
  start_script(":character:chclient", array(who));
  
  if(IsAccountFrozen(acct))
    acct.EraseProp("TIMEDACCOUNT");
  endif
  
  SetObjProperty(who, PROP_LOGONTIME, ReadGameClock());

  CheckLogonLocation(who);
  SendLogonWPackets(who);
  SendWeatherUpdatePacket(who);
  CheckLogonMods(who);

  if(who.dead)
    start_script("::deadctl",who);
  endif

  var Mount := GetEquipmentByLayer(who,LAYER_MOUNT);
  if(Mount)
    var Animal := FindNpcBySerial(GetObjProperty(Mount,"AnimalSerial"));
    if(Animal)
      SendChEvent(Animal, who, EVID_MOUNT_LOGON);
    endif
  endif

  UnSecurePets(who);


  acct.set_uo_expansion("ML");
  CheckSleepLogon(who);

  UpdateLight(who);
  CheckBuffsOnLogon(who);

  if(HasUnreadMails(who))
    SendSysMessage(who, "Masz nieprzeczytane listy w skrzynce pocztowej.", FONT_NORMAL, COLOR_WHITE);
  endif
  

  // niech nie wywala okna zaraz po utworzeniu postaci, bo jak gosc bedzie mial zle imie to zaraz wywali sto okien.
  
  CheckLogonChanges(who);
  CheckAutoEventPlayerLogin(who);

  return 1;
  
endfunction

//--------------------------------------------------------------------------------

function CheckLogonLocation(who)

  if(!who.isa(POLCLASS_MOBILE))
    return;
  endif

  var Cfg := ReadConfigFile(LOGONLOCS_CFG);
  var Range;

  foreach LocId in GetConfigStringKeys(Cfg)
    if(Cfg[LocId].Realm == who.realm)

      if(Cfg[LocId].murderer and !who.murderer)
        continue;
      elseif(Cfg[LocId].OneTime)
        if(GetObjProperty(who, "LocMove_"+LocId))
          continue;
        endif
      endif

      Range := SplitWords(Cfg[LocId].Range);
      MakeIntArray(Range);
      if(who.x >= Range[1] and who.y >= Range[2] and who.x <= Range[3] and who.y <= Range[4])
        var mLocs := GetConfigStringArray(Cfg[LocId], "MoveLoc");
        var mLoc;

        if(mLocs.size() == 1)
          mLoc := mLocs[1];
        else
          mLoc := mLocs[RandomInt(mLocs.size())+1];
        endif

        if(!mLoc)
          mLoc := mLocs[1];
        endif

        mLoc := SplitWords(mLoc); 
        MakeIntArray(mLoc);

        if(mLoc.size() < 3)
          mLoc[3] := GetWorldHeight(mLoc[1], mLoc[2], who.realm);
        endif

        MoveObject(who, mLoc[1], mLoc[2], mLoc[3], who.realm, MOVECHAR_FORCELOCATION);

        if(Cfg[LocId].OneTime)
          SetObjProperty(who, "LocMove_"+LocId, ReadGameClock());
        endif

        var Msgs := GetConfigStringArray(Cfg[LocId], "Message");

        if(Msgs.size())
          foreach Msg in Msgs
            SendSysMessage(who, Msg, FONT_NORMAL, COLOR_WHITE);
          endforeach
        endif

        return;
      endif
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------

function CheckAccount(who, acct)

  var i,nr := 0;
  for(i:=1;i<=5;i:=i+1)
    if(acct.GetCharacter(i))
      nr := nr + 1;
    endif
  endfor

  if(nr > MAX_CHAR_ON_ACC)
    SendSysMessage(who,"Mozna miec tylko "+MAX_CHAR_ON_ACC+" postacie na swoim koncie!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function VerifyIP(who)

  var acc   := who.acct;
  var ResIP := acc.GetProp("RestIP");
  var cor   := 0;

  if(ResIP.size())
    foreach IP in ResIP
      if(MatchIP(IP, who.ip))
        cor := 1;
        break;
      endif
    endforeach

    if(!cor)
      LogMsg(WRONGIP_LOG,who);
      return 0;
    endif
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function CheckIPs(who)

  var acct   := who.acct;
  var LastIP := acct.GetProp("LastIP");
  
  if(!LastIP)
    LastIP := dictionary;
  endif
  
  var Num := CInt(LastIP[who.ip][3]) + 1;
  LastIP[who.ip] := {who.name, PolCore().systime, Num};
  acct.SetProp("LastIP", LastIP);

  if(LastIP.size() > 40)
    if(!DelLastIPsBelowTime(LastIP, 604800, 40))
      if(!DelLastIPsBelowTime(LastIP, 259200, 40))
        DelLastIPsBelowTime(LastIP, 86400, 40);
      endif
    endif
    acct.SetProp("LastIP", LastIP);
  endif
  
  acct.SetProp("LastLogin", ReadGameClock());
  
  AppendToFile(LASTLOG_PKG + who.acctname, array(Pack(array(who.ip, GetRealName(who), PolCore().systime, 0, 0, 0))));
  
endfunction

//--------------------------------------------------------------------------------

function DelLastIPsBelowTime(byref LastIPs, oTime, Limit)

  if(LastIPs.size() > Limit)
    var oLastIP := LastIPs;
    var Time    := PolCore().systime - oTime;

    foreach Ip in oLastIP
      if(LastIPs.size() <= Limit)
        return 1;
      endif
      if(CInt(IP[2]) < Time)
        LastIPs.erase(_Ip_iter);
      endif
      sleepms(2);
    endforeach
  endif

  return (LastIPs.size() <= 40);

endfunction

//--------------------------------------------------------------------------------

function CheckLogOff(who)

  EraseObjProperty(who,"#LoggedIn");
  who.acct.SetProp("LastLogin", ReadGameClock());
  
  if(!CInt(GetObjProperty(who, PROP_LOGONTIME)))
    SetObjProperty(who, PROP_LOGONTIME, ReadGameClock());
  endif
  
  SetObjProperty(who, "LogoffTime", ReadGameClock());

  if(GetObjProperty(who,"#ChoosingRace"))
    EraseObjProperty(who,"#ChoosingRace");
    who.frozen := 0;
  endif
  
  var Mount := GetEquipmentByLayer(who,LAYER_MOUNT);
  if(Mount)
    var Animal := FindNpcBySerial(GetObjProperty(Mount,"AnimalSerial"));
    if(Animal)
      SendChEvent(Animal, who.serial, EVID_MOUNT_LOGOFF);
    endif
  endif
  
  MarkLastLogIP(who);
  StopPlayerHooks(who);
  UnSetTempFlag(who, PCF_TARGETING);
  CheckAutoEventPlayerLogoff(who);

endfunction

//--------------------------------------------------------------------------------

function MarkLastLogIP(who)

  var LastLogIp := ReadFile(LASTLOG_PKG + who.acctname);
  var Last      := LastLogIP[LastLogIP.size()];

  if(Last)
    Last    := UnPack(Last);

    if(!Last[4])
      Last[4] := PolCore().systime - CInt(Last[3]);
      LastLogIP[LastLogIP.size()] := Pack(Last);
      WriteFile(LASTLOG_PKG + who.acctname, LastLogIP);
    endif
  endif
  
endfunction

//--------------------------------------------------------------------------------

const SHORTLOGS_LOGRANGE           := 15;
const SHORTLOGS_TIMERANGE          := 10800; //3h
const SHORTLOGS_MAXLOGDURATION     := 30;

//--------------------------------------------------------------------------------

function CheckForShortLogs(who)

  if(!who.cmdlevel)
    var LastLogIp := ReadFile(LASTLOG_PKG + who.acctname);

    if(LastLogIp.size() >= SHORTLOGS_LOGRANGE)
      var i, LogIp;
      var LogStart := LastLogIp.size() - SHORTLOGS_LOGRANGE + 1;
      
      LogIp := UnPack(LastLogIp[LogStart]);

      if(CInt(LogIp[3]) > 0 and CInt(LogIp[4]) > 0)
        if(PolCore().systime - CInt(LogIp[3]) < SHORTLOGS_TIMERANGE)
          
          for(i:=LogStart; i<=LastLogIp.size(); i+=1)
            LogIp := UnPack(LastLogIp[i]);
            if(CInt(LogIp[3]) > 0 and CInt(LogIp[4]) > 0)
              if(CInt(LogIp[4]) > SHORTLOGS_MAXLOGDURATION)
                return 0;
              endif
            endif
          endfor
          return 1;

        endif
      endif
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CheckLogonChanges(who)

  var BaseVal := GetBaseSkill(who, SKILLID_ITEMID);

  if(BaseVal > 0)
    if(BaseVal > 30)
      var Level := CInt(BaseVal / 20);
      
      ShowInfoEx(who, "Umiejetnosc Identyfikacja zostala usunieta i zastapiona zwojami identyfikacji.|Posiadal"+ggm(who,3)+" umiejetnosc identyfikacji w wysokosci "+BaseVal+"%.|Otrzymujesz z tego powodu "+BaseVal+" zwojow identyfikacji z poziomem "+Level+"!", 10, 40, 530);
      
      var ItemDesc := GetItemDescriptor(0x655A);
      ItemDesc.CProps["ItemLevel"] := Level;
      CreateItemInPack(who, ItemDesc, BaseVal);
    endif

    SetBaseSkill(who, SKILLID_ITEMID, 0);
  endif

endfunction

//--------------------------------------------------------------------------------
