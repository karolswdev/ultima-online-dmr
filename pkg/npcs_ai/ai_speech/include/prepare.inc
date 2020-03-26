//-----------------------------------------------------------------------------

include "const";
include "proc_event";
include "process";
include "say";
include "triggers";
include "defaults";
include "question";

include "utils/misc";
include "utils/speech";

//-----------------------------------------------------------------------------

var _AI_Speech;

//-----------------------------------------------------------------------------

function AISys_Speech_Init()

  SetBitOn(_AI_Data.sysFlags, AI_SYS_SPEECH);

  AI_AddEvent(EVID_ITEM_GIVEN);
  AI_AddEvent(EVID_SPEECH, AI_SPEECH_DEF_RANGE);

  return AISys_Speech_Prepare();

endfunction

//-----------------------------------------------------------------------------

function AISys_Speech_Prepare()

  _AI_Speech := struct;

  _AI_Speech.+Triggers := array;
  _AI_Speech.+QuestionData := 0;

  return 1;

endfunction

//-----------------------------------------------------------------------------
