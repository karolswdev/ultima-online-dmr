//-----------------------------------------------------------------------------

include "include/reputation/checks";
include "include/reputation/pvparea";
include "include/events";
include "include/mobile/npcs/city_funcs";

//-----------------------------------------------------------------------------

const MP_MAX_COUNTER         := 3;
const MP_DELAY_TIME          := 600; // 10 min styknie.
const MP_MIN_LSKVAL          := 50;

//-----------------------------------------------------------------------------

const MPF_LOWER_SWORDS       := 0x00000001;
const MPF_LOWER_MACES        := 0x00000002;
const MPF_LOWER_FENCING      := 0x00000004;
const MPF_LOWER_WRESTLING    := 0x00000008;
const MPF_LOWER_ARCHERY      := 0x00000010;
const MPF_LOWER_TACTICS      := 0x00000020;
const MPF_LOWER_EVALINT      := 0x00000040;
const MPF_LOWER_ANATOMY      := 0x00000080;
const MPF_LOWER_MAGERY       := 0x00000100;
const MPF_LOWER_AXES         := 0x00000200;
const MPF_LOWER_VAMP         := 0x00000400;
const MPF_LOWER_LIKAN        := 0x00000800;
const MPF_LOWER_NECRO        := 0x00001000;
const MPF_LOWER_DRUID        := 0x00002000;
const MPF_LOWER_PALADINISM   := 0x00004000;
const MPF_LOWER_FANATICISM   := 0x00008000;
const MPF_LOWER_BARBARIAN    := 0x00010000;

//-----------------------------------------------------------------------------

function AddToMurdPnCounter(who, Victim)

  if(!IsMurdPenaltyEnabled())
    return;
  endif

  if(!CanReport(Victim, who))
    return;
  endif

  var Reports := GetMurdReports(Victim);

  if(!Reports.exists(who.serial))
    return;
  endif

  var MurdData := GetMurdData(who);
  MurdData.Counter    := MurdData.Counter + 1;
  SetObjProperty(who, PROP_MURDPNDATA, MurdData);

endfunction

//-----------------------------------------------------------------------------

function GetMurdData(who)

  var MurdData := GetObjProperty(who, PROP_MURDPNDATA);

  if(!MurdData)
    MurdData := struct;
    MurdData.+Counter    := 0;
    MurdData.+CurseMod   := 0;
    MurdData.+CurCurse   := 0;
    MurdData.+LastKill   := 0;
    MurdData.+LastVicSer := 0;
    MurdData.+Flags      := 0;
  endif

  return MurdData;

endfunction

//-----------------------------------------------------------------------------

function ApplyMurdPenalty(who, Victim, MurdData := 0)

  if(!MurdData)
    MurdData := GetMurdData(who);
  endif

  MurdData.CurseMod := CInt(MurdData.CurseMod)+1; // aby zawsze dawal spowolnienie, to tutaj zwiekszamy o 1
  MurdData.LastKill := GetObjProperty(who, PROP_ONLINETIMER);
  MurdData.LastVicSer := Victim.serial;
  SetObjProperty(who, PROP_MURDPNDATA, MurdData);

  CheckMurdPenalty(who, Victim);

endfunction

//-----------------------------------------------------------------------------

function GetMurdPnCurseModFromCnt(who)

  var Counter := GetMurdersCount(who);
  Counter := MinMax(Counter - 3, 5, 10);

  if(IsDungeon(who.x, who.y, who.realm))
    Counter := CInt(Counter / 2);
  endif

  return Counter;

endfunction

//-----------------------------------------------------------------------------

function CheckMurdPenalty(who, Victim := 0)

  if(GetObjProperty(who, "PermRed"))
	who.SetMurderer(1);
  endif
return;
  var ModCore := FindModCore(who);

  if(ModCore)
    var ev := struct;
    ev.+type := EVID_MURDPENALTY;
    ev.+source := who;
    ev.+victim := Victim;
    ModCore.sendevent(ev);
  else
    __ApplyMurdPenaltyMods(who, Victim);
  endif

endfunction

//-----------------------------------------------------------------------------

