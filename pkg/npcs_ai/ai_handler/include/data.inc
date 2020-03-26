//-----------------------------------------------------------------------------

function AIHandler_PrepareData(byref Data)

  Data := struct;

  Data.+npc    := Self();
  Data.+source := 0;
  Data.+param  := 0;

endfunction

//-----------------------------------------------------------------------------
