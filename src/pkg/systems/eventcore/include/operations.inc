//--------------------------------------------------------------------------------

include ":baseitems:common";
include ":statics:common";

//--------------------------------------------------------------------------------

function CreateEventPortal(PortalLoc, DestLoc)

  if(PortalLoc and DestLoc)
    DestLoc := SplitWords(DestLoc);
    PortalLoc := SplitWords(PortalLoc);
    var Gate := CreatePortal(UOBJ_BLACKGATE, CInt(PortalLoc[1]), CInt(PortalLoc[2]), CInt(PortalLoc[3]), PortalLoc[4]);
    SetPortalLoc(Gate, CInt(DestLoc[1]), CInt(DestLoc[2]), CInt(DestLoc[3]), DestLoc[4]);
    SetCarryPortalProperty(Gate, "AutoEvent", gEventId);

    return Gate;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CreateEventExitPortal(PortalLoc, DestLoc)

  if(PortalLoc and DestLoc)
    DestLoc := SplitWords(DestLoc);
    PortalLoc := SplitWords(PortalLoc);
    var Gate := CreatePortal(UOBJ_BLACKGATE, CInt(PortalLoc[1]), CInt(PortalLoc[2]), CInt(PortalLoc[3]), PortalLoc[4]);
    SetPortalLoc(Gate, CInt(DestLoc[1]), CInt(DestLoc[2]), CInt(DestLoc[3]), DestLoc[4]);
    SetCarryPortalProperty(Gate, "AutoEvent", 0, "erase");

    return Gate;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function AddEventPlayer(EventId, Player, Title := "")

  var Event := GetEventElement(EventId);

  if(!Event)
    return error;
  endif

  var Players := Event.GetProp("ActivePlayers");
  var StaticList := Event.GetProp("Players");

  if(!Players)
    Players := array;
  endif
  
  if(!StaticList)
    StaticList := array;
  endif

  if(!(Player.serial in StaticList))
    StaticList.append(Player.serial);
    Event.SetProp("Players", StaticList);
  endif

  if(!(Player.serial in Players))
    Players.append(Player.serial);
    Event.SetProp("ActivePlayers", Players);
  endif

  if(!GetObjProperty(Player, "AutoEventTitleSave"))
    SetObjProperty(Player, "AutoEventTitleSave", Player.title_guild);
    Player.title_guild := Title;
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function DelEventPlayer(EventId, Player)

  var Event := GetEventElement(EventId);

  if(!Event)
    return error;
  endif

  RestoreFromAutoEventTitle(Player);

  var Players := Event.GetProp("ActivePlayers");

  if(Player.serial in Players)
    Players.erase(RetIndex(Players, Player.serial));
    Event.SetProp("ActivePlayers", Players);
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetEventPlayers(EventId)

  var Event := GetEventElement(EventId);

  if(!Event)
    return error;
  endif

  var Serials := Event.GetProp("ActivePlayers");
  var Players := array;

  foreach Serial in Serials
    var Player := FindPlayerBySerial(Serial);

    if(Player)
      Players.append(Player);
    endif
  endforeach

  return Players;

endfunction 

//--------------------------------------------------------------------------------

function CreateAutoEventItemAtLoc(x, y, z, ObjType, realm)

  var Item := CreateItemAtLoc(x, y, z, ObjType, 1, realm);

  if(Item)
    SetObjProperty(Item, PROP_AUTOEVENT_ITEM, ReadGameClock());
  endif

  return Item;

endfunction

//--------------------------------------------------------------------------------

function CreateAutoEventItem(Cont, ObjType)

  var Item := CreateItemInCont(Cont, ObjType);

  if(Item)
    SetObjProperty(Item, PROP_AUTOEVENT_ITEM, ReadGameClock());
  endif

  return Item;

endfunction

//--------------------------------------------------------------------------------

function ClearAutoEventAreas(EvId)

  var Event := GetEventElement(EvId);
  var Areas := Event.GetProp("Areas");

  if(Event and Areas)
    foreach Area in Areas
      foreach Item in ListObjectsInBox(Area[1], Area[2], -127, Area[3], Area[4], 127, GetRealmById(Area[5]))
        if(Item.isa(POLCLASS_ITEM))
          if(IsAutoEventItem(Item))
            DestroyItem(Item);
          elseif(Item.isa(POLCLASS_CORPSE))
            DestroyItem(Item);
          elseif(IsPortal(Item))
            DestroyItem(Item);
          elseif(!GetItemBINr(Item) and !IsStc(Item))
            if(!Item.movable or IsGmCreated(Item))
              DestroyItem(Item);
            endif
          endif
        endif
      endforeach
    endforeach
  endif

endfunction

//--------------------------------------------------------------------------------
