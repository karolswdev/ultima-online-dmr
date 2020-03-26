//-----------------------------------------------------------------------------

function CanCarrier(Npc, Player)

  __ParamUnused(Player);

  if(!GetObjProperty(Npc, "DestPos") or !GetObjProperty(Npc, "ShipDestLoc") or !GetObjProperty(Npc, "ShipLoc"))
    return RetError("Jeszcze nigdzie nie przewoze. Wpadnij za jakis czas.");
  endif

  var Tillerman := FindItemBySerial(GetObjProperty(Npc, "Tillerman"));

  if(!Tillerman)
    return RetError("Niestety, ale lodz nie istnieje.");
  endif

  var Proc := GetProc(GetObjProperty(Tillerman, "#ControlPid"));

  if(!Proc)
    return RetError("Lodz wymknela sie spod kontroli!");
  endif

  var State := CInt(GetObjProperty(Tillerman, "#State"));

  if(State != CARRIER_STATE_DOCK)
    return RetError("Statek niedawno wyplynal. Sprobuj za chwile!");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SendCarrierEvent(Npc, Player, DestLoc, ShipLoc)

  if(!CanCarrier(Npc, Player))
    return 0;
  endif

  var lev := struct;
  lev.+type := EVTYPE_CARRIER;
  lev.+npc := Npc;
  lev.+mobile := Player;
  lev.+playerloc := DestLoc;
  lev.+shiploc := ShipLoc;

  var Tillerman := FindItemBySerial(GetObjProperty(Npc, "Tillerman"));
  var Proc := GetProc(GetObjProperty(Tillerman, "#ControlPid"));
  Proc.sendevent(lev);

  return 1;

endfunction

//-----------------------------------------------------------------------------
