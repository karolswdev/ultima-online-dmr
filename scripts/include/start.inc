//-----------------------------------------------------------------------------

use uo;
use os;

//-----------------------------------------------------------------------------

include "include/const/cfgs";
include "include/logerror";
include "include/proc";

//-----------------------------------------------------------------------------

const PROP_GCTRL_SCRIPT                := "GlCtrl_";
const PROP_GCHK_SCRIPT                 := "GlChk_";
const PROP_GCTRL_LIST                  := "GlCtrlScList";
const PROP_GCHK_LIST                   := "GlChkScList";

const SERV_ST_DOWN                     := 0;
const SERV_ST_UP                       := 1;
const SERV_ST_RES                      := 2;

const SHARDINFO_STATUS                 := "STATUS=";
const SHARDINFO_UPTIME                 := "UP_TIME=";
const SHARDINFO_ONLINE                 := "ONLINE=";
const SHARDINFO_SEASON                 := "SEASON=";
const SHARDINFO_NIGHT                  := "NIGHT=";
const SHARDINFO_ACC_COUNT              := "ACC_COUNT=";
const SHARDINFO_CHARS_COUNT            := "CHARS_COUNT=";
const SHARDINFO_ACTIVE_ACC_COUNT       := "ACTIVE_ACC_COUNT=";
const SHARDINFO_NEW_ACC_COUNT          := "NEW_ACC_COUNT=";
const SHARDINFO_SERVER_LIFETIME        := "SERVER_LIFETIME=";
const SHARDINFO_LAST_ACCOUNT           := "LAST_ACCOUNT=";

const SI_STATUS_IDX                    := 1;
const SI_UPTIME_IDX                    := 2;
const SI_ONLINE_IDX                    := 3;
const SI_SEASON_IDX                    := 4;
const SI_NIGHT_IDX                     := 5;
const SI_ACC_COUNT_IDX                 := 6;
const SI_CHARS_COUNT_IDX               := 7;
const SI_ACTIVE_ACC_COUNT_IDX          := 8;
const SI_NEW_ACC_COUNT_IDX             := 9;
const SI_SERVER_LIFETIME               := 10;
const SI_LAST_ACCOUNT                  := 11;

//-----------------------------------------------------------------------------

function AddGlobCtrlScript(Script, Name)

  SetGlobalProperty(PROP_GCTRL_SCRIPT+Script, array(Name, 0, 0));

  var List := GetGlobalProperty(PROP_GCTRL_LIST);
  if(!List)
    List := array;
  endif

  if(!(Script in List))
    List.append(Script);
    SetGlobalProperty(PROP_GCTRL_LIST, List);
  endif

endfunction

//-----------------------------------------------------------------------------

function StartGlobCtrlScript(Script)

  if(!Script)
    return error;
  endif

  var Prop := GetGlobalProperty(PROP_GCTRL_SCRIPT+Script);

  if(Prop)
    if(ExPid(Prop[3]))
      return 0;
    endif
  endif

  var Res := start_script(Script, array(Script));

  if(Res)
    print("GlobalControl: "+Prop[1]+" [Started].");
    return 1;
  else
    print("*** GlobalControl: "+Prop[1]+" [Start Failed!] ["+Res+"] ***");
    return error;
  endif

endfunction

//-----------------------------------------------------------------------------

function StopGlobCtrlScript(Script)

  if(!Script)
    return error;
  endif

  var Prop := GetGlobalProperty(PROP_GCTRL_SCRIPT+Script);
  var Proc := 0;

  if(Prop)
    Proc := GetProc(Prop[3]);
  endif

  if(!Proc)
    return 0;
  endif
  
  Proc.kill();
  print("GlobalControl: "+Prop[1]+" [Stopped].");
  Prop[2] := PolCore().systime;
  Prop[3] := 0;
  SetGlobalProperty(PROP_GCTRL_SCRIPT+Script, Prop);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function RestartGlobCtrlScript(Script)

  if(!Script)
    return error;
  endif

  var Prop := GetGlobalProperty(PROP_GCTRL_SCRIPT+Script);
  var Proc := 0;

  if(Prop)
    Proc := GetProc(Prop[3]);
  endif

  if(Proc)
    Proc.kill();
  endif
  
  if(start_script(Script, array(Script)))
    print("GlobalControl: "+Prop[1]+" [Restarted].");
    return 1;
  else
    print("*** GlobalControl: "+Prop[1]+" [Restart Failed!] ***");
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function FillGlobCtrl(byref Parms)

  if(!Parms[1])
    print("GlobalControl: Fill Failed ["+Parms[1]+"]");
    return 0;
  endif

  var Prop := GetGlobalProperty(PROP_GCTRL_SCRIPT+Parms[1]);

  if(!Prop)
    print("GlobalControl: Fill Failed ["+Parms[1]+"] Prop Missing.");
    return 0;
  elseif(ExPid(Prop[3]))
    print("GlobalControl: Fill Failed ["+Parms[1]+"] Pid: "+Prop[3]+" Exists.");
    return 0;
  endif

  Prop[2] := PolCore().systime;
  Prop[3] := GetPid();

  SetGlobalProperty(PROP_GCTRL_SCRIPT+Parms[1], Prop);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetGlobCtrlProc(ScName)

  var Prop := CInt(GetGlobalProperty(PROP_GCTRL_SCRIPT+ScName)[3]);

  if(Prop)
    return GetProcess(Prop);
  endif

  LogError("GetGlobCtrlProc: ["+ScName+"] Nie istnieje.");
  return 0;

