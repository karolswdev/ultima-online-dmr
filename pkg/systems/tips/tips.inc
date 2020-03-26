//-----------------------------------------------------------------------------
// Modified: 2006-11-22
//-----------------------------------------------------------------------------

include "tip_data";

//-----------------------------------------------------------------------------
// TipWatcher Events
//-----------------------------------------------------------------------------

const TWE_PROC_EVENT         := 1;
const TWE_PROC_TIP           := 2;

//-----------------------------------------------------------------------------
// TipWatcher Flags (Proc Tip Flags)
//-----------------------------------------------------------------------------

const TWPTF_IMMED            := 0x0100;
const TWPTF_FORCE            := 0x0200;
const TWPTF_CHECK_CLASS      := 0x0400;
const TWPTF_MY_CLASS         := 0x0800;
const TWPTF_ONETIME_EVENT    := 0x1000;
const TWPTF_QUEST_CREATED    := 0x2000;

//-----------------------------------------------------------------------------
// Day Tip
//-----------------------------------------------------------------------------
const DT_DATE_FS := "%d-%m-%Y";

//-----------------------------------------------------------------------------

function TipsEnabled()

  var Cfg := ReadConfigFile(TIP_MAIN_CFG);

  return Cfg["Main"].Enabled;

endfunction

//-----------------------------------------------------------------------------

function TriggerTipEvent(who, eventId, tFlags := TWPTF_CHECK_CLASS)

  if(!TipsEnabled())
    return;
  endif

  if(!(tFlags & TWPTF_ONETIME_EVENT))
    var TipEvents := GetObjProperty(who, PROP_TIPS_EVENTS);
    if(!TipEvents)
      TipEvents := array;
    endif

    if(eventId in TipEvents)
      return;
    endif

    TipEvents.append(eventId);
    SetObjProperty(who, PROP_TIPS_EVENTS, TipEvents);
    
  endif

  var TipWatcher := GetTipWatcher(who);

  if(!TipWatcher)
    return;
  endif

  var tEv := struct;
  tEv.+type   := TWE_PROC_EVENT;
  tEv.+eId    := eventId;
  tEv.+tFlags := tFlags;

  TipWatcher.sendevent(tEv);

endfunction

//-----------------------------------------------------------------------------

function TriggerTip(who, tipId, tFlags)

  var TipWatcher := GetTipWatcher(who);

  if(!TipWatcher)
    return;
  endif

  var ev := struct;
  ev.+type   := TWE_PROC_TIP;
  ev.+tId    := tipId;
  ev.+tFlags := tFlags;

  TipWatcher.sendevent(ev);

endfunction

//-----------------------------------------------------------------------------

function GetTipWatcher(who)

  var Proc := GetProc(GetObjProperty(who, "#TipWatcherPid"));

  if(!Proc)
    Proc := start_script(":tips:tip_watcher", who);
  endif

  return Proc;

endfunction

//-----------------------------------------------------------------------------
