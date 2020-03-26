//-----------------------------------------------------------------------------
use util;
use os;
include "include/logerror";
include "include/const/misc";
include "include/clock";

//-----------------------------------------------------------------------------

const PROP_CHDATA           := "ChampData";
const PROP_CHHISTORY        := "ChampHistory";
const PROP_CHGATE_LOC       := "ChampGateLoc";
const PROP_CHAMPNR          := "ChampNr";
const PROP_DESTLOC          := "Dest";
const PROP_BOOKSER          := "ChampBookSer";
const PROP_CHFIGHT_DELAY    := "ChampFightDelay";

const TEMP_CHALTAR          := "champaltar";
const TEMP_CHPOD            := "champ";
const TEMP_CHTLAMP          := "champlamp";

const UOBJ_CHGATE_IN        := 0x9786;
const UOBJ_CHGATE_OUT       := 0x9787;
const UOBJ_CHBOOK           := 0x9780;
const UOBJ_TLLIGHT          := 0x376a;
const UOBJ_TLBALL           := 0x224f;
const UOBJ_SKULL1           := 0x9781;
const UOBJ_SKULL2           := 0x9782;
const UOBJ_SKULL3           := 0x9783;
const UOBJ_SKULL4           := 0x9784;
const UOBJ_SKULL5           := 0x9785;

//-----------------------------------------------------------------------------

const CHC_MSG_ADDFIGHTER     := 0x0001;
const CHC_MSG_DELFIGHTER     := 0x0002;
const CHC_MSG_CHAMPDEATH     := 0x0003;
const CHC_MSG_OPENGATE       := 0x0004;
const CHC_MSG_RESET_DELAY    := 0x0005;
const CHC_MSG_CH_DAY_ACTIV   := 0x0006;

//-----------------------------------------------------------------------------

const CHI_GTIME              := 1;
const CHI_CHTIME             := 2;
const CHI_FIGHTERS           := 3;
const CHI_FIGHTTIME          := 4;
const CHI_FIGHTBOOK          := 5;
const CHI_OGATE              := 6;
const CHI_OGATETIME          := 7;
const CHI_ACTIVE_DAYS        := 8;

//-----------------------------------------------------------------------------

const CHST_GL_WAIT           := 1;
const CHST_CH_WAIT           := 2;
const CHST_BUSY              := 3;
const CHST_OGATE             := 4;
const CHST_CAN_FIGHT         := 5;
const CHST_GL_BEFORE         := 6;
const CHST_GL_AFTER          := 7;
const CHST_NOACTIV_DAY       := 8;

//-----------------------------------------------------------------------------

const G_RANDOM_MAX           := 65536; 
const CH_GL_START_TIME       := 67500; //18:45
const GH_GL_END_TIME         := 77400; //21:30
const CH_GL_PERIOD           := 10800; //3:00
const CH_DEATH_WAIT          := 900;
const CH_GATE_WAIT           := 30;
const CH_FIGHT_DELAY         := 1800;
const CH_OPEN_GATE_DELAY     := 600;
const CH_FREE_TIME           := 30;
const CHFG_LOGOFF_DELAY      := 1800;
const CHFG_DEATH_DELAY       := 1800;

const CH_MAX_HIST_DELAY      := 2;
const CH_DELAY_TIME_MOD      := 1.6;

//-----------------------------------------------------------------------------

const CH_GTYPE_IN            := 1;
const CH_GTYPE_OUT           := 2;

//-----------------------------------------------------------------------------

const CH_BAD_SKCLR           := 999;
const CH_SKULL_TIME          := 7776000;// czas czasek wydluzony do 90 dni

//-----------------------------------------------------------------------------

function SendChampDeath(Corpse, Nr)

  var ev := struct;
  ev.+type   := CHC_MSG_CHAMPDEATH;
  ev.+nr     := Nr;
  ev.+corpse := corpse;

  var Proc   := GetChampProc();
  if(Proc)
    Proc.sendevent(ev);
  endif

endfunction

//-----------------------------------------------------------------------------

function SendChampOpenGate(Book,who)

  var ev := struct;
  ev.+type    := CHC_MSG_OPENGATE;
  ev.+book    := Book;
  ev.+source  := who;

  var Proc := GetChampProc();
  if(Proc)
    Proc.sendevent(ev);
  endif

endfunction

//-----------------------------------------------------------------------------

function SendChampAddFighter(who,Book)

  var ev := struct;
  ev.+type   := CHC_MSG_ADDFIGHTER;
  ev.+source := who;
  ev.+book   := Book;

  var Proc := GetChampProc();
  if(Proc)
    Proc.sendevent(ev);
  endif

endfunction

//-----------------------------------------------------------------------------

function SendChampDelFighter(who)

  var ev := struct;
  ev.+type   := CHC_MSG_DELFIGHTER;
  ev.+source := who;

  var Proc := GetChampProc();
  if(Proc)
    Proc.sendevent(ev);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetChampProc()

  return GetGlobCtrlProc(CHCORE_SCNAME);

endfunction

//-----------------------------------------------------------------------------

function GetChampWaitTime(ChampNr,ChampCfg := 0)

  if(!ChampCfg)
    ChampCfg := ReadConfigFile(CHAMP_CFG);
  endif

  return CInt(GetConfigInt( ChampCfg["WaitTime"], CStr(ChampNr) ));

