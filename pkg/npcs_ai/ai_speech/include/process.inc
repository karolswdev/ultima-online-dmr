//-----------------------------------------------------------------------------

function AISys_Speech_Process()

  // sprawdzmy czy pytanie sie nie przeterminowalo
  AISys_Speech_QuestionCheck();

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_GetWaitTime()

  if(_AI_Speech.QuestionData)
    return 2; // dwie sekundy jesli mamy oczekajace pytanie
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------
