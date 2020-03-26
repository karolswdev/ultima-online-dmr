//-----------------------------------------------------------------------------

function AI_EnableEvents()

  foreach EvParam in (_AI_Data.Events)
    if(EvParam != -1)
      AI_Log("System", "EnableEvent ["+GetEventName(_EvParam_iter)+"] Range: "+EvParam);
    else
      AI_Log("System", "EnableEvent ["+GetEventName(_EvParam_iter)+"]");
    endif
    EnableEvents(_EvParam_iter, EvParam);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function AI_DisableEvents()

  foreach EvParam in (_AI_Data.Events)
    if(EvParam != -1)
      AI_Log("System", "DisableEvent ["+GetEventName(_EvParam_iter)+"] Range: "+EvParam);
    else
      AI_Log("System", "DisableEvent ["+GetEventName(_EvParam_iter)+"]");
    endif
    DisableEvents(_EvParam_iter);
  endforeach

endfunction

//-----------------------------------------------------------------------------

function AI_AddEvent(EvId, EvParam := -1)

  _AI_Data.Events[EvId] := EvParam;

endfunction

//-----------------------------------------------------------------------------

function AI_DelEvent(EvId)

  _AI_Data.Events.erase(EvId);

endfunction

//-----------------------------------------------------------------------------
