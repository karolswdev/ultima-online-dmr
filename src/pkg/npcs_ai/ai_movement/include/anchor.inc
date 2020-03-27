//-----------------------------------------------------------------------------

const AI_MOVEMENT_DEF_ANCHOR_RANGE     := 10;

const PROP_ANCHOR                      := "Anchor";
const AI_ANCHORF_FORCED                := 0x01;

//-----------------------------------------------------------------------------

function AISys_Movement_InitAnchor()

  // Anchor jest domyslnie wlaczony dla npecow miastowych
  // dla innych - nie.

  if(GetNpcConfig(Self()).CityNpc)
    AISys_Movement_EnableAnchor();
    
    // NPC nie ma jeszcze anchora? dropnijmy go!
    if(!AISys_Movement_GetAnchor())
      AISys_Movement_DropAnchor();
    endif

    // sprawdzmy czy nie oddalil sie nam NPC od anchora.
    AISys_Movement_CheckAnchor();
  else
    AISys_Movement_DisableAnchor(AI_ANCHORF_FORCED);
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_DropAnchor()

  var cfgElem := GetNpcConfig(Self());

  var Range := CInt(cfgElem.AnchorRange);

  if(!Range)
    Range := AI_MOVEMENT_DEF_ANCHOR_RANGE;
  endif

  SetProperty(PROP_ANCHOR, { Self().x, Self().y, Self().z, Range, Self().realm });

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_EnableAnchor()

  if(IsSetFlag(Self(), NPCF_NO_ANCHOR) or IsSetFlag(Self(), NPCF_ANCHOR_ENABLED))
    return 0;
  endif

  var Anchor := AISys_Movement_GetAnchor();

  SetFlag(Self(), NPCF_ANCHOR_ENABLED);
  SetAnchor(Anchor[1], Anchor[2], Anchor[4], 100);
  
  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_DisableAnchor(Flags := 0)

  if(IsSetFlag(Self(), NPCF_ANCHOR_ENABLED) or (Flags & AI_ANCHORF_FORCED))

    SetAnchor(Self().x, Self().y, 0, 0);
    UnSetFlag(Self(), NPCF_ANCHOR_ENABLED);

  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_GetAnchor()

  return GetProperty(PROP_ANCHOR);

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_CheckAnchor()

  if(IsSetFlag(Self(), NPCF_NO_ANCHOR) or !IsSetFlag(Self(), NPCF_ANCHOR_ENABLED))
    return 1;
  endif

  var anchor := GetProperty(PROP_ANCHOR);

  if(anchor)

    if(coordist(anchor[1],anchor[2],Self().x,Self().y) > anchor[4])
      MoveObject(Self(), anchor[1], anchor[2], anchor[3], anchor[5]);
    endif
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AISys_Movement_MoveToAnchor()

  if(IsSetFlag(Self(), NPCF_NO_ANCHOR))
    return 1;
  endif

  var anchor := GetProperty(PROP_ANCHOR);

  if(anchor)
    if(Self().x == anchor[1] and Self().y == anchor[2] and Self().z == anchor[3] and Self().realm == anchor[5])
      return -1;
    endif

    var Res := MoveObject(Self(), anchor[1], anchor[2], anchor[3], anchor[5]);
    if(Res)
      return 1;
    else
      AI_LogError("Movement", "MoveToAnchor: "+Self().serial+" ["+Self().x+","+Self().y+","+Self().z+","+Self().realm+" => "+Anchor[1]+","+Anchor[2]+","+Anchor[3]+" "+Anchor[5]+"] Err: "+Res.errortext);
      return 0;
    endif
  endif

  return -2;

endfunction

//-----------------------------------------------------------------------------
