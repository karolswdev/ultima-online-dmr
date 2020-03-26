//--------------------------------------------------------------------------------

include "include/gumps";
include "include/client/skills";

//================================================================================
//--------------------------------------------------------------------------------
// Merchant Skill Gump
//--------------------------------------------------------------------------------
//================================================================================

const SK_MENU_PPAGE_X        := 32;
const SK_MENU_NPAGE_X        := 453;
const SK_MENU_BT_X           := 55;
const SK_MENU_BT_MAX_X       := 430;
const SK_MENU_UP_Y           := 121;
const SK_MENU_DOWN_Y         := 351;

const GUMPPIC_CRAFTDOT       := 0xD8;

//--------------------------------------------------------------------------------
// globalny struct zawierajacy info dla gumpa umow
//--------------------------------------------------------------------------------

var SkillGlob;

//--------------------------------------------------------------------------------

function InitSkillGlob(who, cfgName, skillId)

  SkillGlob := struct;
  SkillGlob.+curSkill   := GetEffectiveSkill(who, skillId);
  SkillGlob.+cfg        := ReadConfigFile(cfgName);
  SkillGlob.+avObjTypes := array;
  SkillGlob.+itemCfg    := ReadConfigFile(ITEMS_CFG);
  SkillGlob.+expPropName := 0;
  SkillGlob.+expCraftDot := 1;

  SkillGlob.+CraftReceptures := GetCraftReceptures(who, SkillID);

endfunction

//--------------------------------------------------------------------------------

function BuildMainGump()

  GInit(60, 0);
  GNoDispose();
  GNoClose();
  GPage();
  
  GResPic(0, 60, 2600, 500, 400);
  GResPic(25, 119, 5100, 450, 25);
  GResPic(25, 349, 5100, 450, 25);

endfunction

//--------------------------------------------------------------------------------

function BuildGreyMainGump()

  GInit( 30 , 30 );
  GPage();
  
  GResPic(0, 0, 3600, 510, 440);
  GGumppic (10,  50,  3607);
  GGumppic (130, 50,  3607);
  GGumppic (250, 50,  3607);
  GGumppic (373, 50,  3607);
  GGumppic (10,  100, 3607);
  GGumppic (130, 100, 3607);
  GGumppic (250, 100, 3607);
  GGumppic (373, 100, 3607);
  GGumppic (10,  320, 3607);
  GGumppic (130, 320, 3607);
  GGumppic (250, 320, 3607);
  GGumppic (373, 320, 3607);
  GGumppic (10,  370, 3607);
  GGumppic (130, 370, 3607);
  GGumppic (250, 370, 3607);
  GGumppic (373, 370, 3607);


endfunction

//--------------------------------------------------------------------------------

function IsSkillReceptureAvail(ObjType)

  if(CInt(SkillGlob.cfg[ObjType].CraftLevel) < CRAFT_RECP_MIN_LEVEL)
    return 1;
  elseif(ObjType in SkillGlob.CraftReceptures)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GCraftGumpDot(x, y, CraftLevel)

  if(CraftLevel >= 1 and CraftLevel <= 4)
    GGumpPic(x, y, GUMPPIC_CRAFTDOT, GetColorForCraftLevel(CraftLevel));
  endif

endfunction

//--------------------------------------------------------------------------------

function AddGumpElem( ObjType, TxtX, TxtY, PicX, PicY, ButX, ButY, Desc := "", Pic := 0, DColor := 0, ButPic := 210, ButPicC := 211 )

  if(!SkillGlob)
    return 0;
  endif

  if(!Desc)
    Desc := GetDescForItem(ObjType);
  endif
  
  if(Desc == "None")
    Desc := "";
  endif
  
  if(!Pic)
    Pic := GetGraphicForItem(ObjType);
  endif

  var color := DColor;
  var ItemColor := 999;
  var CraftLevel := CInt(SkillGlob.cfg[ObjType].CraftLevel);

  if(CraftLevel >= CRAFT_RECP_MIN_LEVEL and !(ObjType in SkillGlob.CraftReceptures))
    color := 999;
    GTilePic(ButX-15, ButY, 0xE39);
  elseif(SkillGlob.curSkill + 15 >= Cint(SkillGlob.cfg[ObjType]."skill")) 
    color := 900;
    GButton(ButX, ButY, ButPic, ButPicC, ObjType);

    var expProp := 1;

    if(SkillGlob.expPropName)
      expProp := GetConfigInt(SkillGlob.cfg[ObjType], SkillGlob.expPropName);
    endif

    if(SkillGlob.expCraftDot and expProp)
      GCraftGumpDot(ButX+12, ButY-4, CraftLevel);
    endif

    SkillGlob.avObjTypes.append(ObjType);
    ItemColor := CInt(SkillGlob.itemCfg[ObjType].Color);
  endif

  GTextLine( TxtX, TxtY, color, Desc);
  if(PicX and PicY)
    GTilePicHue( PicX, PicY, Pic, ItemColor);
  endif
  
