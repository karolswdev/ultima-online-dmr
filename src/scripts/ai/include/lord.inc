//-----------------------------------------------------------------------------

include "ai/include/guarding";

//-----------------------------------------------------------------------------

const L_SPEECH_RANGE         := 3;

//-----------------------------------------------------------------------------

function EnableMainEvents()

  EnableEvents(EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents(EVID_ENTEREDAREA + EVID_LEFTAREA,HALT_THRESHOLD);
  EnableEvents(EVID_SPEECH,L_SPEECH_RANGE);

endfunction

//-----------------------------------------------------------------------------

function CmdGuards(Targ,EvID)

  foreach Npc in ListMobilesNearLocation(me.x,me.y,me.z,HALT_THRESHOLD,me.realm)
    if(IsTownGuard(Npc))
      SendChEvent(Npc,Targ,EvID);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
