//-----------------------------------------------------------------------------

include "include/items";

//-----------------------------------------------------------------------------

const UOBJ_MAGICALTAR_DEED          := 0xa301;

const UOBJ_ALTAR_COLUMN             := 0xa3A9;
const UOBJ_ALTAR_OUTERCOLUMNUP      := 0xa39E;
const UOBJ_ALTAR_OUTERCOLUMNDOWN    := 0xa39F;
const UOBJ_ALTAR_PODEST             := 0xa3A0;
const UOBJ_ALTAR_STAIRS_UP          := 0xa3A1;
const UOBJ_ALTAR_STAIRS_DOWN        := 0xa3A2;
const UOBJ_ALTAR_STAIRS_LEFT        := 0xa3A3;
const UOBJ_ALTAR_STAIRS_RIGHT       := 0xa3A4;

const UOBJ_ALTAR_CORNER_UPL         := 0xa3A5;
const UOBJ_ALTAR_CORNER_UPR         := 0xa3A6;
const UOBJ_ALTAR_CORNER_DOWNL       := 0xa3A7;
const UOBJ_ALTAR_CORNER_DOWNR       := 0xa3A8;

const UOBJ_ALTAR_GREENLIGHTON       := 0xa39A;
const UOBJ_ALTAR_GREENLIGHTOFF      := 0xa39B;
const UOBJ_ALTAR_REDLIGHTON         := 0xa39C;
const UOBJ_ALTAR_REDLIGHTOFF        := 0xa39D;

const UOBJ_ALTAR_SYMBOL1            := 0xa3AA;
const UOBJ_ALTAR_SYMBOL2            := 0xa392;
const UOBJ_ALTAR_SYMBOL3            := 0xa393;
const UOBJ_ALTAR_SYMBOL4            := 0xa394;
const UOBJ_ALTAR_SYMBOL5            := 0xa395;
const UOBJ_ALTAR_SYMBOL6            := 0xa396;
const UOBJ_ALTAR_SYMBOL7            := 0xa397;
const UOBJ_ALTAR_SYMBOL8            := 0xa398;
const UOBJ_ALTAR_SYMBOL9            := 0xa399;

const UOBJ_MAGIC_SYMBOL1            := 0xA401;
const UOBJ_MAGIC_SYMBOL2            := 0x0E5F;
const UOBJ_MAGIC_SYMBOL3            := 0x0E65;
const UOBJ_MAGIC_SYMBOL4            := 0x0E68;
const UOBJ_MAGIC_SYMBOL5            := 0x0E62;
const UOBJ_ALTAR_CANDLE             := 0xA406;

const UOBJ_ALTAR_SYMBOL_CHANGE      := 0x1822;

const ALTAR_DELAY                   := 60;

const GRAPHIC_ALTAR_GREENLIGHTON    := 0x1ECD;
const GRAPHIC_ALTAR_GREENLIGHTOFF   := 0x1ECE;
const GRAPHIC_ALTAR_REDLIGHTON      := 0x1ED0;
const GRAPHIC_ALTAR_REDLIGHTOFF     := 0x1ED1;

//-----------------------------------------------------------------------------

function GetAltarSymbols()

  return {
    UOBJ_ALTAR_SYMBOL1,
    UOBJ_ALTAR_SYMBOL2,
    UOBJ_ALTAR_SYMBOL3,
    UOBJ_ALTAR_SYMBOL4,
    UOBJ_ALTAR_SYMBOL5,
    UOBJ_ALTAR_SYMBOL6,
    UOBJ_ALTAR_SYMBOL7,
    UOBJ_ALTAR_SYMBOL8,
    UOBJ_ALTAR_SYMBOL9
  };
  
endfunction

//-----------------------------------------------------------------------------

function FindAltarOwner(altarcenter)

  return FindPlayerBySerial(Cint(GetObjProperty(altarcenter,"AltarOwner")));

endfunction

//-----------------------------------------------------------------------------

function FindAltarCenter(Element)

  return SystemFindObjectBySerial(Cint(GetObjProperty(Element,"CenterSerial")));
  
endfunction

//-----------------------------------------------------------------------------