function __ApplyMurdPenaltyMods(who, Victim)
return;
  var MurdData := GetMurdData(who);

  if(MurdData)
    MurdData.CurCurse := CInt(GetObjProperty(who, PROP_MURDPNCURSE));
    if(!MurdData.LastKill or (MurdData.LastKill + MP_DELAY_TIME < GetObjProperty(who, PROP_ONLINETIMER)))
      /*if(MurdData.CurCurse)
        ModifyDexterityMod(who, MurdData.CurCurse*2);
        ModifyStrengthMod(who, MurdData.CurCurse*2);
        ModifyIntelligenceMod(who, MurdData.CurCurse*2);
        RecoverMurdPnSkill(who, SKILLID_SWORDSMANSHIP, MPF_LOWER_SWORDS,    MurdData);
        RecoverMurdPnSkill(who, SKILLID_MACEFIGHTING,  MPF_LOWER_MACES,     MurdData);
        RecoverMurdPnSkill(who, SKILLID_FENCING,       MPF_LOWER_FENCING,   MurdData);
        RecoverMurdPnSkill(who, SKILLID_WRESTLING,     MPF_LOWER_WRESTLING, MurdData);
        RecoverMurdPnSkill(who, SKILLID_ARCHERY,       MPF_LOWER_ARCHERY,   MurdData);
        RecoverMurdPnSkill(who, SKILLID_TACTICS,       MPF_LOWER_TACTICS,   MurdData);
        RecoverMurdPnSkill(who, SKILLID_ANATOMY,       MPF_LOWER_ANATOMY,   MurdData);
        RecoverMurdPnSkill(who, SKILLID_EVALINT,       MPF_LOWER_EVALINT,   MurdData);
        RecoverMurdPnSkill(who, SKILLID_MAGERY,        MPF_LOWER_MAGERY,    MurdData);
        RecoverMurdPnSkill(who, SKILLID_AXESMANSHIP,   MPF_LOWER_AXES,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_VAMPIRE,       MPF_LOWER_VAMP,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_WEARWOLF,      MPF_LOWER_LIKAN,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_NECRO,         MPF_LOWER_NECRO,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_DRUID,         MPF_LOWER_DRUID,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_PALADINISM,    MPF_LOWER_PALADINISM,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_FANATICISM,    MPF_LOWER_FANATICISM,      MurdData);
        RecoverMurdPnSkill(who, SKILLID_BARBARIAN,     MPF_LOWER_BARBARIAN,      MurdData);

        SendSysMessage(who, "Twe sumienie doznalo spokoju.", FONT_NORMAL, COLOR_GREEN);
      endif*/
      MurdData.Counter  := 0;
      MurdData.CurCurse := 0;
      MurdData.CurseMod := 0;
      SetObjProperty(who, PROP_MURDPNDATA, MurdData);
      SetObjProperty(who, PROP_MURDPNCURSE, MurdData.CurCurse);

    elseif(MurdData.CurCurse < MurdData.CurseMod)

      case(RandomInt(4))
        default: SendSysMessage(who, "Wyrzuty sumienia nie pozwalaja Ci sie skoncentrowac.", FONT_NORMAL, COLOR_RED);
        1: SendSysMessage(who, "Wyrzuty sumienia nie daja Ci spokoju...", FONT_NORMAL, COLOR_RED);
        2: SendSysMessage(who, "Wyrzuty sumienia dobijaja Cie...", FONT_NORMAL, COLOR_RED);
      endcase
      
      if(Victim)
        SendBad(Victim, "Zostal"+ggm(Victim,3)+" zaatakowany przez morderce!");
        SendBad(Victim, "Uciekaj w te pedy!", COLOR_ORANGE);
      endif

      TemporarySlowdown(who, 12 + MurdData.CurseMod);
      PlayPcEffect(who, FX_MARK_EFFECT, 31, 0x10, 0x10, 4);
      PlaySoundEffect( who, SFX_SPELL_MANA_VAMPIRE );
      SeekInformTownGuard(who, "Zaatakowal"+ggm(who,4)+" osobe "+GetRealName(Victim), GMR_PLAYER_ATTACK);
    
      if(!MurdData.Flags)
        MurdData.+Flags;
        MurdData.Flags := 0;
      endif

      /*
      ModifyDexterityMod(who, (MurdData.CurCurse - MurdData.CurseMod) *2);
      ModifyStrengthMod(who, (MurdData.CurCurse - MurdData.CurseMod) *2);
      ModifyIntelligenceMod(who, (MurdData.CurCurse - MurdData.CurseMod) *2);
      LowerMurdPnSkill(who, SKILLID_SWORDSMANSHIP, MPF_LOWER_SWORDS,    MurdData);
      LowerMurdPnSkill(who, SKILLID_MACEFIGHTING,  MPF_LOWER_MACES,     MurdData);
      LowerMurdPnSkill(who, SKILLID_FENCING,       MPF_LOWER_FENCING,   MurdData);
      LowerMurdPnSkill(who, SKILLID_WRESTLING,     MPF_LOWER_WRESTLING, MurdData);
      LowerMurdPnSkill(who, SKILLID_ARCHERY,       MPF_LOWER_ARCHERY,   MurdData);
      LowerMurdPnSkill(who, SKILLID_TACTICS,       MPF_LOWER_TACTICS,   MurdData);
      LowerMurdPnSkill(who, SKILLID_ANATOMY,       MPF_LOWER_ANATOMY,   MurdData);
      LowerMurdPnSkill(who, SKILLID_EVALINT,       MPF_LOWER_EVALINT,   MurdData);
      LowerMurdPnSkill(who, SKILLID_MAGERY,        MPF_LOWER_MAGERY,    MurdData);
      LowerMurdPnSkill(who, SKILLID_AXESMANSHIP,   MPF_LOWER_AXES,      MurdData);
      LowerMurdPnSkill(who, SKILLID_VAMPIRE,       MPF_LOWER_VAMP,      MurdData);
      LowerMurdPnSkill(who, SKILLID_WEARWOLF,      MPF_LOWER_LIKAN,      MurdData);
      LowerMurdPnSkill(who, SKILLID_NECRO,         MPF_LOWER_NECRO,      MurdData);
      LowerMurdPnSkill(who, SKILLID_DRUID,         MPF_LOWER_DRUID,      MurdData);
      LowerMurdPnSkill(who, SKILLID_PALADINISM,    MPF_LOWER_PALADINISM,      MurdData);
      LowerMurdPnSkill(who, SKILLID_FANATICISM,    MPF_LOWER_FANATICISM,      MurdData);
      LowerMurdPnSkill(who, SKILLID_BARBARIAN,     MPF_LOWER_BARBARIAN,      MurdData);
      */
      MurdData.CurCurse := MurdData.CurseMod;
      SetObjProperty(who, PROP_MURDPNDATA, MurdData);
      SetObjProperty(who, PROP_MURDPNCURSE, MurdData.CurCurse);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function LowerMurdPnSkill(who, SkID, SkFlag, byref MurdData)

  if(GetBaseSkill(who, SkID) > MP_MIN_LSKVAL)
    if(!MurdData.CurCurse or (MurdData.Flags & SkFlag))
      ModifySkillTemporaryMod(who, SkID, MurdData.CurCurse - MurdData.CurseMod);
      SetBitOn(MurdData.Flags, SkFlag);
    endif
  elseif(!MurdData.CurCurse)
    SetBitOff(MurdData.Flags, SkFlag);
  endif

