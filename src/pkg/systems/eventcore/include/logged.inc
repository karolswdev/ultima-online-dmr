//-----------------------------------------------------------------------------

function CheckAutoEventPlayerLogin(who)

  if(GetObjProperty(who, "AutoEvent"))
    var Data := struct;
    Data.+source := who;

    SendEventMessageForObject(who, MSGID_PLAYER_LOGIN, Data);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoEventPlayerLogoff(who)

  if(GetObjProperty(who, "AutoEvent"))
    var Data := struct;
    Data.+source := who;

    SendEventMessageForObject(who, MSGID_PLAYER_LOGOFF, Data);
  endif

endfunction

//-----------------------------------------------------------------------------
