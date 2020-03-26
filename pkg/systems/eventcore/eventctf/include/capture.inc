//-----------------------------------------------------------------------------

const CTF_STATUS_GATHER      := 1;
const CTF_STATUS_PREPARE     := 2;
const CTF_STATUS_FIGHT       := 3;
const CTF_STATUS_ENDING      := 4;
const CTF_STATUS_FINISHING   := 5;

//-----------------------------------------------------------------------------
// nie ruszac, bo wykorzystywane jest jako indeksy arraya Data.Players!!!
const CTF_RED_TEAM     := 1;
const CTF_BLUE_TEAM    := 2;

//-----------------------------------------------------------------------------

const MSGIDC_FLAG_CAPTURED  := 101;
const MSGIDC_FLAG_PLACING   := 102;
const MSGIDC_FLAG_PLACED    := 103;
const MSGIDC_FLAG_RETURNED  := 104;

//-----------------------------------------------------------------------------

const UOBJ_CTFBLUE_CRYSTAL  := 0xA274;
const UOBJ_CTFRED_CRYSTAL   := 0xA275;

const UOBJ_CTFRED_CLOAK     := 0xA278;
const UOBJ_CTFBLUE_CLOAK    := 0xA279;

//-----------------------------------------------------------------------------

const CTFRED_COLOR          := 1209;
const CTFBLUE_COLOR         := 1321;

const CTFRED_LIGHTCOLOR     := 39;
const CTFBLUE_LIGHTCOLOR    := 95;

//-----------------------------------------------------------------------------

function PrepareCtfData()

  var Data := struct;
  Data.+Status := CTF_STATUS_GATHER;
  Data.+LastTime;
  Data.+GateInSerial;
  Data.+GateOutSerial;
  Data.+Players := array(array, array);
  Data.+WarnMsgDone := 0;
  Data.+Matches := array;
  Data.+CrystalSerials := array;

  return Data;

endfunction

//-----------------------------------------------------------------------------

function GetCaptureTeam(who)

  return GetObjProperty(who, "CaptureTeamId");

endfunction

//-----------------------------------------------------------------------------

function ClearCaptureTeam(who)

  EraseObjProperty(who, "CaptureTeamId");

endfunction

//-----------------------------------------------------------------------------
  
function SetCaptureTeam(who, TeamId)

  SetObjProperty(who, "CaptureTeamId", TeamId);

endfunction

//-----------------------------------------------------------------------------

function CreateCaptureCape(who, TeamID)

  var Cape := GetEquipmentByLayer(who, LAYER_CAPE);

  var CloakObj := UOBJ_CTFRED_CLOAK;

  if(TeamId == CTF_BLUE_TEAM)
    CloakObj := UOBJ_CTFBLUE_CLOAK;
  endif

  if(Cape.objtype == CloakObj)
    return;
  endif

  if(Cape)
    if(!MoveToPack(Cape, who))
      if(Cape.objtype == UOBJ_CTFBLUE_CLOAK or Cape.objtype == UOBJ_CTFRED_CLOAK)
        DestroyItem(Cape);
      endif
    endif
  endif

  if(GetEquipmentByLayer(who, LAYER_CAPE))
    return;
  endif

  var TeamCloak := CreateAutoEventItem(who.backpack, CloakObj);
  
  if(!TeamCloak)
    TeamCloak := CreateAutoEventItemAtLoc(who.x, who.y, who.z, CloakObj, who.realm);
  endif

  EquipItem(who, TeamCloak);
  TeamCloak.movable := 0;

endfunction

//-----------------------------------------------------------------------------

function GetCaptureTeamByItem(Item)

  case(Item.color)
    CTFRED_COLOR:
    CTFRED_LIGHTCOLOR:
      return CTF_RED_TEAM;
      
    CTFBLUE_COLOR:
    CTFBLUE_LIGHTCOLOR:
      return CTF_BLUE_TEAM;
  endcase

  return 0;

endfunction

//-----------------------------------------------------------------------------

function RestoreCaptureCloakColor(Item)

  if(Item.objtype == UOBJ_CTFRED_CLOAK)
    Item.color := CTFRED_COLOR;
  elseif(Item.objtype == UOBJ_CTFBLUE_CLOAK)
    Item.color := CTFBLUE_COLOR;
  endif

endfunction

//-----------------------------------------------------------------------------

function LightenCaptureCloakColor(Item)

  if(Item.objtype == UOBJ_CTFRED_CLOAK)
    Item.color := CTFRED_LIGHTCOLOR;
  elseif(Item.objtype == UOBJ_CTFBLUE_CLOAK)
    Item.color := CTFBLUE_LIGHTCOLOR;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCaptureCloak(who)

  var Cloak := GetEquipmentByLayer(who, LAYER_CAPE);

  if(Cloak.objtype == UOBJ_CTFRED_CLOAK or Cloak.objtype == UOBJ_CTFBLUE_CLOAK)
    return Cloak;
  endif

  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(Item.objtype == UOBJ_CTFRED_CLOAK or Item.objtype == UOBJ_CTFBLUE_CLOAK)
      return Item;
    endif
  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CaptureBlockPlayer(Source)

  TemporarySlowdown(Source, MODDUR_INFINITE);
  DoMod(Source, MOD_BLOCK_CAST, 1, MODDUR_INFINITE);
  DoMod(Source, MOD_BLOCK_FIGHT, 1, MODDUR_INFINITE);
  DoMod(Source, MOD_BLOCK_SELFHEAL, 1, MODDUR_INFINITE);

endfunction

//-----------------------------------------------------------------------------

function CaptureUnblockPlayer(Source)

  // musi poczekac az zrzuci, bo moze pojsc zaraz event DoMod z RUN_SPEED i za chwile znow Undo ;) Tak sie dzieje w przypadku cleargottenitem (wraca do plecaka a potem od razu MoveObject)
  UndoMod(Source, MOD_RUN_SPEED, UMDF_NOCHECK);
  UndoMod(Source, MOD_BLOCK_CAST, UMDF_NOCHECK);
  UndoMod(Source, MOD_BLOCK_FIGHT, UMDF_NOCHECK);
  UndoMod(Source, MOD_BLOCK_SELFHEAL, UMDF_NOCHECK);

endfunction

//-----------------------------------------------------------------------------

function GetCaptureFightTimeLeft(EvId)

  var Event := GetEventElement(EvId);

  if(!Event)
    return error;
  endif

  var Data := Event.GetProp("ProcData");

  if(Data.Status != CTF_STATUS_FIGHT)
    return error;
  endif

  return Max(CInt(Data.LastTime) - ReadGameClock(), 0);

endfunction

//-----------------------------------------------------------------------------
