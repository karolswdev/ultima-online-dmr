//--------------------------------------------------------------------------------

use file;
use cfgfile;

//--------------------------------------------------------------------------------

include "include/client/fonts";
include "include/const/cfgs";
include "include/staff";

//--------------------------------------------------------------------------------

const LOGF_ALERT_SEERS                 := 0x00000001;
const LOGF_ALERT_GMS                   := 0x00000002;
const LOGF_ALERT_ADMINS                := 0x00000004;
const LOGF_NO_NEED                     := 0x00000008;
const LOGF_FORCE_ALERT                 := 0x00000010;
const LOGF_NOTE_INFO                   := 0x00000020;
const LOGF_FORCE_SYSLOG                := 0x00000040;

const LOGF_GENERAL                     := 0x10000000;
const LOGF_WORLD                       := 0x20000000;
const LOGF_SCRIPTS                     := 0x40000000;

//--------------------------------------------------------------------------------

function LogError(Msg, Flags := 0)

  var Cfg  := ReadConfigFile(LOGOPT_CFG);

  var FileName := SM_ERROR_LOG;

  if(Flags & LOGF_WORLD)
    FileName := WM_ERROR_LOG;
  elseif(Flags & LOGF_GENERAL)
    FileName := ERROR_LOG;
  endif

  if(!(Flags & LOGF_NO_NEED) or Cfg["LogErrors"].LogNotNeeded)
    LogToFile(FileName, Msg, LOG_DATETIME);
  endif

  if(Cfg["LogErrors"].SyslogError or (Flags & LOGF_FORCE_SYSLOG))
    syslog("[Error]: "+Msg);
  endif

  if(Cfg["LogErrors"].AlertStaff or (Flags & LOGF_FORCE_ALERT))
    if(Flags & LOGF_ALERT_SEERS)
      AlertSeers(Msg, FONT_NORMAL, COLOR_RED_LIGHT);
    elseif(Flags & LOGF_ALERT_GMS)
      AlertGMs(Msg, FONT_NORMAL, COLOR_RED_LIGHT);
    elseif(Flags & LOGF_ALERT_ADMINS)
      AlertAdmins(Msg, FONT_NORMAL, COLOR_RED_LIGHT);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function LogNotice(Msg, Flags := 0)

  var Cfg := ReadConfigFile(LOGOPT_CFG);

  var FileName := NOTICE_LOG;

  if(Flags & LOGF_WORLD)
    FileName := WM_NOTICE_LOG;
  elseif(Flags & LOGF_SCRIPTS)
    FileName := SM_NOTICE_LOG;
  endif

  if(!(Flags & LOGF_NO_NEED) or Cfg["LogErrors"].LogNotNeeded)
    LogToFile(FileName, Msg, LOG_DATETIME);
  endif

  if(Flags & LOGF_NOTE_INFO)
    if(Cfg["LogErrors"].SyslogNoteInfo or (Flags & LOGF_FORCE_SYSLOG))
      syslog("[Info]: "+Msg);
    endif
  else
    if(Cfg["LogErrors"].SyslogNotice or (Flags & LOGF_FORCE_SYSLOG))
      syslog("[Notice]: "+Msg);
    endif
  endif

  if(Cfg["LogErrors"].AlertStaff or (Flags & LOGF_FORCE_ALERT))
    if(Flags & LOGF_ALERT_SEERS)
      AlertSeers(Msg, FONT_NORMAL, COLOR_GREEN_LIGHT);
    elseif(Flags & LOGF_ALERT_GMS)
      AlertGMs(Msg, FONT_NORMAL, COLOR_GREEN_LIGHT);
    elseif(Flags & LOGF_ALERT_ADMINS)
      AlertAdmins(Msg, FONT_NORMAL, COLOR_GREEN_LIGHT);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------
