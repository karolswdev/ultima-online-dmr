//--------------------------------------------------------------------------------

include "include/proc";

//================================================================================
//--------------------------------------------------------------------------------
// Player Hooks
//--------------------------------------------------------------------------------
//================================================================================

function StartPlayerHooks(who)

  var Cfg     := ReadConfigFile(PLAYERHOOKS_CFG);
  var Scripts := ListConfigElemProps(Cfg["Names"]);

  foreach Script in Scripts
    start_script(Script, who);
  endforeach

endfunction

//--------------------------------------------------------------------------------

function StopPlayerHooks(who)

  var Cfg     := ReadConfigFile(PLAYERHOOKS_CFG);
  var Scripts := ListConfigElemProps(Cfg["Names"]);
  var Pid;

  foreach Script in Scripts
    Pid := GetProc(GetObjProperty(who, GetConfigString(Cfg["Names"], Script)));
    if(Pid)
      Pid.kill();
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------

function RestartPlayerHook(who, Name)

  var Cfg     := ReadConfigFile(PLAYERHOOKS_CFG);
  var Prop    := GetConfigString(Cfg["Names"], Name);

  if(!Prop)
    return 0;
  endif

  var Pid     := GetProc(GetObjProperty(who, Prop));
  if(Pid)
    Pid.kill();
  endif

  return start_script(Name, who);

endfunction

//--------------------------------------------------------------------------------

function GetPlayerHooks(who)

  var Cfg     := ReadConfigFile(PLAYERHOOKS_CFG);
  var Scripts := ListConfigElemProps(Cfg["Names"]);
  var Pids    := dictionary;

  foreach Script in Scripts
    Pids[Script] := GetProc(GetObjProperty(who, GetConfigString(Cfg["Names"], Script)));
  endforeach

  return Pids;

endfunction

//--------------------------------------------------------------------------------

function GetPlayerHooksNames()

  var Cfg := ReadConfigFile(PLAYERHOOKS_CFG);
  return ListConfigElemProps(Cfg["Names"]);

endfunction

//--------------------------------------------------------------------------------
