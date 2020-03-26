//-----------------------------------------------------------------------------
// Modofied: 2005-06-02
//-----------------------------------------------------------------------------

include "guildsdata";
include "guilds";
include "include/logerror";
include "include/bits";
include "include/tempmods";
include "include/accounts";
include "include/reputation/murderer";

include ":npcguilds:shared";
include ":wearwolf:common";

//-----------------------------------------------------------------------------

const NGD_CTRL_PID           := "NpcGuildsCtrlPid";

//-----------------------------------------------------------------------------
// Player Meet Guild Requirements Flags
//-----------------------------------------------------------------------------

const PMGRF_NOCHECK_STATUS   := 0x00000001;
const PMGRF_NOCHECK_CRIM     := 0x00000002;
const PMGRF_LIGHT_CHKST      := 0x00000004;

//-----------------------------------------------------------------------------

function CheckPlayerGuildStatus(who, byref Guild)

  var GdStatus := Guild.GetProp(GDPROP_STATUS);

  if(CanMod(who, MOD_STATUS) == 0)
    var OldAmt := GetModOldAmount(who, MOD_STATUS);

    case(GetModAmount(who, MOD_STATUS))
      1:
      2:        if(GdStatus == NGDSTATUS_CHAOS)
                  return 2;
                elseif(GdStatus == NGDSTATUS_ORDER or GdStatus == NGDSTATUS_NEUTRAL)
                  if(OldAmt and OldAmt[2])
                    return 2;
                  endif
                endif

      3:        if(GdStatus == NGDSTATUS_ORDER)
                  return 2;
                elseif(GdStatus == NGDSTATUS_NEUTRAL and (Guild.GetProp(GDPROP_TYPE) != 6 or !IsWearWolfMorphed(who)))
                  return 2;
                elseif(GdStatus == NGDSTATUS_CHAOS)
                  if(OldAmt and !OldAmt[2])
                    return 2;
                  endif
                endif
    endcase
  else
    case(GdStatus)
      NGDSTATUS_NEUTRAL: 
      NGDSTATUS_ORDER:     if(who.murderer)
                             if(GetMurdersCount(who) > NGD_ORDER_MURD_MAX)
                               return 0;
                             else
                               return 2;
                             endif
                           endif

      NGDSTATUS_CHAOS:     if(!who.murderer)
                             return 0;
                           endif
    endcase
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IsPlayerMeetGuildReqs(who, byref Guild, cFlags := 0)

  var Err := error;

  case(Guild.GetProp(GDPROP_TYPE))
    NGDTYPE_WARRIORS:        if(!IsCharClass(who, CLASS_WARRIOR))
                               Err.errortext := "Nie wyglada na to bys byl"+ggm(who,4)+" wojownikiem.";
                               return Err;
                             endif

    NGDTYPE_ARCHERS:         if(!IsCharClass(who, CLASS_ARCHER))
                               Err.errortext := "Nie wyglada na to bys byl"+ggm(who,4)+" lucznikiem.";
                               return Err;
                             endif

    NGDTYPE_MAGES:           if(!IsCharClass(who, CLASS_MAGE))
                               Err.errortext := "Nie wyglada na to bys byl"+ggm(who,4)+" magiem.";
                               return Err;
                             endif

    NGDTYPE_THIEFS:          if(!IsThief(who))
                               Err.errortext := "Pheh, wynos sie stad, zaden z Ciebie zlodziej.";
                               return Err;
                             endif
     
    NGDTYPE_WEARWOLFS:       if(!IsWearWolf(who))
                               Err.errortext := "Nie wyczuwam w tobie krwi wilkolaka.";
                               return Err;
                             endif

    NGDTYPE_VAMPIRES:        if(!IsVampire(who))
                               Err.errortext := "Pff, won stad zanim wyssam z Ciebie krew!";
                               return Err;
                             endif

    NGDTYPE_NECROS:          if(!IsNecro(who))
                               Err.errortext := "Krew w twych zylach jest nadal ciepla...";
                               return Err;
                             endif

    NGDTYPE_DRUIDS:          if(!IsDruid(who))
                               Err.errortext := "Nie wyglada na to bys zjednoczyl sie z natura...";
                               return Err;
                             endif
  endcase

  if(!(cFlags & PMGRF_NOCHECK_STATUS))
    if(CanMod(who, MOD_STATUS) == 0)
      Err.errortext := "Nie jestem pewny co do twojego stanu, przyjdz pozniej.";
      return Err;
    endif

    if(cFlags & PMGRF_LIGHT_CHKST)
      if(!CheckPlayerGuildStatus(who, Guild))
        Err.errortext := "Twoj status nie bedzie tolerowany.";
        return Err;
      endif
    else
      case(Guild.GetProp(GDPROP_STATUS))
        NGDSTATUS_ORDER:     if(who.murderer)
                               Err.errortext := "Morderca ?! Wynos sie stad.";
                               return Err;
                             elseif(who.criminal and !(cFlags & PMGRF_NOCHECK_CRIM))
                               Err.errortext := "Kryminalista? czego tu szukasz?";
                               return Err;
                             endif

        NGDSTATUS_CHAOS:     if(!who.murderer)
                               Err.errortext := "Pff, mueh, won stad zanim dostaniesz po lbie! morderca...";
                               return Err;
                             elseif(GetMurdersCount(who) < NGD_TYPE_MURD_COUNT)
                               Err.errortext := "Pheh, slaby z Ciebie morderca, za duzo osob to nie zabil"+ggm(who,3)+".";
                               return Err;
                             endif

        NGDSTATUS_NEUTRAL:   if(who.murderer)
                               Err.errortext := "Zaden morderca nie bedzie istniec w naszej Gildii!";
                               return Err;
                             endif
      endcase
    endif
  endif

  foreach SkId in (Guild.GetProp(GDPROP_MAINSKIDS))
    if(GetBaseSkill(who, SkID) >= NGD_SK_REQ_VAL)
      return 1;
    endif
  endforeach

  Err.errortext := "Przykro mi, ale nie pasujesz do naszej Gildii, nie umiesz nic z umiejetnosci, ktore wymagamy.";
  return Err;

