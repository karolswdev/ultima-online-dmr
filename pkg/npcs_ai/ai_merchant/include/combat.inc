//-----------------------------------------------------------------------------

function AISys_Merchant_Engaged(Event)

  AISys_Movement_Attention();

  if(!InformGuards(Self(), Event.source, "Straz! Pomocy!", GMR_NPCATTACKED))
    AISys_Speech_Say(Event.source, AI_SPID_GET_OUT);
  endif

  SetGuardChase(Self(), Event.source);

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_Damaged(Event)

  AISys_Movement_Attention();

  if(!InformGuards(Self(), Event.source, "Straz! Pomocy! Jestem rann"+ggm(Self(),4)+"!", GMR_NPCATTACKED))
    AISys_Speech_Say(Event.source, AI_SPID_GET_OUT);
  endif

  SetGuardChase(Self(), Event.source);

endfunction

//-----------------------------------------------------------------------------
