//-----------------------------------------------------------------------------

function EnableMainEvents()
  DisableEvents(EVID_SPEECH + EVID_LEFTAREA + EVID_DISENGAGED + EVID_OPPONENT_MOVED);
  EnableEvents(EVID_ENGAGED + EVID_DAMAGED);
  EnableEvents(EVID_ENTEREDAREA, Specs[NSPEC_THRESHOLD]);
  EnableEvents(EVID_DOUBLECLICKED);
  EnableEvents(EVID_FLEE);
endfunction

//-----------------------------------------------------------------------------

function DisableMainEvents()
  DisableEvents(EVID_ENGAGED + EVID_DAMAGED + EVID_ENTEREDAREA + EVID_FLEE);
endfunction

//-----------------------------------------------------------------------------

function EnableComMainEvents()
  EnableEvents(EVID_SPEECH,SPEECH_RANGE);
  EnableEvents(EVID_ENGAGED);
  EnableEvents(EVID_DAMAGED);
  EnableEvents(EVID_DOUBLECLICKED);
  EnableEvents(EVID_ITEM_GIVEN);
  EnableEvents(EVID_PEACEMADE + EVID_FLEE);
endfunction

//-----------------------------------------------------------------------------

function DisableComMainEvents()
  DisableEvents(EVID_SPEECH);
  DisableEvents(EVID_ENGAGED);
  DisableEvents(EVID_DAMAGED);
  DisableEvents(EVID_ITEM_GIVEN);
  EnableEvents(EVID_PEACEMADE + EVID_FLEE);
endfunction

//-----------------------------------------------------------------------------
 
