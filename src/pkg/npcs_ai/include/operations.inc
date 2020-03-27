//-----------------------------------------------------------------------------

function SetNpcState(NewState)

  _AI_Data.npcState := NewState;

endfunction

//-----------------------------------------------------------------------------

function AISysEnabled(Flag)

  return IsBitOn(_AI_Data.sysFlags, Flag);

endfunction

//-----------------------------------------------------------------------------
