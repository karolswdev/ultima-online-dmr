//-----------------------------------------------------------------------------

include "const";
include "data";
include "defaults";
include "process";

//-----------------------------------------------------------------------------

var _AI_Handler;

//-----------------------------------------------------------------------------

function AISys_Handler_Init()

  _AI_Handler := struct;

  _AI_Handler.+Triggers := array;
  _AI_Handler.+Current := 0;

  return 1;

endfunction

//-----------------------------------------------------------------------------