endfunction

//-----------------------------------------------------------------------------

function AddGlobChkScript(Script, Name)

  SetGlobalProperty(PROP_GCHK_SCRIPT+Script, array(Name, 0, 0));

  var List := GetGlobalProperty(PROP_GCHK_LIST);
  if(!List)
    List := array;
  endif

  if(!(Script in List))
    List.append(Script);
    SetGlobalProperty(PROP_GCHK_LIST, List);
  endif

endfunction

//-----------------------------------------------------------------------------

function StartGlobChkScript(Script)

  if(!Script)
    return error;
  endif

  var Prop := GetGlobalProperty(PROP_GCHK_SCRIPT+Script);

  if(Prop)
    if(ExPid(Prop[3]))
      return 0;
    endif
  endif

  if(start_script(Script, array(Script)))
    print("GlobalCheck: "+Prop[1]+" [Started].");
    return 1;
  else
    print("*** GlobalCheck: "+Prop[1]+" [Start Failed!] ***");
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function StopGlobChkScript(Script)

  if(!Script)
    return error;
  endif

  var Prop := GetGlobalProperty(PROP_GCHK_SCRIPT+Script);
  var Proc;

  if(Prop)
    Proc := GetProc(Prop[3]);
  endif

  if(!Proc)
    return 0;
  endif

  Proc.kill();
  print("GlobalCheck: "+Prop[1]+" [Stopped].");
  Prop[2] := PolCore().systime;
  Prop[3] := 0;
  SetGlobalProperty(PROP_GCHK_SCRIPT+Script, Prop);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function FillGlobChk(byref Parms)

  if(!Parms[1])
    print("GlobalCheck: Fill Failed ["+Parms[1]+"]");
    return 0;
  endif

  var Prop := GetGlobalProperty(PROP_GCHK_SCRIPT+Parms[1]);

  if(!Prop)
    print("GlobalCheck: Fill Failed ["+Parms[1]+"] Prop Missing.");
    return 0;
  elseif(ExPid(Prop[3]))
    print("GlobalCheck: Fill Failed ["+Parms[1]+"] Pid: "+Prop[3]+" Exists.");
    return 0;
  endif

  Prop[2] := PolCore().systime;
  Prop[3] := GetPid();

  SetGlobalProperty(PROP_GCHK_SCRIPT+Parms[1], Prop);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetGlobChkProc(ScName)

  var Prop := CInt(GetGlobalProperty(PROP_GCHK_SCRIPT+ScName)[3]);

  if(Prop)
    return GetProcess(Prop);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetServerStatus(Status)

  WriteFile(SERVER_STATUS_LOG, array{Status});

  SendShardInfo(SI_STATUS_IDX, Status);

endfunction

//-----------------------------------------------------------------------------

function GetServerStatus()

  var ShardInfo := ReadFile(SHARDINFO_LOG);
  var Status    := "DOWN";

  if(ShardInfo[SI_STATUS_IDX][SHARDINFO_STATUS])
    Status := StrReplace(ShardInfo[SI_STATUS_IDX], SHARDINFO_STATUS, "");
  endif

  case(Status)
    "UP":       Status := SERV_ST_UP;
    "RESTART":  Status := SERV_ST_RES;
    default:    Status := SERV_ST_DOWN;
  endcase

  return Status;

endfunction

//-----------------------------------------------------------------------------

function SetOnlineNum(Num)

//  WriteFile(ONLINE_LOG, array{"Hmm"}); //Num});

  SendShardInfo(SI_ONLINE_IDX, Num);

endfunction

//-----------------------------------------------------------------------------

function GetOnlineNum()

  //return CInt(ReadFile(ONLINE_LOG)[1]);

  var ShardInfo := ReadFile(SHARDINFO_LOG);
  var Online    := 0;

  if(ShardInfo[SI_ONLINE_IDX][SHARDINFO_ONLINE])
    Online := StrReplace(ShardInfo[SI_ONLINE_IDX], SHARDINFO_ONLINE, "");
  endif

  return CInt(Online);

endfunction

//-----------------------------------------------------------------------------

function SendShardInfo(type, value)

  var ShardInfoSc := GetGlobCtrlProc(SHARDINFO_SCNAME);

  if(!ShardInfoSc)
    return 0;
  endif

  var evInfo    := struct;
  evInfo.+type  := type;
  evInfo.+value := value;

  return ShardInfoSc.sendevent(evInfo);

endfunction

//-----------------------------------------------------------------------------
