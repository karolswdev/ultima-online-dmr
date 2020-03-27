//--------------------------------------------------------------------------------

use uo;
use os;
use cfgfile;

//--------------------------------------------------------------------------------

include "include/logerror";

//--------------------------------------------------------------------------------

const LST_CMD                          := 1;
const LST_SPELL                        := 2;
const LST_AI                           := 3;
const LST_CONTROL                      := 4;
const LST_USE                          := 5;
const LST_HOOK                         := 6;
const LST_HTTP                         := 7;

//--------------------------------------------------------------------------------

function SetLogRunCycles(Type)

  var Cfg := ReadConfigFile(LOGOPT_CFG);

  if(Cfg["LogCycles"].Enabled)
    case(Type)
      LST_CMD:      if(Cfg["LogCycles"].LogCmds)
                      return 1;
                    endif

      LST_SPELL:    if(Cfg["LogCycles"].LogSpells)
                      return 1;
                    endif

      LST_AI:       if(Cfg["LogCycles"].LogAi)
                      return 1;
                    endif

      LST_CONTROL:  if(Cfg["LogCycles"].LogControl)
                      return 1;
                    endif

      LST_USE:      if(Cfg["LogCycles"].LogUse)
                      return 1;
                    endif

      LST_HOOK:     if(Cfg["LogCycles"].LogHook)
                      return 1;
                    endif

      LST_HTTP:     if(Cfg["LogCycles"].LogHttp)
                      return 1;
                    endif
    endcase
  endif

  set_script_option(SCRIPTOPT_NO_RUNAWAY, 1);

endfunction

//--------------------------------------------------------------------------------
