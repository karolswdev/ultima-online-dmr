//-----------------------------------------------------------------------------

include ":eventcore:basic";
include ":eventcore:finding";

//-----------------------------------------------------------------------------

function SendGlobalMessage(MsgID, EvID, pData := 0)

  var Data := struct;
  Data.+evid := EvID;
  Data.+msgid := MsgID;
  Data.+data := pData;

  var Proc := GetGlobCtrlProc(":eventcore:event_control");

  if(Proc)
    return Proc.sendevent(Data);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendEventMessageForObject(who, MsgID, pData := 0)

  if(who)
    var EvID := FindEventAreaByLoc(who);

    if(EvID)
      return SendEventMessage(EvID, MsgID, pData);
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SendEventPlayerDeath(corpse, ghost)

  var EvId := FindEventAreaByLoc(corpse);
  
  if(EvId)
    ProcessEventPlayerDeath(corpse, ghost);

    var Data := struct;
    Data.+corpse := corpse;
    Data.+ghost := ghost;

    if(!SendEventMessage(EvID, MSGID_PLAYER_DEATH, Data))
      LogError("[EventCore::SendPlayerDeath] Gracz umarl na terenie eventu ID: '"+EvId+"', ale proces eventu nie istnieje!");
    endif

    return EvId;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ProcessEventPlayerDeath(corpse, ghost)

  var BankBox := FindBankBox(ghost);
  
  foreach item in EnumerateItemsInContainer(corpse)
    if(IsAutoEventItem(item))
      MoveToPack(item, corpse); // dla pewnosci, by byly na wierzchu
    endif
  endforeach

  foreach item in ListRootItemsInContainer(corpse)
    if(!IsAutoEventItem(item))
      MoveItemToContainer(item, BankBox);
    endif
  endforeach

  SendGood(ghost, "Twoje zostaly przetransportowane do Twojej skrzyni bankowej.");

endfunction

//-----------------------------------------------------------------------------