endfunction

//--------------------------------------------------------------------------------

function GetColorForCraftLevel(Level)

  case(Level)
    1:  return 52;
    2:  return 90;
    3:  return 68;
    4:  return 47;
  endcase

endfunction 

//--------------------------------------------------------------------------------

function BuildSkMenuButton(x, y, Id, Name, gType)

  case(gType)
    1:  GButton(x, y+2, 5209, 5209,  Id);
        GTextLine( x+20, y, 0, Name);

    2:  GButton(x, y-48, 2710, 2711,  Id);
        GTextLine( x+20, y-50, 500, Name);
  endcase

endfunction

//--------------------------------------------------------------------------------

function BuildSkillMenu(byref mButtons, gType := 1)

  var puPage := 0, pdPage := 0;
  var nuPage := 1, ndPage := 1;
  var cButtons;

  while(mButtons.Up.size() or mButtons.Down.size())
    
    GPage();

    cButtons := FilterSkMenuButtons(mButtons.Up);

    foreach Button in cButtons
      BuildSkMenuButton(Button[1], SK_MENU_UP_Y, Button[2], Button[3], gType);
    endforeach
    
    cButtons := FilterSkMenuButtons(mButtons.Down);

    foreach Button in cButtons
      BuildSkMenuButton(Button[1], SK_MENU_DOWN_Y, Button[2], Button[3], gType);
    endforeach

    BuildSkPageButtons(nuPage, puPage, SK_MENU_UP_Y, mButtons.Up.size() > 0);
    BuildSkPageButtons(ndPage, pdPage, SK_MENU_DOWN_Y, mButtons.Down.size() > 0);
  endwhile

endfunction

//--------------------------------------------------------------------------------

function BuildSkPageButtons(byref nPage, byref pPage, y, IsNext)

  if(nPage > 1)
    pPage := nPage - 1;
  endif

  if(IsNext)
    nPage += 1;
  else
    nPage := 0;
  endif

  if(pPage)
    GButtonPage(SK_MENU_PPAGE_X, y+3, 0x15E3, 0x15E7, pPage);
  endif

  if(nPage)
    GButtonPage(SK_MENU_NPAGE_X, y+3, 0x15E1, 0x15E5, nPage);
  endif

endfunction

//--------------------------------------------------------------------------------

function FilterSkMenuButtons(byref Buttons)

  var retButs := array;
  var retBut, Width, x := SK_MENU_BT_X, cMax := SK_MENU_BT_X;
  var Temp := Buttons;

  foreach Button in Temp
    Width := 20 + CalcSkMenuWidth(Button);

    if(x + Width > SK_MENU_BT_MAX_X)
      break;
    endif

    retBut    := array;
    retBut[1] := x;
    retBut[2] := _Button_iter;
    retBut[3] := Button;

    Buttons.erase(_Button_iter);
    retButs.append(retBut);

    x    += Width;
    cMax := x;
  endforeach

  foreach ret in retButs
    ret[1] := SK_MENU_BT_X + CInt(CDbl(ret[1]-SK_MENU_BT_X) / (cMax-SK_MENU_BT_X) * (SK_MENU_BT_MAX_X-SK_MENU_BT_X));
  endforeach

  return retButs;

endfunction

//--------------------------------------------------------------------------------

function CalcSkMenuWidth(Name)

  return CInt(len(Name) * 6.8);

endfunction

//--------------------------------------------------------------------------------
