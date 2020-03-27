//-----------------------------------------------------------------------------

include ":eventcore:basic";

//-----------------------------------------------------------------------------

const PROP_AUTOEVENT_ITEM := "AutoEventItem";

//-----------------------------------------------------------------------------

function CheckAutoEventMovement(oldLoc, Obj)

  if(IsPlayer(Obj) and !IsStaff(Obj))
    var AutoEvent := CInt(GetObjProperty(Obj, "AutoEvent"));

    if(AutoEvent)
      ProcessAutoEventMove(oldLoc, Obj, AutoEvent);
    endif
  endif

endfunction

//--------------------------------------------------------------------------------

function RestoreFromAutoEventTitle(Player)

  var Prop := GetObjProperty(Player, "AutoEventTitleSave");

  if(Prop != error)
    Player.title_guild := Prop;
    EraseObjProperty(Player, "AutoEventTitleSave");
  endif

endfunction

//-----------------------------------------------------------------------------

function ProcessAutoEventMove(oldLoc, Obj, CurId)

  var OldId := FindEventAreaByLoc(oldLoc);
  var NewId := FindEventAreaByLoc(Obj);

  if((OldId != NewId and OldId == CurId) or NewId != CurId)
    // event moze dojdzie moze nie, a title musowo usunac !!!
    RestoreFromAutoEventTitle(Obj);
    SendEventMessage(OldId, MSGID_PLAYER_LEFT, Obj);
    EraseObjProperty(Obj, "AutoEvent");
    CheckAutoEventItems(Obj);
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoEventItems(Obj)

  foreach Item in ListEquippedItems(Obj)
    if(GetObjProperty(Item, PROP_AUTOEVENT_ITEM))
      SendBad(Obj, "Przedmiot '"+GetName(Item)+"' rozlecial sie na Twoich oczach!");
      DestroyItem(Item);
    endif
  endforeach

  foreach Item in EnumerateItemsInContainer(Obj.backpack)
    if(GetObjProperty(Item, PROP_AUTOEVENT_ITEM))
      SendBad(Obj, "Przedmiot '"+GetName(Item)+"' rozlecial sie na Twoich oczach!");
      DestroyItem(Item);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
