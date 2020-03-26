//-----------------------------------------------------------------------------

function AISys_Handler_AddTrigger(ScriptName, Trig, Flags := 0)

  var Trigger := struct;

  Trigger.+trig   := Trig;
  Trigger.+flags  := Flags;
  Trigger.+script := ScriptName;

  _AI_Handler.Triggers.append(Trigger);

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_HaveTrigger(Trig)

  foreach Trigger in (_AI_Handler.Triggers)
  
    if(IsBitOn(Trigger.trig, Trig))
      return 1;
    endif
  
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_Trigger(Source, Trig, Param := 0)

  if(AISys_Handler_IsBusy())
    return 0;
  endif

  var retRes := 0;
  var Res;

  foreach Trigger in (_AI_Handler.Triggers)
  
    if(IsBitOn(Trigger.trig, Trig))
      Res := AISys_Handler_Start(Source, Trigger.script, Trigger.flags, Param);

      if(Res)
        retRes := Res;
      endif
    endif

  endforeach

  return retRes;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_Start(Source, ScriptName, Flags, Param := 0)

  if(AISys_Handler_IsBusy())
    return error{"errortext" := "NPC jest zajety."};
  endif

  var Data;

  AIHandler_PrepareData(Data);

  Data.source := Source;
  Data.param  := Param;

  if(Flags & AIHF_SEND_MERC_DATA)
    Data.+_AI_Merchant := _AI_Merchant;
  endif

  AI_Log("Handler", "Start handler: ["+ScriptName+"] for: "+Source.name);

  // jesli chcemy, aby skrypt sie uruchomil do konca i zwrocil result

  if(Flags & AIHF_RESULT_WAIT)
    var Clock := ReadMillisecondClock();
    var Res := run_script(ScriptName, Data);
    var runTime := ReadMillisecondClock() - Clock;
    
    if(Res == error)
      AI_LogError("Handler", "Nie udalo sie uruchomic skryptu handlera ("+ScriptName+"): "+Res.errortext);
    endif
    
    AI_Log("Handler", "Handler done! ["+ScriptName+"] for: "+Source.name+" runTime: "+runTime+"ms");

    if(runTime > AI_HANDLER_MAX_SYNC_DURATION)
      
      AI_LogError("Handler", "Houston! We've got problem! Handler ["+ScriptName+"] for: "+Source.name+" runTime: "+runTime+"ms HIGHER than maxTime: "+AI_HANDLER_MAX_SYNC_DURATION+"ms");

    endif

    // handler zwrocil nam obiekt skryptu, ktory bedzie asynchronicznym handlerem npeca

    if(Res.pid)
      var Handler := struct;
      Handler.+source := Source;
      Handler.+clock  := ReadGameClock();
      Handler.+script := Res;
      Handler.+max_duration := -1; // zajety dopoki skrypt nie zakonczy dzialania

      _AI_Handler.Current := Handler;
      return 1;
    else
      return Res;
    endif
  endif

  var Handler := struct;
  Handler.+source := Source;
  Handler.+clock  := ReadGameClock();
  Handler.+script := start_script(ScriptName, Data);

  if(Flags & AIHF_PASS_DURATION)
    Handler.+max_duration := Param;
    
    if(!Handler.max_duration)
      AI_LogError("Handler", "Handler otrzymal flage MAX_DURATION, ale bez wartosci parametru!");
      return 0;
    endif
  else
    Handler.+max_duration := AI_HANDLER_DEF_DURATION;
  endif

  if(!Handler.script)
    AI_LogError("Handler", "Nie udalo sie uruchomic skryptu handlera ("+ScriptName+"): "+Handler.script);
    return 0;
  endif

  _AI_Handler.Current := Handler;
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_Kill(HandleData)

  var Res := CloseGump(HandleData.source, HandleData.script.pid);

  if(Res)
    var myCount := 0;

    while(HandleData.script and myCount < 100)
      sleepms(10);
      myCount := myCount + 1;
    endwhile
  endif

  if(HandleData.script.pid)
    return HandleData.script.kill();
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_UnBusy()

  AISys_Handler_Kill(_AI_Handler.Current);
  _AI_Handler.Current := 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_IsBusy()

  if(_AI_Handler.Current.script.pid)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_Process()

  if(AISys_Handler_IsBusy())

    if(_AI_Handler.Current.max_duration != -1)
      if(_AI_Handler.Current.clock + _AI_Handler.Current.max_duration < ReadGameClock())
        
        AI_Log("Handler", "Time to unbusy me.");
        AISys_Handler_UnBusy();
      endif
    endif

  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Handler_GetWaitTime()

  if(AISys_Handler_IsBusy())
    return 2; // niech czeka 2 sekundy
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------
