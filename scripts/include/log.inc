//--------------------------------------------------------------------------------

use file;
use cfgfile;

//--------------------------------------------------------------------------------

include "include/const/cfgs";
include "include/misc";
include "include/names";

//--------------------------------------------------------------------------------

const LOG_PKG                          := ":clog:";

const KILL_LOG                         := "kill";
const DESTROY_LOG                      := "destroy";
const CREATE_LOG                       := "create";
const MOVE_LOG                         := "move";
const INFO_LOG                         := "info";
const POSSESS_LOG                      := "possess";
const SPAWN_LOG                        := "spawn";
const WRONGIP_LOG                      := "wrongip";
const GOTO_LOG                         := "goto";
const INF_LOG                          := "inf";
const PASS_LOG                         := "pass";
const SM_LOG                           := "sm";
const SHPS_LOG                         := "shps";
const FBITEMS_LOG                      := "fbit";
const QSITEMS_LOG                      := "quit";

const LE_LOGCMDS                       := "LogCmds";

//================================================================================
//--------------------------------------------------------------------------------
// Log
//--------------------------------------------------------------------------------
//================================================================================

function GetLogNames()
  return {KILL_LOG,DESTROY_LOG,CREATE_LOG,MOVE_LOG,INFO_LOG,POSSESS_LOG,SPAWN_LOG,WRONGIP_LOG,GOTO_LOG,INF_LOG,PASS_LOG,SHPS_LOG, FBITEMS_LOG,QSITEMS_LOG};
endfunction

//--------------------------------------------------------------------------------