function DestroyAltar(who,AltarCenter)

  if(!CheckAltarDelay())
    SendSysMessage(who,"Magiczny oltarz jest za slaby.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  var CreatedArray  := GetObjProperty(AltarCenter,"CreatedArray");
  var AltarOwner    := FindAltarOwner(AltarCenter);
  var AltarDeed,DeedGraphic;
  
  if(AltarOwner == who or who.cmdlevel)
    AltarDeed     := CreateItemAtTempLoc(UOBJ_MAGICALTAR_DEED);
    DeedGraphic   := AltarDeed.graphic;
    if(!AltarDeed)
      SendSysMessage(who,"Nie masz miejsca w plecaku!",FONT_NORMAL,COLOR_RED);
      return;
    endif
  endif
  
  if(AltarOwner != who or who.cmdlevel)
    SendSysMessage(who,"Nie jestes wlascicielem oltarza!",FONT_NORMAL,COLOR_RED);
    return;
  endif
  

  if(!DestroyAltarElements(CreatedArray))
    AltarDeed.movable := 1;
    DestroyItem(AltarDeed);
    SendSysMessage(who,"Nie mozesz teraz zniszczyc oltarza.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  EraseAltarProperty(AltarCenter);
  
  if(AltarOwner == who or who.cmdlevel)
    MoveToPack(AltarDeed, who);
    SendSysMessage(who,"Zniszczyl"+ggm(who,3)+" oltarz i wlozyl"+ggm(who,3)+" deed do plecaka.",FONT_NORMAL,COLOR_GREEN);
  else
    DestroyItem(AltarDeed);
  endif
  
endfunction

//-----------------------------------------------------------------------------

function DestroyAltarElements(CreatedArray)

  foreach Element in CreatedArray
    if(!ReserveItem(Element) or !Element)
      return 0;
    endif
  endforeach

  foreach Element in CreatedArray
    if(!DestroyItem(Element))
      return 0;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IdentifyPrefixReagent(AltarCenter)

  return IdentifyItemAtLocation(AltarCenter.x-1,AltarCenter.y,AltarCenter.z,AltarCenter.realm);
  
endfunction

//-----------------------------------------------------------------------------

function IdentifySuffixReagent(AltarCenter)

  return IdentifyItemAtLocation(AltarCenter.x+1,AltarCenter.y,AltarCenter.z,AltarCenter.realm);

endfunction

//-----------------------------------------------------------------------------

function IdentifyItemInAltarCenter(AltarCenter)

  return IdentifyItemAtLocation(AltarCenter.x,AltarCenter.y,AltarCenter.z,AltarCenter.realm);
  
endfunction

//-----------------------------------------------------------------------------

function IdentifyItemAtLocation(x,y,z,Rlm,range := 0)

  var item := {};
  var AltarSymbols := GetAltarSymbols();
  
  foreach items in ListItemsNearLocation(x,y,z,range,Rlm)
    if(!(items.objtype in AltarSymbols) and items.objtype != UOBJ_ALTAR_PODEST)
      if(item.objtype == UOBJ_ALTAR_CANDLE)
        DestroyItem(item);
        continue;
      endif
      item.append(items);
    endif
  endforeach

  if(item.size() > 1 or !item.size())
    return 0;
  endif

  if(item[1])
    return item[1];
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function FindAltarElements(AltarCenter,ElemObj)

  var RetElements  := {};
  var CreatedArray := GetObjProperty(AltarCenter,"CreatedArray");
  if(!CreatedArray)
    return 0;
  endif
  
  foreach Elem in CreatedArray
    Elem := SystemFindObjectBySerial(Cint(Elem));
    if(!Elem)
      return 0;
    elseif(Elem.objtype == ElemObj)
      RetElements.append(Elem);
    endif
  endforeach
  
  if(RetElements.size() == 1)
    RetElements := RetElements[1];
  elseif(!RetElements.size())
    RetElements := 0;
  endif
  
  return RetElements;
  
endfunction

//-----------------------------------------------------------------------------

function EraseAltarProperty(AltarCenter)

  var Owner := FindAltarOwner(AltarCenter);
  if(Owner)
    EraseObjProperty(Owner,"AltarOwner");
  endif
  
endfunction

//-----------------------------------------------------------------------------

function HaveAltarAlready(who)

  if(SystemFindObjectBySerial(Cint(GetObjProperty(who,"AltarOwner"))))
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetAltarProperty(who,AltarCenter)

  SetObjProperty(who,"AltarOwner",AltarCenter.serial);
  
endfunction

//-----------------------------------------------------------------------------

function SetAltarDelay()

  SetObjProperty(AltarCenter,"AltarDelay",ReadGameClock() + ALTAR_DELAY);
  start_script("regen_altar", AltarColumns);

endfunction

//-----------------------------------------------------------------------------

function CheckAltarDelay()

  if(GetObjProperty(AltarCenter,"AltarDelay") and Cint(GetObjProperty(AltarCenter,"AltarDelay")) > ReadGameClock())
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanAccessAltar(AltarCenter, who)

  if(CanControlHouse(AltarCenter, who))
    return 1;
  elseif(FindAltarOwner(AltarCenter) == who)
    return 1;
  elseif(IsGm(who))
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
