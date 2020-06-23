//--------------------------------------------------------------------------------

include "include/objtypes/magic";
include "include/objtypes/skills";
include "include/objtypes/misc";

//================================================================================
//--------------------------------------------------------------------------------
// Ingots
//--------------------------------------------------------------------------------
//================================================================================

function GetIngObjByColor(Color)

  var ObjTypes := GetIngotsObjTypes();
  var ItemCfg  := ReadConfigFile(ITEMS_CFG);

  foreach Ingot in ObjTypes
    if(CInt(ItemCfg[Ingot].Color) == Color)
      return CInt(Ingot);
    endif
  endforeach

  return ObjTypes[1];

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Woods
//--------------------------------------------------------------------------------
//================================================================================

function GetLogObjTypes()

  return {
    UOBJ_NORMALLOG,
    UOBJ_CEDARLOG,
    UOBJ_OAKLOG,
    UOBJ_WALNUTLOG,
    UOBJ_WILLOWLOG,
    UOBJ_CYPRESSLOG,
    UOBJ_YEWLOG,
    UOBJ_OHIILOG,
    UOBJ_MYSTICLOG
  };

endfunction

//--------------------------------------------------------------------------------

function GetBoardObjTypes()

  return {
    UOBJ_NORMALBOARD,
    UOBJ_CEDARBOARD,
    UOBJ_OAKBOARD,
    UOBJ_WALNUTBOARD,
    UOBJ_WILLOWBOARD,
    UOBJ_CYPRESSBOARD,
    UOBJ_YEWBOARD,
    UOBJ_OHIIBOARD,
    UOBJ_MYSTICBOARD
  };

endfunction

//--------------------------------------------------------------------------------

function GetAllWood()

  var Woods := {};
  foreach wood in GetLogObjTypes()
    Woods.append(wood);
  endforeach
  foreach wood in GetBoardObjTypes()
    Woods.append(wood);
  endforeach

  return Woods;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Leather
//--------------------------------------------------------------------------------
//================================================================================


function GetHidesObjTypes()

  return {
    UOBJ_NORMALHIDE,
    UOBJ_BLACKHIDE,
    UOBJ_SNOWHIDE,
    UOBJ_ICEHIDE,
    UOBJ_STONEHIDE,
    UOBJ_FIREHIDE,
    UOBJ_MYSTICHIDE,
	UOBJ_DAEMONHIDE
  };

endfunction

//--------------------------------------------------------------------------------

function GetLeatherObjTypes()

  return {
    UOBJ_NORMALLEATHER,
    UOBJ_BLACKLEATHER,
    UOBJ_SNOWLEATHER,
    UOBJ_ICELEATHER,
    UOBJ_STONELEATHER,
    UOBJ_FIRELEATHER,
    UOBJ_MYSTICLEATHER,
	UOBJ_DAEMONLEATHER
  };

endfunction

//--------------------------------------------------------------------------------

function GetLeatherObjByColor(Color)

  var ObjTypes := GetLeatherObjTypes();
  var ItemCfg  := ReadConfigFile(ITEMS_CFG);

  foreach LeatObj in ObjTypes
    if(CInt(ItemCfg[LeatObj].Color) == Color)
      return CInt(LeatObj);
    endif
  endforeach

  return ObjTypes[1];

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Locks
//--------------------------------------------------------------------------------
//================================================================================


function GetLockAbleItems()

  var ObjTypes := {},i;

  ObjTypes.append(0xe79);
  for(i:=0x9a8;i<=0x9ab;i:=i+1)
    ObjTypes.append(i);
  endfor

  for(i:=0xe3c;i<=0xe43;i:=i+1)
    ObjTypes.append(i);
  endfor

  for(i:=0xe7c;i<=0xe80;i:=i+1)
    ObjTypes.append(i);
  endfor

  return ObjTypes;

endfunction

//--------------------------------------------------------------------------------

function GetKeysObjTypes()

  var ObjTypes := {},i;

  for(i:=UOBJ_KEYSTART;i<=UOBJ_KEYEND;i:=i+1)
    ObjTypes.append(i);
  endfor

  return ObjTypes;

endfunction

//--------------------------------------------------------------------------------

function GetDoorsObjTypes()

  var ObjTypes := {},i;

  for(i:=UOBJ_IDOORS_START;i<=UOBJ_IDOORS_END;i:=i+1)
    ObjTypes.append(i);
  endfor
  for(i:=UOBJ_DOORS_START;i<=UOBJ_DOORS_END;i:=i+1)
    ObjTypes.append(i);
  endfor

  return ObjTypes;

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Potions
//--------------------------------------------------------------------------------
//================================================================================

