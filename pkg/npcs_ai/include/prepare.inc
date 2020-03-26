//-----------------------------------------------------------------------------

include "include/bits";

//-----------------------------------------------------------------------------

var _AI_Data;

//-----------------------------------------------------------------------------

function AI_Init(sysFlags)

  AI_Log("System", "Preparing data...");
  AI_Prepare();

  if(!AISys_Handler_Init())
    AI_LogError("System", "Nastapil powazny blad podczas przygotowywania danych do subsystemu Handler! AI teraz zakonczy swoje dzialanie.");
    return 0;
  endif

  AI_Log("Handler","Prepared.");

  if(IsBitOn(sysFlags, AI_SYS_SPEECH))
    if(!AISys_Speech_Init())
      AI_LogError("System", "Nastapil powazny blad podczas przygotowywania danych do subsystemu Speech! AI teraz zakonczy swoje dzialanie.");
      return 0;
    endif
    AI_Log("Speech", "Prepared.");
  endif

  if(IsBitOn(sysFlags, AI_SYS_MERCHANT))
    if(!AISys_Merchant_Init())
      AI_LogError("System", "Nastapil powazny blad podczas przygotowywania danych do subsystemu Merchant! AI teraz zakonczy swoje dzialanie.");
      return 0;
    endif
    AI_Log("Merchant", "Prepared.");
  endif

  if(IsBitOn(sysFlags, AI_SYS_MOVEMENT))
    if(!AISys_Movement_Init())
      AI_LogError("System", "Nastapil powazny blad podczas przygotowywania danych do subsystemu Movement! AI teraz zakonczy swoje dzialanie.");
      return 0;
    endif
    AI_Log("Movement", "Prepared.");
  endif
  
  if(IsBitOn(sysFlags, AI_SYS_FIGHT))
    if(!AISys_Fight_Init())
      AI_LogError("System", "Nastapil powazny blad podczas przygotowywania danych do subsystemu Fight! AI teraz zakonczy swoje dzialanie.");
      return 0;
    endif
    AI_Log("Fight", "Prepared.");
  endif
  
  AI_Log("System", "Done.");
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AI_Prepare()

  _AI_Data := struct;
  _AI_Data.+npcState   := NPCSTATE_PREPARING;
  _AI_Data.+waitTime   := AI_DEF_EVENT_WAIT;
  _AI_Data.+clockSleep := ReadGameClock();
  _AI_Data.+sysFlags   := 0;
  _AI_Data.+Events     := dictionary;

endfunction

//-----------------------------------------------------------------------------