endfunction

//-----------------------------------------------------------------------------

function GetChampFightTime(ChampNr,ChampCfg := 0)

  if(!ChampCfg)
    ChampCfg := ReadConfigFile(CHAMP_CFG);
  endif

  return CInt(GetConfigInt( ChampCfg["FightTime"], CStr(ChampNr) ));

endfunction

//-----------------------------------------------------------------------------

function GetGlobalActiveTime()
    var Data := GetGlobalProperty(PROP_CHDATA);
    return CH_GL_START_TIME + CInt((GH_GL_END_TIME-CH_GL_START_TIME)*Data[CHI_GTIME][2]/G_RANDOM_MAX);
endfunction

//-----------------------------------------------------------------------------

function GetGlobalReadyTime()
    var Data := GetGlobalProperty(PROP_CHDATA);
    return GetGlobalActiveTime() + CInt(CH_GL_PERIOD*Data[CHI_GTIME][3]/G_RANDOM_MAX)+600;
endfunction

//-----------------------------------------------------------------------------

function GetChampState(Nr)

  var Data := GetGlobalProperty(PROP_CHDATA);
  var TimeComponents := ExtractTimeComponents(PolCore().systime);
 
  if(Data[CHI_FIGHTBOOK])
    return CHST_BUSY;
  elseif(Data[CHI_OGATE])
    var Gate := FindItemBySerial(Data[CHI_OGATE]);
    var Book := FindItemBySerial(GetObjProperty(Gate, PROP_BOOKSER));
    var BookNr := GetObjProperty(Book, PROP_CHAMPNR);

    if(Nr == BookNr)
      return CHST_OGATE;
    else
      return CHST_BUSY;
    endif
  elseif(CInt(Data[CHI_GTIME][1]) > PolCore().systime)
    return CHST_GL_AFTER;
  elseif(!Data[CHI_ACTIVE_DAYS][TimeComponents.weekday])
    return CHST_NOACTIV_DAY;
  elseif(GetGlobalActiveTime() >  TimeComponents.daytime)
    return CHST_GL_BEFORE;
  elseif(GetGlobalReadyTime() > TimeComponents.daytime)
    return CHST_GL_WAIT;
  elseif(Data[CHI_CHTIME][Nr] + GetChampWaitTime(Nr) > ReadGameClock())
    return CHST_CH_WAIT;
  endif

  return CHST_CAN_FIGHT;

endfunction

//-----------------------------------------------------------------------------

function GetChampFightNr()

  var Data := GetGlobalProperty(PROP_CHDATA);
  var Book      := FindItemBySerial(Data[CHI_FIGHTBOOK]);

  if(Book)
    return CInt(GetObjProperty(Book,PROP_CHAMPNR));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampLoc(Nr)

  var ChampCfg := ReadConfigFile(CHAMP_CFG);

  if(!ChampCfg)
    LogError("GetChampLoc: Nie udalo sie odczytac konfiga mistrzow.");
  else
    var Locs := GetConfigStringArray(ChampCfg["Locs"],"Loc");
    var Loc  := SplitWords(Locs[Nr]);
    if(Loc)
      Loc[1] := CInt(Loc[1]);
      Loc[2] := CInt(Loc[2]);
      Loc[3] := CInt(Loc[3]);
    endif
    return Loc;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampOutLoc(Nr)

  var ChampCfg := ReadConfigFile(CHAMP_CFG);

  if(!ChampCfg)
    LogError("GetChampOutLoc: Nie udalo sie odczytac konfiga mistrzow.");
  else
    var Locs := GetConfigStringArray(ChampCfg["Locs"],"OutLoc");
    var Loc  := SplitWords(Locs[Nr]);
    if(Loc)
      Loc[1] := CInt(Loc[1]);
      Loc[2] := CInt(Loc[2]);
      Loc[3] := CInt(Loc[3]);
    endif
    return Loc;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampOutGate(Loc)

  if(CInt(Loc))
    Loc := GetChampOutLoc(CInt(Loc));
  endif

  foreach Item in ListItemsAtLocation(CInt(Loc[1]),CInt(Loc[2]),CInt(Loc[3]),Loc[4])
    if(Item.objtype == UOBJ_CHGATE_OUT)
      return Item;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampTLLight(Loc)

  foreach Item in ListObjectsInBox(CInt(Loc[1])-1,CInt(Loc[2])-1,-127,CInt(Loc[1])+1,CInt(Loc[2])+1,127,Loc[4])
    if(Item.isa(POLCLASS_ITEM))
      if(Item.objtype == UOBJ_TLLIGHT)
        return Item;
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampTLBall(Loc)

  foreach Item in ListObjectsInBox(CInt(Loc[1])-1,CInt(Loc[2])-1,-127,CInt(Loc[1])+1,CInt(Loc[2])+1,127,Loc[4])
    if(Item.isa(POLCLASS_ITEM))
      if(Item.objtype == UOBJ_TLBALL)
        return Item;
      endif
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetChampName(Nr)

  var ChampCfg := ReadConfigFile(CHAMP_CFG);
  return GetConfigString(ChampCfg["Names"],CStr(Nr));

endfunction

//-----------------------------------------------------------------------------