function GetPoisonPotions()

  return {
    UOBJ_LESSER_POISON_POTION,
    UOBJ_POISON_POTION,
    UOBJ_GREATER_POISON_POTION,
    UOBJ_DEADLY_POISON_POTION
  };
  
endfunction
  
//--------------------------------------------------------------------------------

function GetCurePotions()

  return {
    UOBJ_LESSER_CURE_POTION,
    UOBJ_CURE_POTION,
    UOBJ_GREATER_CURE_POTION
  };

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Hairs & Beards
//--------------------------------------------------------------------------------
//================================================================================

function GetShortHairs()

  return {0x203B,0x2044,0x2045,0x2046,0x2048};

endfunction

//--------------------------------------------------------------------------------

function GetLongHairs()

  return {0x203C,0x203d,0x2047,0x2049,0x204A};

endfunction

//--------------------------------------------------------------------------------

function GetShortBeards()

  return {0x203F,0x204B,0x2040};

endfunction

//--------------------------------------------------------------------------------

function GetMustach()

  return {0x2040,0x2041,0x204D};

endfunction

//--------------------------------------------------------------------------------

function GetLongBeards()

  return {0x203E,0x204C};

endfunction

//--------------------------------------------------------------------------------

function GetBeardsObjTypes()

  return {
    0x2005,
    0x203E,
    0x203F,
    0x2040,
    0x2041,
    0x204B,
    0x204C
  };

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Carpets
//--------------------------------------------------------------------------------
//================================================================================

function GetCarpetsObjTypes()

  return {
    UOBJ_RRED_CARPET,
    UOBJ_RBLUE_CARPET,
    UOBJ_RBRONZE_CARPET,
    UOBJ_RBLUE2_CARPET,
    UOBJ_RBEZOWY_CARPET,
    UOBJ_RCBEZOWY_CARPET
  };

endfunction

//================================================================================
//--------------------------------------------------------------------------------
// Reagents
//--------------------------------------------------------------------------------
//================================================================================

function GetMainRegsObjTypes()

  return {
    UOBJ_BLACK_PEARL,
    UOBJ_BLOOD_MOSS,
    UOBJ_GARLIC,
    UOBJ_GINSENG,
    UOBJ_MANDRAKE_ROOT,
    UOBJ_NIGHTSHADE,
    UOBJ_SULFUROUS_ASH,
    UOBJ_SPIDER_SILK
  };

endfunction

//--------------------------------------------------------------------------------

function GetDruidRegObjTypes()

  return array(0xF25, 0xF7B, 0xF11, 0xF8F, 0xF8E, 0xF13, 0xF81, 0x9F1, 0x1766, 0xF85, 0x1422, 0xF83, 0xF84, 0xF80, 0xF7F, 0xF7E, 0xF90, 0xF7A, 0xF86, 0xF88);

endfunction

//--------------------------------------------------------------------------------

function GetWearWolfRegObjTypes()

  return array(0xF7B, 0xF8C, 0xF89, 0xF7C, 0xF0F, 0xF88, 0xA720, 0xA721, 0xA722, 0xF10, 0xF30);

endfunction

//--------------------------------------------------------------------------------

function GetVampireRegObjTypes()

  return array(0xF8C, 0xF86, 0xF7F, 0xF78, 0xF8A, 0xF89, 0x9F1, 0xF81, 0xF7B, 0xF7A, 0xF88, 0xA800, 0xF10, 0xF13, 0xF79, 0xF11, 0xF8D, 0x1BD1, 0xA806, 0xA807, 0xA808, 0xA809,0xA80A,0xA80B, 0xA80C, 0xA80D);

endfunction

//--------------------------------------------------------------------------------

function GetNecroRegObjTypes()

  return array(0xF8E, 0xF89, 0xF78, 0xF82, 0xF79, 0xF87, 0xF90, 0xF8F, 0xF7C, 0xF7E, 0xF83, 0xF81, 0xF7F);

endfunction

function GetSpecLootObjTypes()

  return array(0xde00, 0xde04, 0xde06, 0xde07, 0xde09, 0xde0a, 0xde0b, 0xde0c, 0xde0d, 0xde0e, 0xde0f, 0xde11, 0xde12, 0xde41, 0xde42, 0xde40, 30274, 0xDE43, 0xDE44, 0xDE45);

endfunction

//--------------------------------------------------------------------------------
/*
function IsDoorObjType(Obj)

  if((Obj >= UOBJ_DOORS_START) and (Obj <= UOBJ_DOORS_END))
    return 1;
  elseif(Obj >= 2105 and Obj <= 2120)
    return 1;
  elseif(Obj >= 2124 and Obj <= 2139)
    return 1;
  elseif(Obj >= 2150 and Obj <= 2165)
    return 1;
  endif

  return 0;

endfunction
*/
//--------------------------------------------------------------------------------
