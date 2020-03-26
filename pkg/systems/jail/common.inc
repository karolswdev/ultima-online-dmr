//-----------------------------------------------------------------------------

function IsJailed(who)

  if(Malloc(who))
	return 1;
  endif

  return GetObjProperty(who, "Jailed") != error;

endfunction

//-----------------------------------------------------------------------------
