//-----------------------------------------------------------------------------

function CheckPlayerRef(byref who, whoSerial)

  if(!who)
    who := FindPlayerBySerial(whoSerial);
  endif

  if(!who)
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function WaitForLogon(who, maxTime := 10)

  var clockTime := ReadGameClock() + maxTime;

  while(clockTime > ReadGameClock())
    if(IsLoggedIn(who))
      return 1;
    endif
    sleep(1);
  endwhile

  return 0;

endfunction

//-----------------------------------------------------------------------------
