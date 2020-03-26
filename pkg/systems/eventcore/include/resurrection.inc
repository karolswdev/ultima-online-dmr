//-----------------------------------------------------------------------------

function CheckAutoEventResurrection(who)

  if(GetObjProperty(who, "AutoEvent"))
    var Data := struct;
    Data.+source := who;

    SendEventMessageForObject(who, MSGID_PLAYER_RESURRECT, Data);
  endif

endfunction

//-----------------------------------------------------------------------------