endfunction

//-----------------------------------------------------------------------------

function CheckNpcGuildMember(Player, GuildId, byref Guild)

  var CheckFlags := PMGRF_NOCHECK_CRIM + PMGRF_LIGHT_CHKST;

  if(CanMod(Player,MOD_STATUS) == 0)
    SetBitOn(CheckFlags, PMGRF_NOCHECK_STATUS);
  endif

  var Res := IsPlayerMeetGuildReqs(Player, Guild, CheckFlags);

  if(!Res)
    SendBad(Player, "Przestal"+ggm(Player,3)+" spelniac wymagania gildii miejskiej '"+Guild.GetProp(GDPROP_NAME)+"'");
    if(Res.errortext)
      SendBad(Player, "Powod: "+Res.errortext);
    endif
    return 0;
  endif

  var PlayerData := GetObjProperty(Player, PROP_NGDDATA);

  if(!IsActiveAccount(Player.acct) or (GetObjProperty(Player, PROP_LASTLOG) + NGD_LOSE_REP_LLTIME < ReadGameClock()))

    if(!(PlayerData.Guilds[GuildId].Flags & NGPF_NOT_ACTIVE))
      SetBitOn(PlayerData.Guilds[GuildId].Flags, NGPF_NOT_ACTIVE);
      SetObjProperty(Player, PROP_NGDDATA, PlayerData);
    endif

  elseif(PlayerData.Guilds[GuildId].Flags & NGPF_NOT_ACTIVE)

    PlayerData.Guilds[GuildId].LastVisit := ReadGameClock() - NGD_LOSE_REP_LLTIME;
    SetBitOff(PlayerData.Guilds[GuildId].Flags, NGPF_NOT_ACTIVE);
    SetObjProperty(Player, PROP_NGDDATA, PlayerData);

  else
  
    if(!PlayerData.Guilds[GuildId].LastVisit)
      PlayerData.Guilds[GuildId].LastVisit := ReadGameClock();
      SetObjProperty(Player, PROP_NGDDATA, PlayerData);
    endif

    if(PlayerData.Guilds[GuildId].LastVisit + NGD_LOSE_REP_TIME < ReadGameClock())
      if(!PlayerData.Guilds[GuildId].LastRepLose)
        PlayerData.Guilds[GuildId].LastRepLose := ReadGameClock();
        SetObjProperty(Player, PROP_NGDDATA, PlayerData);
      endif
      
      if(PlayerData.Guilds[GuildId].LastRepLose + NGD_LOSE_REP_DELAY < ReadGameClock())
        SendBad(Player, "Czujesz, ze dawno odwiedzal"+ggm(Player,3)+" gildie miejska...");
        PlayerData.Guilds[GuildId].LastRepLose := ReadGameClock();
        if(!LoseGuildExp(Player, PlayerData, GuildId, Guild, NGD_LOSE_REP_VALUE))
          return 0;
        else
          SetObjProperty(Player, PROP_NGDDATA, PlayerData);
        endif
      endif
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
