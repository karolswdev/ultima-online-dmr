//-----------------------------------------------------------------------------

const PLOUGH_RANGE          := 15;
const UOBJ_HARROW_N         := 0x1507;
const UOBJ_HARROW_W         := 0x1505;

const UOBJ_DIRT             := 0xAB00;

const DIRT_COLOR_S1         := 0x0226;
const DIRT_COLOR_S2         := 0x0224;
const DIRT_COLOR_E          := 0x0227;

//Dirt
const DIRT_TIME             := 259200;
const DIRT_GROW_TIME        := 800;
const DIRT_DESTROY_TIME     := 900;

//-----------------------------------------------------------------------------

include ":housing:common";

//-----------------------------------------------------------------------------

function EmptyDirt(Dirt)

  if(CInt(GetObjProperty(Dirt,"Growing")) or CInt(GetObjProperty(Dirt,"GrowSer")))
    return 0;
  endif
  return 1;

endfunction

//-----------------------------------------------------------------------------

function SetDirtProd(Dirt,Prod,Grown)

  if(EmptyDirt(Dirt))
    SetObjProperty(Dirt,"Growing",Prod);
    SetObjProperty(Dirt,"SowTime",ReadGameClock());
    if(Grown)
      SetObjProperty(Dirt,"StayGrown",1);
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function IsFlower(item)

  var Cfg     := ReadConfigFile(":farming:sow");
  var All     := GetConfigStringKeys(Cfg);
  var Flowers := {};
  
  foreach Flow in All
    if(Cfg[Flow].StayGrown)
      Flowers.append(CInt(Cfg[Flow].Product));
    endif
  endforeach
  
  if(item.objtype in Flowers)
    return 1;
  endif
  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function DestroyFlower(who,item)

  if(CanControlHouse(item, who))
    DestroyItem(item);
  else
    SendSysMessage(who,"Nie mozesz tego zniszczyc.",FONT_NORMAL,COLOR_RED);
    return;
  endif

endfunction

//-----------------------------------------------------------------------------