endfunction

//-----------------------------------------------------------------------------

function RecoverMurdPnSkill(who, SkID, SkFlag, byref MurdData)

  if(MurdData.Flags & SkFlag)
    ModifySkillTemporaryMod(who, SkID, MurdData.CurCurse);
    SetBitOff(MurdData.Flags, SkFlag);
  endif

endfunction

//-----------------------------------------------------------------------------

function IsAggressorFor(who, byref Victim)

  if(!IsPlayer(Victim) and IsTamed(Victim))
    Victim := GetMaster(Victim);
  endif

  if(who == Victim or who.cmdlevel or !IsPlayer(Victim) or who.dead or Victim.dead)
    return 0;
  endif

  if(Victim.murderer and who.murderer)
    return 0;
  endif
  
  if(Victim.criminal and who.criminal)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CheckAggresiveAction(who, Targ)

  if(IsAggressorFor(who, Targ))
    RecheckAttackFlag(who, Targ);

    if(IsOrderGuarded(Targ, who))
      // nie mozna ciskac stad applypenalty, bo to jest script oncompletion - nie sleepuje!
      // a MOD_RUN_SPEED chcemy zrzucic z waitem, i tu jest zonk! CRASHuje serwer!
      start_script(":pvparea:apply_murd", array(who, Targ));
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
