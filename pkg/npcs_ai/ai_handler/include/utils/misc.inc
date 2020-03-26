//-----------------------------------------------------------------------------

const AIH_RES_SUCCESS        := 1;
const AIH_RES_UNKNOWN        := 0;

//-----------------------------------------------------------------------------

function AIHandler_Init(byref Data)

  if(!Data)
    return 0;
  elseif(!Data.npc.isa(POLCLASS_NPC))
    return 0;
  elseif(!Data.source.isa(POLCLASS_MOBILE) and Data.source != 0)
    return 0;
  endif

  if(Data.source)
    // chcemy, aby tylko jeden handler mogl byc uruchomiony na danym osobniku
    var BlockProc := AIHandler_Blocked(Data.source);

    if(BlockProc)
      LogNotice("AIHandler: ["+GetProcess(GetPid()).name+"] zostal ominiety, poniewaz byl juz drugi uruchomiony ["+BlockProc.name+"]!", LOGF_SCRIPTS);
      return 0;
    endif

    // ustawiamy controllera skryptu jako zrodlo
    SetScriptController(Data.source);
    SetObjProperty(Data.source, "#NpcHandlerPid", GetPid());
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AIHandler_Blocked(Source)

  var Pid := CInt(GetObjProperty(Source, "#NpcHandlerPid"));

  if(!Pid)
    return 0;
  endif

  var Proc := 0;
  var maxTime := ReadMilliSecondClock() + 1500;

  repeat

    Proc := GetProcess(Pid);

    if(Proc)
      sleepms(100);
    endif

  until(!Proc or ReadMilliSecondClock() > maxTime );

  return Proc;

endfunction

//-----------------------------------------------------------------------------