function LogMsg(LogKind, who, what := -1)

  if(!who or !what or !LogKind)
    return;
  endif

  var Cfg := ReadConfigFile(LOGOPT_CFG);

  if(!Cfg[LE_LOGCMDS].Enabled)
    return;
  elseif(who.cmdlevel == 5)
    if(!Cfg[LE_LOGCMDS].LogDevs)
      return;
    endif
  elseif(who.cmdlevel == 4)
    if(!Cfg[LE_LOGCMDS].LogAdmins)
      return;
    endif
  endif

  if(!GetConfigInt(Cfg[LE_LOGCMDS], "En_"+LogKind))
    return;
  endif

  var LogElem := array;
  var acct    := who.acctname;
  
  if(!acct)
    acct := "NPC";
  endif

  LogElem[1] := PolCore().systime;
  LogElem[2] := GetRealName(who);
  LogElem[3] := acct;
  
  case(LogKind)
    KILL_LOG:     if(who == what)
                    return;
                  endif
    
                  LogElem[4] := {CInt(what.x),CInt(what.y),CInt(what.z),what.realm};
                  if(what.isa(POLCLASS_CORPSE))
                    LogElem[5] := GetName(what);
                    LogElem[6] := ClStr(GetObjProperty(what, PROP_NPCTEMP));
                  elseif(what.isa(POLCLASS_NPC))
                    LogElem[5] := GetName(what);
                    LogElem[6] := what.npctemplate;
                  elseif(what.dead)
                    LogElem[5] := "duch "+GetRealName(GetName(what));
                    LogElem[6] := what.acctname;
                  else
                    LogElem[5] := GetRealName(GetName(what));
                    LogElem[6] := what.acctname;
                  endif
                  
                  
    DESTROY_LOG:  if(what.isa(POLCLASS_ITEM) and GetObjProperty(what, PROP_CREATED))
                    return;
                  endif

                  LogElem[4] := what.desc;
                  LogElem[5] := what.name;
                  LogElem[6] := what.serial;
                  if(what.container)
                    LogElem[7] := CInt(what.container.serial);
                  else
                    LogElem[7] := what.x+","+what.y+","+what.z+" "+what.realm;
                  endif
                  
    QSITEMS_LOG:
    FBITEMS_LOG:  LogElem[4] := {what.x,what.y,what.z,what.realm};
                  if(what.isa(POLCLASS_NPC))
                    LogElem[5] := what.npctemplate;
                    LogElem[6] := what.serial;
                  elseif(what.isa(POLCLASS_ITEM))
                    LogElem[5] := what.desc;
                    LogElem[6] := what.serial;
                    LogElem[7] := CInt(what.container.serial);
                    LogElem[8] := what.objtype;
                  endif

    CREATE_LOG:   if(what.isa(POLCLASS_ITEM) and GetObjProperty(what, PROP_CREATED))
                    return;
                  endif
    
                  LogElem[4] := {what.x,what.y,what.z,what.realm};
                  if(what.isa(POLCLASS_NPC))
                    LogElem[5] := what.npctemplate;
                    LogElem[6] := what.serial;
                  elseif(what.isa(POLCLASS_ITEM))
                    LogElem[5] := what.desc;
                    LogElem[6] := what.serial;
                    LogElem[7] := CInt(what.container.serial);
                    LogElem[8] := what.objtype;
                  endif

    INF_LOG:
    INFO_LOG:     var Obj := what[1];
                  
                  if(who == Obj or (Obj.isa(POLCLASS_ITEM) and GetObjProperty(Obj, PROP_CREATED)))
                    return;
                  endif
                  
                  LogElem[4] := GetRealName(Obj);
                  if(Obj.isa(POLCLASS_NPC))
                    LogElem[5] := Obj.npctemplate;
                  elseif(IsPlayer(Obj))
                    LogElem[5] := Obj.acctname;
                  else
                    LogElem[5] := Obj.objtype;
                  endif

                  if(!LogElem[5])
                    LogElem[5] := "Acc";
                  endif

                  if(Obj.serial)
                    LogElem[6] := Obj.serial;
                  else
                    LogElem[6] := 0;
                  endif

                  var i;
                  for(i:=2;i<=what.size();i:=i+1)
                    LogElem[5+i] := what[i];
                  endfor

    GOTO_LOG:
    MOVE_LOG:     var Obj := what[1];

                  if(who == Obj or (Obj.isa(POLCLASS_ITEM) and GetObjProperty(Obj, PROP_CREATED)))
                    return;
                  endif
                  
                  if(Obj.x and Obj.y and LogKind != GOTO_LOG)
                    LogElem[4] := {CInt(Obj.x),CInt(Obj.y),CInt(Obj.z),Obj.realm};
                  else
                    LogElem[4] := {who.x,who.y,who.z,who.realm};
                  endif
                  LogElem[5] := {CInt(what[2]),CInt(what[3]),CInt(what[4]),what[5]};
                  if(Obj.isa(POLCLASS_MOBILE))
                    LogElem[6] := GetRealName(Obj);
                  else
                    LogElem[6] := ClStr(Obj.desc);
                  endif
                  if(Obj.serial)
                    LogElem[7] := Obj.serial;
                  else
                    LogElem[7] := "Loc";
                  endif
                  LogElem[8] := CInt(Obj.container.serial);

    POSSESS_LOG:  LogElem[4] := {what.x,what.y,what.z,what.realm};
                  LogElem[5] := what.npctemplate;
                  LogElem[6] := what.name;
                  
    SPAWN_LOG:    if(what[2] == error and !what[2].errortext)
                    what[2] := "Brak";
                  endif
                  LogElem[4] := what[1];
                  LogElem[5] := what[2];
                  LogElem[6] := what[3];
    
    WRONGIP_LOG:  LogElem[4] := who.ip;
    
    SM_LOG:       LogElem[4] := what[1];
                  LogElem[5] := what[2];
                  LogElem[6] := what[3];
                  LogElem[7] := what[4];

    PASS_LOG:     LogElem[4] := what[1];
                  
    SHPS_LOG:     LogElem[4] := what[1];
                  LogElem[5] := what[2];
                  LogElem[6] := Pack(what[3]);

  endcase

  LogToFile(LOG_PKG+LogKind,Pack(LogElem));
  
endfunction

//--------------------------------------------------------------------------------
